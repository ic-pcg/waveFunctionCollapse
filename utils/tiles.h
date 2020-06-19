#ifndef TILES_H
#define TILES_H

#include <stdbool.h>
#include <stdio.h>

typedef enum direction { TOP, RIGHT, BOTTOM, LEFT } Direction;

typedef struct coords {
  unsigned int x;
  unsigned int y;
} Coords;

// we will construct an array of these structs at the begining (parsing XML) an
// refer to it throughout the program, indexing arbitrary (eg - reading from
// file order)
typedef struct tile {
  int id;
  char* name;
  FILE* file;
  int top;
  int right;
  int bottom;
  int left;
} Tile;

typedef struct constraint {
  int id;
  Coords* coordinates;
} Constraint;

void freeTile(Tile* tile);

int tileRotation(Tile* tile);

// returns a char representation of the given tile
char prettyTile(int tile);

// returns true iff originTile can have neighborTile as its neighbour
// in the given direction ( v_n([x], [y], LEFT) <=> [yx] is valid )
bool validNeighbor(int originTile, int neighborTile, Direction direction,
                   Tile** tiles, int* tileTypesLengthPt);

#endif
