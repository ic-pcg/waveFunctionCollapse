#include <inttypes.h>

#include "graphicsRunner.h"

// add pixels values of tile at position x y of tilemap
void drawTileOver(GLfloat* tile, int y, int x, int tileHeight, int tileWidth) {
  for (int i = 0; i < tileHeight; i++) {
    for (int j = 0; j < tileWidth; j++) {
      buffer[(x + j + (y + i) * windowWidth) * 3] +=
          tile[(j + i * tileWidth) * 3];
      buffer[(x + j + (y + i) * windowWidth) * 3 + 1] +=
          tile[(j + i * tileWidth) * 3 + 1];
      buffer[(x + j + (y + i) * windowWidth) * 3 + 2] +=
          tile[(j + i * tileWidth) * 3 + 2];
    }
  }
}

// undecide tiles are drawn as average colors of possible tiles
void drawTileAverage(int y, int x) {
  int counter = 0;
  int setBits = 0;
  Superposition superposition =
      matrix[matrixHeight - 1 - y / tileHeight][x / tileWidth] &
      BITS_EXCEPT_MSB;
  for (Superposition i = superposition; i > 0; i >>= 1, counter++) {
    if (i & 1) {
      drawTileOver(tiles[counter], y, x, tileHeight, tileWidth);
      setBits++;
    }
  }

  for (int i = 0; i < tileHeight; i++) {
    for (int j = 0; j < tileWidth; j++) {
      buffer[(x + j + (y + i) * windowWidth) * 3] /= setBits;
      buffer[(x + j + (y + i) * windowWidth) * 3 + 1] /= setBits;
      buffer[(x + j + (y + i) * windowWidth) * 3 + 2] /= setBits;
    }
  }
}

void drawTileMatrix(int y, int x) {
  int miniTilesInRow = integerSqrt(numOfTiles);
  Superposition superposition =
      matrix[matrixHeight - 1 - y / tileHeight][x / tileWidth] & ~0 &
      BITS_EXCEPT_MSB;
  int setBits = entropy(superposition);

  int counter = 0;

  if (setBits == 1) {
    for (Superposition i = superposition; i > 0; i >>= 1, counter++)
      ;
    drawTileOver(tiles[counter - 1], y, x, tileHeight, tileWidth);
  } else {
    int miniTileHeight = tileHeight / miniTilesInRow;
    int miniTileWidth = tileWidth / miniTilesInRow;

    for (Superposition i = superposition; i > 0; i >>= 1, counter++) {
      if ((i & 1) == 1) {
        int yOffset =
            tileHeight - miniTileHeight * (counter / miniTilesInRow + 1);
        int xOffset = miniTileWidth * (counter % miniTilesInRow);
        drawTileOver(tiles[counter + numOfTiles], y + yOffset, x + xOffset,
                     miniTileHeight, miniTileWidth);
      }
    }
  }
}

// set all pixels white
void resetTile(int y, int x) {
  for (int i = 0; i < tileHeight; i++) {
    for (int j = 0; j < tileWidth; j++) {
      buffer[(x + j + (y + i) * windowWidth) * 3] = 0;
      buffer[(x + j + (y + i) * windowWidth) * 3 + 1] = 0;
      buffer[(x + j + (y + i) * windowWidth) * 3 + 2] = 0;
    }
  }
}
