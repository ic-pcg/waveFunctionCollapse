#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <stdbool.h>
#include <stdio.h>

#include "tiles.h"

Tile* rotateTile(Tile* tile, int rotateId, int id, char* path);

Tile* flipTile(Tile* tile, int flipId, int id, char* path);

void cleanAutorotations(void);

void initialiseAutorotations(void);

#endif
