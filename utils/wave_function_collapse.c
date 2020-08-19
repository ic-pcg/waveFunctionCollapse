#include "wave_function_collapse.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix_utils.h"
#include "tiles.h"

static Coords findMinimumEntropy(Superposition **const matrix,
                                 unsigned int sizeX, unsigned int sizeY);

static bool updateNeighbors(Coords tile, Superposition **matrix,
                            unsigned int sizeX, unsigned int sizeY,
                            Tile **tileTypes, int *tileTypesLengthPt,
                            struct timespec *sleepTimer);

static bool stillPossible(Superposition **matrix, Coords tile,
                          Direction direction, int neighborType,
                          Tile **tileTypes, int *tileTypesLengthPt);

bool waveFunctionCollapse(Superposition **const matrix, unsigned int sizeX,
                          unsigned int sizeY, Tile **tileTypes,
                          int *tileTypesLengthPt, struct timespec *sleepTimer) {
  printf("*** starting WFC ***\n");
#ifdef VERBOSE
  printSuperpositionMatrix(matrix, sizeX, sizeY);
#endif

  Superposition *minimumSuperpos = NULL;

  while (!allCollapsed(matrix, sizeX, sizeY)) {
#ifdef VERBOSE
    printSuperpositionMatrix(matrix, sizeX, sizeY);
    printf("\n");
#endif

    // find the recently collapsed cells (with entropy=1)
    // and update their neighbors accordingly

    // if no cell was found to have entropy=1
    // collapse the lowest-entropy cell to a randomly chosen possibility

    Coords minimum = findMinimumEntropy(matrix, sizeX, sizeY);
    minimumSuperpos = pointerFromCoords(matrix, sizeX, sizeY, minimum);

#ifdef VERBOSE
    printf("Found minimum coords: y=%02u, x=%02u\n", minimum.y, minimum.x);
#endif

    if (entropy(*minimumSuperpos) == 0) {
      printf("found a 0-entropy cell at %p\n", (void *)minimumSuperpos);
      printf("no moves are possible, terminating\n");
      return false;
    }

    // collapse the min entropy tile to a random posibility
    // if no superposition is collapsed
    if (entropy(*minimumSuperpos) > 1)
      *minimumSuperpos = collapseRandomly(*minimumSuperpos, *tileTypesLengthPt);

    assert(entropy(*minimumSuperpos) == 1);

    clearRecentBitMatrix(matrix, sizeX, sizeY);

#ifdef VERBOSE
    printf("cleared recent bits\n");
    printSuperpositionMatrix(matrix, sizeX, sizeY);
    printf("\n");
#endif

    // if no tiles have been updated set all uncollapsed to 'recent'
    // otherwise entropy minimum won't be found
    if (!updateNeighbors(minimum, matrix, sizeX, sizeY, tileTypes,
                         tileTypesLengthPt, sleepTimer))
      setRecentAllUncollapsed(matrix, sizeX, sizeY);

#ifdef VERBOSE
    printf("*** neighbors updated ***\n\n");
    printSuperpositionMatrix(matrix, sizeX, sizeY);
    printf("\n");
#endif
  }

  // check if there's a 0 entropy cell
  return noConflicts(matrix, sizeX, sizeY);
}

static Coords findMinimumEntropy(Superposition **const matrix,
                                 unsigned int sizeX, unsigned int sizeY) {
  // larger than any possible value of entropy
  unsigned int smallestEntropyYet = 1025;
  unsigned int equalFound = 0;
  Coords *minima = (Coords *)calloc(sizeX * sizeY, sizeof(Coords));

  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      unsigned int entropyTmp = entropy(matrix[i][j]);
      if (entropyTmp == 0) {
        free(minima);
        Coords result;
        result.y = i;
        result.x = j;
        return result;
      }

      if (isRecent(matrix[i][j])) {
        if (entropyTmp == smallestEntropyYet) {
          minima[equalFound].y = i;
          minima[equalFound].x = j;
          equalFound++;
        } else if (entropyTmp < smallestEntropyYet) {
          smallestEntropyYet = entropyTmp;
          minima[0].y = i;
          minima[0].x = j;
          equalFound = 1;
        }
      }
    }
  }

  // pick random coords from minima[0..equalFound - 1]
  Coords result = minima[rand() % equalFound];
  free(minima);
  return result;
}

// iterates through the neighbors of tile, removing any possibilities
// that are now ruled out, setting any modified tiles to 'recent';
// recurses into the updated tile's neighbours
// returns true iff anything has been updated

static bool updateNeighbors(Coords tile, Superposition **matrix,
                            unsigned int sizeX, unsigned int sizeY,
                            Tile **tileTypes, int *tileTypesLengthPt,
                            struct timespec *sleepTimer) {
#ifdef VERBOSE
  printf("entering updateNeighbors (y=%02u, x=%02u)\n", tile.y, tile.x);
#endif

  bool modifiedAny = false;

  // iterate through the neighbors
  for (Direction direction = 0; direction < 4; direction++) {
    Coords neighbor = {sizeX, sizeY};  // dummy initialisation

    if (!getNeighborCoords(matrix, sizeX, sizeY, tile, direction, &neighbor))
      continue;  // skip to the next iteration if the neighbor is out of bounds

    bool modifiedThisNeighbor = false;
    for (int neighborType = 0; neighborType < *tileTypesLengthPt;
         neighborType++) {
      // if neighborType is set as possible but has actually been
      // ruled out by recent changes, update the superposition
      if (tilePossibleCoords(matrix, neighborType, neighbor) &&
          !stillPossible(matrix, tile, direction, neighborType, tileTypes,
                         tileTypesLengthPt)) {
        removePossibility(neighborType, neighbor, matrix);
        modifiedThisNeighbor = true;

        nanosleep(sleepTimer, NULL);
      }
    }

    if (modifiedThisNeighbor) {
      // recurse
      modifiedAny = true;
      setRecent(&matrix[neighbor.y][neighbor.x]);
      updateNeighbors(neighbor, matrix, sizeX, sizeY, tileTypes,
                      tileTypesLengthPt, sleepTimer);
    }
  }
  return modifiedAny;
}

static bool stillPossible(Superposition **matrix, Coords tile,
                          Direction direction, int neighborType,
                          Tile **tileTypes, int *tileTypesLengthPt) {
  for (int id = 0; id < *tileTypesLengthPt; id++)
    if (tilePossibleCoords(matrix, id, tile) &&
        validNeighbor(id, neighborType, direction, tileTypes,
                      tileTypesLengthPt))
      return true;

  return false;
}
