#include "matrix_utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiles.h"

int **initialiseTileMatrix(unsigned int sizeX, unsigned int sizeY,
                           Superposition **superpositionMatrix) {
  int **matrix = (int **)calloc(sizeY, sizeof(int *));

  if (matrix == NULL) {
    fprintf(stderr, "Not enough memory to allocate the matrix\n");
    exit(EXIT_FAILURE);
  }

  matrix[0] = (int *)calloc(sizeX * sizeY, sizeof(int));

  if (matrix[0] == NULL) {
    free(matrix);
    fprintf(stderr, "Not enough memory to allocate the matrix\n");
    exit(EXIT_FAILURE);
  }

  // initialise the remaining row pointers
  for (int i = 1; i < sizeY; i++) matrix[i] = matrix[i - 1] + sizeX;

  // initialise the values
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      int k = 0;
      superpositionMatrix[i][j] <<= 1;
      superpositionMatrix[i][j] >>= 1;
      *(matrix[i] + j) = 0;
      while (superpositionMatrix[i][j]) {
        k++;

        if (superpositionMatrix[i][j] == 1) *(matrix[i] + j) = k;

        superpositionMatrix[i][j] >>= 1;
      }
    }
  }
  return matrix;
}

void freeTileMatrix(int **matrix) {
  free(matrix[0]);
  free(matrix);
}

void printTileMatrix(int **matrix, unsigned int sizeX, unsigned int sizeY) {
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) printf("%c\t ", prettyTile(matrix[i][j]));
    printf("\n");
  }
}

Superposition **initialiseSuperpositionMatrix(unsigned int sizeX,
                                              unsigned int sizeY,
                                              int tileKinds) {
  Superposition **matrix =
      (Superposition **)calloc(sizeY, sizeof(Superposition *));

  if (matrix == NULL) {
    fprintf(stderr, "Not enough memory to allocate the matrix\n");
    exit(EXIT_FAILURE);
  }

  matrix[0] = (Superposition *)calloc(sizeX * sizeY, sizeof(Superposition));

  if (matrix[0] == NULL) {
    free(matrix);
    fprintf(stderr, "Not enough memory to allocate the matrix\n");
    exit(EXIT_FAILURE);
  }

  // initialise the remaining row pointers
  for (int i = 1; i < sizeY; i++) matrix[i] = matrix[i - 1] + sizeX;

  printf("tileKinds = %d\n", tileKinds);
  // initialise the values
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      *(matrix[i] + j) = (((uint64_t)1) << tileKinds) - 1;

      // making sure all tiles are considered
      // by findMinEntropy at first iteration
      setRecent(matrix[i] + j);
    }
  }

  return matrix;
}

void freeSuperpositionMatrix(Superposition **matrix) {
  free(matrix[0]);
  free(matrix);
}

void printSuperpositionMatrix(Superposition **matrix, unsigned int sizeX,
                              unsigned int sizeY) {
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) printf("0x%016lx ", matrix[i][j]);
    printf("\n");
  }
}

void prettyPrintSuperpositionMatrix(Superposition **matrix, unsigned int sizeX,
                                    unsigned int sizeY) {
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      if (entropy(matrix[i][j]) == 1) {
        printf("%c ", prettyTile(collapsedToWhich(matrix[i][j])));
      } else if (entropy(matrix[i][j]) == 0) {
        printf("0 ");
      } else {
        printf("? ");
      }
    }
    printf("\n");
  }
}

Superposition *pointerFromCoords(Superposition **matrix, unsigned int sizeX,
                                 unsigned int sizeY, Coords coords) {
  assert(coords.x >= 0 && coords.y >= 0 && coords.x < sizeX &&
         coords.y < sizeY);

  return &matrix[coords.y][coords.x];
}

bool getNeighborCoords(Superposition **const matrix, unsigned int sizeX,
                       unsigned int sizeY, Coords in, Direction direction,
                       Coords *out) {
  unsigned int neighborX = in.x;
  unsigned int neighborY = in.y;

  switch (direction) {
    case TOP:
      neighborY--;
      break;
    case BOTTOM:
      neighborY++;
      break;
    case LEFT:
      neighborX--;
      break;
    case RIGHT:
      neighborX++;
      break;
    default:
      fprintf(stderr, "Invalid direction: %d\n", direction);
  }

  if (neighborX < 0 || neighborY < 0 || neighborX >= sizeX ||
      neighborY >= sizeY) {
    // making out's values invalid so that it is not erroneously used
    out->x = sizeX;
    out->y = sizeY;
    return false;
  }

  out->x = neighborX;
  out->y = neighborY;
  return true;
}

void applyConstraints(Superposition **matrix, Constraint **constraints,
                      int constraintsLength, unsigned int sizeX,
                      unsigned int sizeY) {
  if (constraintsLength > 0) clearRecentBitMatrix(matrix, sizeX, sizeY);

  for (int i = 0; i < constraintsLength; i++) {
    Superposition result = ((Superposition)1) << constraints[i]->id;
    setRecent(&result);
    printf("applied constraints[%d] (id=%d, y=%02u, x=%02u): 0x%016lx\n", i,
           constraints[i]->id, constraints[i]->coordinates->y,
           constraints[i]->coordinates->x, result);
    matrix[constraints[i]->coordinates->y][constraints[i]->coordinates->x] =
        result;
  }
}
