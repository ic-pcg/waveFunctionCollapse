#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <stdbool.h>

#include "superposition.h"
#include "tiles.h"

// TODO wrap the matrices in a struct with their dimensions to make function
// calls more succint?

// initialises the tile matrix on the heap
int **initialiseTileMatrix(unsigned int sizeX, unsigned int sizeY,
                           Superposition **superpositionMatrix);

void freeTileMatrix(int **matrix);

void printTileMatrix(int **matrix, unsigned int sizeX, unsigned int sizeY);

// initialises the superposition matrix on the heap
// sets all the tile types to be possible in each cell
Superposition **initialiseSuperpositionMatrix(unsigned int sizeX,
                                              unsigned int sizeY,
                                              int tileKinds);

void freeSuperpositionMatrix(Superposition **matrix);

void printSuperpositionMatrix(Superposition **matrix, unsigned int sizeX,
                              unsigned int sizeY);

void prettyPrintSuperpositionMatrix(Superposition **matrix, unsigned int sizeX,
                                    unsigned int sizeY);

Superposition *pointerFromCoords(Superposition **matrix, unsigned int sizeX,
                                 unsigned int sizeY, Coords coords);

// puts coords of the neighbor in given direction into out
// returns false iff the specified neighbor is out of bounds
bool getNeighborCoords(Superposition **const matrix, unsigned int sizeX,
                       unsigned int sizeY, Coords in, Direction direction,
                       Coords *out);

void applyConstraints(Superposition **matrix, Constraint **constraints,
                      int constraintsLength, unsigned int sizeX,
                      unsigned int sizeY);
#endif
