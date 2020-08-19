/*
  For installation of libxml2 see:
  http://www.xmlsoft.org/FAQ.html
  (Might be that you will have to install sth more, the terminal will
  inform you)
  Compiles with:
  gcc -Wall -I/usr/include/libxml2 -o parseXml parseXml.c parseXml.h tiles.h
  -lxml2 -g disabling poiter sign errors
*/

#include "parse_xml.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image_utils.h"
#include "superposition.h"
#include "tiles.h"

#define MAX_NAME_LENGTH (50)
#define MAX_DIR_LENGHT (100)
#define MAX_EDGE_TYPES (100)
#define MAX_CONSTRAINTS (100)

static int edgeToId(char* edgeName, char** edges, int* edgesLengthPt);
static int tileToId(char* tileName, Tile** tileTypes, int* tileTypesLengthPt);

// parses an XML file
void parseXml(char* filename, Tile** tileTypes, int* tileTypesLengthPt,
              int* sizeXPt, int* sizeYPt, Constraint** constraints,
              int* constraintsLengthPt) {
  // creating the XML tree
  xmlDoc* document;
  xmlNode *root, *firstChild, *node;
  document = xmlReadFile(filename, NULL, 0);
  root = xmlDocGetRootElement(document);
  printf("Root is <%s> (%i)\n", root->name, root->type);

  // processing the root arguments: dir, size_x, size_y
  xmlAttr* propertyNode;
  char* dir = (char*)calloc(MAX_DIR_LENGHT, sizeof(char));

  for (propertyNode = root->properties; propertyNode;
       propertyNode = propertyNode->next) {
    // not relevant info
    if (!strcmp((const char*)propertyNode->name, "text")) {
      continue;

      // seting the local reference folder
    } else if (!strcmp((const char*)propertyNode->name, "dir")) {
      strcpy(dir, (char*)propertyNode->children->content);
      printf("We will look for tiles in: %s\n", dir);

      // horizontal size of the image (all tiles are assumed to be squares)
    } else if (!strcmp((const char*)propertyNode->name, "size_x")) {
      *sizeXPt = atoi((char*)propertyNode->children->content);

      // vertical size of the image
    } else if (!strcmp((const char*)propertyNode->name, "size_y")) {
      *sizeYPt = atoi((char*)propertyNode->children->content);
    }
  }

  // onto the tiles...
  firstChild = root->children;

  // this structure will translate from edge names to their ids
  char** edges = (char**)calloc(MAX_EDGE_TYPES, sizeof(char*));
  int edgesLength = 0;

  // temporary directory for rotated and flipped tiles
  initialiseAutorotations();

  // creating a list of tiles, constraints, and subsets
  printf("Reading tile types..\n");
  for (node = firstChild; node; node = node->next) {
    // not relevant info
    if (!strcmp((const char*)node->name, "text")) {
      continue;
    }

    // creating a list of tiles
    if (!strcmp((const char*)node->name, "tile")) {
      Tile* tile = (Tile*)calloc(1, sizeof(Tile));
      tile->id = *tileTypesLengthPt;
      tile->name = (char*)calloc(MAX_NAME_LENGTH, sizeof(char));

      // setting the properties of a tile
      xmlAttr* propertyNode;
      int rotate = 0;
      int flip = 0;
      char* path = (char*)calloc(MAX_DIR_LENGHT, sizeof(char));

      for (propertyNode = node->properties; propertyNode;
           propertyNode = propertyNode->next) {
        // not relevant info
        if (!strcmp((const char*)propertyNode->name, "text")) {
          continue;

          // FILE* and the name of the file
        } else if (!strcmp((const char*)propertyNode->name, "file")) {
          // creating the absolute directory
          strcat(path, dir);
          strcat(path, (char*)propertyNode->children->content);
          tile->file = fopen(path, "r");
          strcpy(tile->name, (char*)propertyNode->children->content);
#ifdef VERBOSE
          printf("%s - reading file\n", path);
#endif
          if (!tile->file) {
            fprintf(stderr, "%s - file not found!\n",
                    propertyNode->children->content);
          }

          // border types
        } else if (!strcmp((const char*)propertyNode->name, "top")) {
          tile->top = edgeToId((char*)propertyNode->children->content, edges,
                               &edgesLength);

        } else if (!strcmp((const char*)propertyNode->name, "right")) {
          tile->right = edgeToId((char*)propertyNode->children->content, edges,
                                 &edgesLength);

        } else if (!strcmp((const char*)propertyNode->name, "bottom")) {
          tile->bottom = edgeToId((char*)propertyNode->children->content, edges,
                                  &edgesLength);

        } else if (!strcmp((const char*)propertyNode->name, "left")) {
          tile->left = edgeToId((char*)propertyNode->children->content, edges,
                                &edgesLength);

          // rotation and flip properties
        } else if (!strcmp((const char*)propertyNode->name, "rotate")) {
          rotate = atoi((const char*)propertyNode->children->content);

        } else if (!strcmp((const char*)propertyNode->name, "flip")) {
          flip = atoi((const char*)propertyNode->children->content);
        }
      }

      tileTypes[*tileTypesLengthPt] = tile;
      (*tileTypesLengthPt)++;

      // creating rotated tiles
      // rotate = 1 rotation by 90 degrees
      // rotate = 2 rotation by 180 degrees
      // rotate = 3 rotation by 270 degrees
      for (int i = 0; i < rotate; i++) {
        Tile* currTile = rotateTile(tile, (i + 1), (*tileTypesLengthPt), path);
        tileTypes[*tileTypesLengthPt] = currTile;
        (*tileTypesLengthPt)++;
      }

      // flip = 1 flipping image vertically
      // flip = 2 flipping image horizontally
      // flip = 3 both
      // creating flipped tiles
      if (flip == 1 || flip == 3) {
        Tile* flippedTile = flipTile(tile, 1, (*tileTypesLengthPt), path);
        tileTypes[*tileTypesLengthPt] = flippedTile;
        (*tileTypesLengthPt)++;
      }

      // creating flipped tiles
      if (flip == 2 || flip == 3) {
        Tile* flippedTile = flipTile(tile, 2, (*tileTypesLengthPt), path);
        tileTypes[*tileTypesLengthPt] = flippedTile;
        (*tileTypesLengthPt)++;
      }

      free(path);
    }

    // processing the constraints
    else if (!strcmp((const char*)node->name, "constraint")) {
      xmlAttr* propertyNode;
      Constraint* constraint = (Constraint*)calloc(1, sizeof(Constraint));
      constraint->coordinates = (Coords*)calloc(1, sizeof(Coords));

      for (propertyNode = node->properties; propertyNode;
           propertyNode = propertyNode->next) {
        if (!strcmp((const char*)propertyNode->name, "text")) {
          continue;

        } else if (!strcmp((const char*)propertyNode->name, "file")) {
          constraint->id = tileToId((char*)propertyNode->children->content,
                                    tileTypes, tileTypesLengthPt);

        } else if (!strcmp((const char*)propertyNode->name, "x")) {
          constraint->coordinates->x =
              atoi((const char*)propertyNode->children->content);

        } else if (!strcmp((const char*)propertyNode->name, "y")) {
          constraint->coordinates->y =
              atoi((const char*)propertyNode->children->content);
        }
      }
      printf("%d\n", constraint->id);
      constraints[*constraintsLengthPt] = constraint;
      (*constraintsLengthPt)++;
    }
  }

  if (*tileTypesLengthPt > MAX_TILE_TYPES) {
    fprintf(stderr, "Too many tile types were specified in %s: %d (max %ld)\n",
            filename, *tileTypesLengthPt, MAX_TILE_TYPES);
    exit(EXIT_FAILURE);
  }

  // freeing the xml tree structure
#ifdef VERBOSE
  printf("freeing the xml document...\n");
#endif
  xmlFreeDoc(document);
  free(dir);
  free(edges);
#ifdef VERBOSE
  printf("freeing done\n");
#endif

  // FREE ID TO EDGE LABELS AND CONSTRAINTS (THIS ONE LATER)

  printf("Finished parsing XML (%s) successfully!\n", filename);
}

// translates from edge laber (string) to id (int)
static int edgeToId(char* edgeName, char** edges, int* edgesLengthPt) {
  // lets see if we have already seen this edge label
  for (int i = 0; i < *edgesLengthPt; i++) {
    if (!strcmp(edgeName, edges[i])) {
      return i;
    }
  }

  // if not add it and return the new id
  edges[*edgesLengthPt] = edgeName;
  (*edgesLengthPt)++;
  return (*edgesLengthPt) - 1;
}

// translates a tile to the ID
static int tileToId(char* tileName, Tile** tileTypes, int* tileTypesLengthPt) {
  for (int i = 0; i < *tileTypesLengthPt; i++) {
    if (!strcmp(tileName, tileTypes[i]->name)) {
      return (tileTypes[i])->id;
    }
  }
  printf("Tile not recognised in tileToId!\n");
  exit(EXIT_FAILURE);
  return -1;
}
