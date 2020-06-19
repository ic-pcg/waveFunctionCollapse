#ifndef WFC_H
#define WFC_H

#include <stdbool.h>
#include <time.h>

#include "superposition.h"
#include "tiles.h"

// returns true iff all the superpositions have been collapsed
// the result is written to the given matrix
bool waveFunctionCollapse(Superposition **const matrix, unsigned int sizeX,
                          unsigned int sizeY, Tile **tileTypes,
                          int *tileTypesLengthPt, struct timespec *sleepTimer);

// takes a matrix of collapsed superpositions
// writes the corresponding tiles for each superposition to tileMatrix
// returns false iff any of the superpositions were not collapsed
bool superpositionsToTiles(const Superposition **matrix, Tile **tileMatrix,
                           unsigned int sizeX, unsigned int sizeY);

#endif
