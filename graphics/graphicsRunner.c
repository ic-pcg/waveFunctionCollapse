#include "graphicsRunner.h"

#include <MagickWand/MagickWand.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../utils/tiles.h"
#include "drawingTiles.h"

MagickWand* importTiles(Tile** tile_types);
float** TilesToPixels(MagickWand* wand);
void freePixels(float** pixels);
void display();
void populateTiles(float** tilesAsPixels);
void updateBuffer();
void addRotatedTiles(float** pixels, int currentTile);
void resetTile(int y, int x);
void addMiniTile(float** pixels, int currentTile, MagickWand* wand,
                 int miniTilesInRow);
void timer(int);

GLfloat** tiles;
GLfloat* buffer;
DrawingStyle drawingStyle;
int windowHeight;
int windowWidth;
int tileHeight;
int tileWidth;
int numOfTiles;
int tilesRepetition;
Superposition** matrix;
int matrixHeight;
int matrixWidth;

// PRE: numOfTilesInRow, numOfTilesInColumn, windowHeight, windowWidth > 0
void initialiseGraphics(Tile** tile_types, int tiles_length, int window_height,
                        int window_width, int numOfTilesInRow,
                        int numOfTilesInColumn, DrawingStyle style,
                        Superposition** input_matrix) {
  tileHeight = window_height / numOfTilesInColumn;
  tileWidth = window_width / numOfTilesInRow;
  numOfTiles = tiles_length;
  drawingStyle = style;
  matrix = input_matrix;
  matrixHeight = numOfTilesInColumn;
  matrixWidth = numOfTilesInRow;

  if (drawingStyle == TilesMatrix) {
    tilesRepetition = 2;
  } else {
    tilesRepetition = 1;
  }

  if (tileHeight > tileWidth) {
    tileHeight = tileWidth;
  } else {
    tileWidth = tileHeight;
  }

  windowHeight = tileHeight * numOfTilesInColumn;
  windowWidth = tileWidth * numOfTilesInRow;

  MagickWand* wand = importTiles(tile_types);

  float** tilesAsPixels = TilesToPixels(wand);
  populateTiles(tilesAsPixels);
  freePixels(tilesAsPixels);

  buffer = malloc(sizeof(GLfloat) * windowHeight * windowWidth * 3);

  wand = DestroyMagickWand(wand);
  MagickWandTerminus();
}

// creates a window and updates it depending on current state of simulation
void* runGraphics(void* mainArguments) {
  MainArguments_t* mainArgs = (MainArguments_t*)mainArguments;

  glutInit(mainArgs->argc, *mainArgs->argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition((1920 - windowWidth) / 2, (1080 - windowHeight) / 2);

  glutCreateWindow("WFC Simulation");

  glutDisplayFunc(display);
  glutTimerFunc(0, timer, 0);

  glutMainLoop();
  return NULL;
}

void display() {
  glLoadIdentity();

  updateBuffer();
  glDrawPixels(windowWidth, windowHeight, GL_RGB, GL_FLOAT, buffer);

  glutSwapBuffers();
}

void timer(int x) {
  glutPostRedisplay();
  int frameDuration = 1000 / 300;  // in ms
  glutTimerFunc(frameDuration, timer, 0);
}

// updates buffer to imitate tilemap
void updateBuffer() {
  for (int i = 0; i < matrixHeight; i++) {
    for (int j = 0; j < matrixWidth; j++) {
      resetTile(i * tileHeight, j * tileWidth);
      if (drawingStyle == AverageColor) {
        drawTileAverage(i * tileHeight, j * tileWidth);
      } else {
        drawTileMatrix(i * tileHeight, j * tileWidth);
      }
    }
  }
}

// converts floats from pixels to GLfloats and put them into tiles array
void populateTiles(float** pixels) {
  tiles = malloc(sizeof(GLfloat*) * numOfTiles * tilesRepetition);
  int height = tileHeight;
  int width = tileWidth;
  for (int i = 0; i < numOfTiles * tilesRepetition; i++) {
    if (i == numOfTiles) {
      int miniTilesInRow = integerSqrt(numOfTiles);
      height = tileHeight / miniTilesInRow;
      width = tileWidth / miniTilesInRow;
    }
    tiles[i] = malloc(sizeof(GLfloat) * 3 * width * height);
    for (int j = 0; j < height; j++) {
      for (int k = 0; k < width; k++) {
        int pos = j * width * 3 + k * 3;
        tiles[i][pos] = pixels[i][(height - 1 - j) * width * 3 + k * 3];
        tiles[i][pos + 1] = pixels[i][(height - 1 - j) * width * 3 + k * 3 + 1];
        tiles[i][pos + 2] = pixels[i][(height - 1 - j) * width * 3 + k * 3 + 2];
      }
    }
  }
}

MagickWand* importTiles(Tile** tileTypes) {
  MagickWand* wand;

  MagickWandGenesis();
  wand = NewMagickWand();

  int i;

  for (i = 0; i < numOfTiles; i++) {
    MagickSetLastIterator(wand);
    MagickBooleanType status = MagickReadImageFile(wand, tileTypes[i]->file);
    if (status == MagickFalse) {
      perror("problem with interprating file as image\n");
      exit(EXIT_FAILURE);
    }
  }

  return wand;
}

int integerSqrt(int number) {
  int begin = 0, end = number, i = 0;
  while (begin != end) {
    i = (begin + end) / 2;
    if (i * i < number) {
      begin = i + 1;
    } else {
      end = i;
    }
  }
  return begin;
}

float** TilesToPixels(MagickWand* wand) {
  MagickResetIterator(wand);
  MagickNextImage(wand);
  float** pixels = malloc(numOfTiles * sizeof(float*) * tilesRepetition);

  assert(numOfTiles > 0);
  int miniTilesInRow = integerSqrt(numOfTiles);

  for (int i = 0; i < numOfTiles; i++) {
    if (MagickGetImageWidth(wand) != tileWidth ||
        MagickGetImageHeight(wand) != tileHeight) {
      MagickResizeImage(wand, tileWidth, tileHeight, BoxFilter);
    }

    pixels[i] = malloc(sizeof(float) * tileHeight * tileWidth * 3);
    MagickExportImagePixels(wand, 0, 0, tileWidth, tileHeight, "RGB",
                            FloatPixel, pixels[i]);

    if (drawingStyle == TilesMatrix) {
      addMiniTile(pixels, i, wand, miniTilesInRow);
    }

    MagickNextImage(wand);
  }
  MagickResetIterator(wand);
  return pixels;
}

// resizes current image and exports it to pixels[currentile]
void addMiniTile(float** pixels, int currentTile, MagickWand* wand,
                 int miniTilesInRow) {
  assert(numOfTiles > 0);

  int newWidth = tileWidth / miniTilesInRow;
  int newHeight = tileHeight / miniTilesInRow;

  MagickResizeImage(wand, newWidth, newHeight, BoxFilter);

  pixels[currentTile + numOfTiles] =
      malloc(sizeof(float) * newWidth * newHeight * 3);
  MagickExportImagePixels(wand, 0, 0, newWidth, newHeight, "RGB", FloatPixel,
                          pixels[currentTile + numOfTiles]);
}

// not used at the moment
void addRotatedTile(float** pixels, int currentTile) {
  pixels[currentTile + numOfTiles] =
      malloc(sizeof(float) * tileHeight * tileWidth * 3);
  pixels[currentTile + numOfTiles * 2] =
      malloc(sizeof(float) * tileHeight * tileWidth * 3);
  pixels[currentTile + numOfTiles * 3] =
      malloc(sizeof(float) * tileHeight * tileWidth * 3);

  for (int i = 0; i < tileHeight; i++) {
    for (int j = 0; j < tileWidth; j++) {
      int pos = currentTile + numOfTiles;
      pixels[pos][(tileWidth - 1 - i + j * tileHeight) * 3] =
          pixels[currentTile][(i * tileWidth + j) * 3];
      pixels[pos][(tileWidth - 1 - i + j * tileHeight) * 3 + 1] =
          pixels[currentTile][(i * tileWidth + j) * 3 + 1];
      pixels[pos][(tileWidth - 1 - i + j * tileHeight) * 3 + 2] =
          pixels[currentTile][(i * tileWidth + j) * 3 + 2];

      pos += numOfTiles;
      pixels[pos][(tileWidth - 1 - j + (tileHeight - 1 - i) * tileWidth) * 3] =
          pixels[currentTile][(i * tileWidth + j) * 3];
      pixels[pos][(tileWidth - 1 - j + (tileHeight - 1 - i) * tileWidth) * 3 +
                  1] = pixels[currentTile][(i * tileWidth + j) * 3 + 1];
      pixels[pos][(tileWidth - 1 - j + (tileHeight - 1 - i) * tileWidth) * 3 +
                  2] = pixels[currentTile][(i * tileWidth + j) * 3 + 2];

      pos += numOfTiles;
      pixels[pos][(i + (tileWidth - 1 - j) * tileHeight) * 3] =
          pixels[currentTile][(i * tileWidth + j) * 3];
      pixels[pos][(i + (tileWidth - 1 - j) * tileHeight) * 3 + 1] =
          pixels[currentTile][(i * tileWidth + j) * 3 + 1];
      pixels[pos][(i + (tileWidth - 1 - j) * tileHeight) * 3 + 2] =
          pixels[currentTile][(i * tileWidth + j) * 3 + 2];
    }
  }
}

void freePixels(float** pixels) {
  for (int i = 0; i < numOfTiles * tilesRepetition; i++) {
    free(pixels[i]);
  }

  free(pixels);
}

void freeGraphicsMemory() {
  free(buffer);

  for (int i = 0; i < numOfTiles * tilesRepetition; i++) {
    free(tiles[i]);
  }

  free(tiles);
}