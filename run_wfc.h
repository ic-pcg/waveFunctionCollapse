#ifndef RUN_WFC_H
#define RUN_WFC_H

#include "utils/superposition.h"

typedef struct WfcArguments_t {
  Superposition **matrix;
  unsigned int sizeX;
  unsigned int sizeY;
  Tile **tileTypes;
  int *tileTypesLengthPt;
  struct timespec *sleepTimer;
} WfcArguments_t;

#endif
