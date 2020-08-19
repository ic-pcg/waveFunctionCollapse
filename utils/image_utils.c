#include "image_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tiles.h"

#define OUT_DIR "./autorotations/"
#define MAX_NAME_LENGTH (100)

// Rotating tiles based on rotateId
// rotateId = 1 rotation by 90 degrees
// rotateId = 2 rotation by 180 degrees
// rotateId = 3 rotation by 270 degrees

Tile* rotateTile(Tile* tile, int rotateId, int id, char* path) {
  Tile* newTile = (Tile*)calloc(1, sizeof(Tile));
  newTile->id = id;
  newTile->name = (char*)calloc(MAX_NAME_LENGTH, sizeof(char));
  strcpy(newTile->name, tile->name);

  char out[1000] = OUT_DIR;
  char filename[15];
  char cmd[1000] = "convert ";
  strcat(cmd, path);

  if (rotateId == 1) {
    strcat(cmd, " -rotate 90 ");
    newTile->right = tile->top;
    newTile->bottom = tile->right;
    newTile->left = tile->bottom;
    newTile->top = tile->left;
  } else if (rotateId == 2) {
    strcat(cmd, " -rotate 180 ");
    newTile->right = tile->left;
    newTile->bottom = tile->top;
    newTile->left = tile->right;
    newTile->top = tile->bottom;
  } else if (rotateId == 3) {
    strcat(cmd, " -rotate 270 ");
    newTile->right = tile->bottom;
    newTile->bottom = tile->left;
    newTile->left = tile->top;
    newTile->top = tile->right;
  }

  sprintf(filename, "%d", id);

  strcat(filename, ".png");
  strcat(out, filename);
  strcat(cmd, out);
  system(cmd);
  newTile->file = fopen(out, "r");
  return newTile;
}

// Flipping tiles based on flipId
// flipId = 1 flipping image vertically
// flipId = 2 flipping image horizontally
// flipId = 3 both

Tile* flipTile(Tile* tile, int flipId, int id, char* path) {
  Tile* newTile = (Tile*)calloc(1, sizeof(Tile));
  newTile->id = id;
  newTile->name = (char*)calloc(MAX_NAME_LENGTH, sizeof(char));
  strcpy(newTile->name, tile->name);

  char out[1000] = OUT_DIR;
  char filename[15];
  char cmd[1000] = "convert ";
  strcat(cmd, path);

  if (flipId == 1) {
    strcat(cmd, " -flip ");
    newTile->right = tile->right;
    newTile->bottom = tile->top;
    newTile->left = tile->left;
    newTile->top = tile->bottom;
  } else if (flipId == 2) {
    strcat(cmd, " -flop ");
    newTile->right = tile->left;
    newTile->bottom = tile->bottom;
    newTile->left = tile->right;
    newTile->top = tile->top;
  }

  sprintf(filename, "%d", id);

  strcat(filename, ".png");
  strcat(out, filename);
  strcat(cmd, out);
  system(cmd);
  newTile->file = fopen(out, "r");
  return newTile;
}

void initialiseAutorotations(void) {
  // sets up the directory structure for storing autorotations

  system("mkdir -p " OUT_DIR);

  // cleaning up any leftover files and
  // supressing the no matches found message
  system("rm " OUT_DIR "* 2> /dev/null");
}

void cleanAutorotations(void) {
  system("rm -rf " OUT_DIR);
  printf("autorotations cleaned successfully\n");
}
