#ifndef GRAPHICSRUNNER_H
#define GRAPHICSRUNNER_H

#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
//#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdio.h>
#include "../utils/superposition.h"
#include "../utils/tiles.h"
#include "drawingTiles.h"

extern GLfloat** tiles;
extern GLfloat* buffer;
extern DrawingStyle drawingStyle;
extern int windowHeight;
extern int windowWidth;
extern int tileHeight;
extern int tileWidth;
extern int numOfTiles;
extern int tilesRepetition;
extern Superposition** matrix;
extern int matrixHeight;
extern int matrixWidth;

typedef struct MainArguments {
  int* argc;
  char*** argv;
} MainArguments_t;

void initialiseGraphics(Tile** tileTypes, int tiles_length, int window_height,
                        int window_width, int numOfTilesInRow,
                        int numOfTilesInColumn, DrawingStyle style,
                        Superposition** input_matrix);
void freeGraphicsMemory();
void* runGraphics(void* mainArguments);
int integerSqrt(int number);
void saveResultImage(const char* fileName);

#endif
