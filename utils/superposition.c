#include "superposition.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

static bool tilePossible(int tile, Superposition superposition);

unsigned int entropy(Superposition superposition) {
  // ignoring the MSB 'recency' bit
  superposition &= BITS_EXCEPT_MSB;
  unsigned int count = 0;

  while (superposition) {
    count += superposition & 1;
    superposition >>= 1;
  }

#ifdef VERY_VERBOSE
  printf("entropy of 0x%016lx: %u\n", superposition, count);
#endif
  return count;
}

// prerequisite: the given superposition must be collapsed
// returns the id of the particulair tile type
int collapsedToWhich(Superposition superposition) {
  assert(entropy(superposition) == 1);

  for (int i = 0; superposition; i++, superposition >>= 1)
    if (superposition & 1) return i;

  assert(0);  // shouldn't get to this point
  return 0;
}

void setRecent(Superposition *superposition) {
  *superposition |= ((Superposition)1) << MAX_TILE_TYPES;
}

void setRecentAllUncollapsed(Superposition **matrix, unsigned int sizeX,
                             unsigned int sizeY) {
  for (int i = 0; i < sizeY; i++)
    for (int j = 0; j < sizeX; j++)
      if (entropy(matrix[i][j]) > 1) setRecent(&matrix[i][j]);
}

void unsetRecent(Superposition *superposition) {
  *superposition &= BITS_EXCEPT_MSB;
}

void clearRecentBitMatrix(Superposition **matrix, unsigned int sizeX,
                          unsigned int sizeY) {
  for (int i = 0; i < sizeY; i++)
    for (int j = 0; j < sizeX; j++) unsetRecent(&matrix[i][j]);
}

bool isRecent(Superposition superposition) {
  return superposition & (((Superposition)1) << MAX_TILE_TYPES);
}

static bool tilePossible(int tile, Superposition superposition) {
  return superposition & (((Superposition)1) << tile);
}

bool tilePossibleCoords(Superposition **matrix, int tile, Coords coords) {
  return tilePossible(tile, matrix[coords.y][coords.x]);
}

// returns the superposition collapsed to a random tile
Superposition collapseRandomly(Superposition superposition, int tileKinds) {
  unsigned int currentEntropy = entropy(superposition);

  // this function should only get uncollapsed superpositions
  assert(currentEntropy > 1);

  Superposition randomTile = (rand() % tileKinds);

  while (!tilePossible(randomTile, superposition))
    randomTile = rand() % tileKinds;

  Superposition result = ((Superposition)1) << randomTile;

  if (isRecent(superposition)) setRecent(&result);

#ifdef VERBOSE
  printf("collapsing 0x%016lx randomly to 0x%016lx\n", superposition, result);
#endif
  assert(entropy(result) == 1);
  return result;
}

void removePossibility(int possibility, Coords coords, Superposition **matrix) {
  matrix[coords.y][coords.x] &=
      ~((Superposition)0) & ~(((Superposition)1) << possibility);
}

bool allCollapsed(Superposition **const matrix, unsigned int sizeX,
                  unsigned int sizeY) {
  for (int i = 0; i < sizeY; i++)
    for (int j = 0; j < sizeX; j++)
      if (entropy(matrix[i][j]) > 1) return false;

  return true;
}

bool noConflicts(Superposition **const matrix, unsigned int sizeX,
                 unsigned int sizeY) {
  for (int i = 0; i < sizeY; i++)
    for (int j = 0; j < sizeX; j++)
      if (entropy(matrix[i][j]) == 0) return false;

  return true;
}
