#include "tiles.h"

#include <stdio.h>
#include <stdlib.h>

// the first char of the name
char prettyTile(int tile) { return tile + '0'; }

static Tile* findTile(Tile** tiles, int id, int* tileTypesLengthPt) {
  for (int i = 0; i < *tileTypesLengthPt; i++)
    if (id == tiles[i]->id) return tiles[i];

  return NULL;
}

bool validNeighbor(int originTileId, int neighborTileId, Direction direction,
                   Tile** tiles, int* tileTypesLengthPt) {
  Tile* tileOrigin = findTile(tiles, originTileId, tileTypesLengthPt);
  Tile* tileNeighbor = findTile(tiles, neighborTileId, tileTypesLengthPt);

  switch (direction) {
    case RIGHT:
      return tileOrigin->right == tileNeighbor->left;
    case LEFT:
      return tileOrigin->left == tileNeighbor->right;
    case TOP:
      return tileOrigin->top == tileNeighbor->bottom;
    case BOTTOM:
      return tileOrigin->bottom == tileNeighbor->top;
    default:
      return false;
  }
}

void freeTile(Tile* tile) {
  free(tile->name);
  fclose(tile->file);
  free(tile);
}
