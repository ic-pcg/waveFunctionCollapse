#ifndef SUPERPOSITION_H
#define SUPERPOSITION_H

#include <stdint.h>

#include "tiles.h"

#define MAX_TILE_TYPES (8 * sizeof(Superposition) - 1)

// useful for disregarding the 'recency' bit
#define BITS_EXCEPT_MSB (~0 & ~(((Superposition)1) << MAX_TILE_TYPES))

// nth bit encodes whether nth tile type is possible
typedef uint64_t Superposition;

// returns the entropy (i.e. the number of possibilities
// a given superposition allows)
// entropy=1 means the superposition has collapsed
//    (we know the exact tile which will occupy this spot)
// entropy=0 means no tiles match the given constraints
unsigned int entropy(Superposition superposition);

// prerequisite: the given superposition must be collapsed
int collapsedToWhich(Superposition superposition);

void setRecent(Superposition *superposition);

void setRecentAllUncollapsed(Superposition **matrix, unsigned int sizeX,
                             unsigned int sizeY);

void unsetRecent(Superposition *superposition);

void clearRecentBitMatrix(Superposition **matrix, unsigned int sizeX,
                          unsigned int sizeY);

bool isRecent(Superposition superposition);

// returns true iff the given superposition allows the given tile type
bool tilePossibleCoords(Superposition **matrix, int tile, Coords coords);

Superposition collapseRandomly(Superposition superposition, int tileKinds);

void removePossibility(int possibility, Coords coords, Superposition **matrix);

bool allCollapsed(Superposition **const matrix, unsigned int sizeX,
                  unsigned int sizeY);

bool noConflicts(Superposition **const matrix, unsigned int sizeX,
                 unsigned int sizeY);

#endif
