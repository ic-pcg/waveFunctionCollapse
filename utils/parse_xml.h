#ifndef PARSEXML_H
#define PARSEXML_H

#include "tiles.h"

void parseXml(char* filename, Tile** tileTypes, int* tileTypesLengthPt,
              int* sizeXPt, int* sizeYPt, Constraint** constraints,
              int* constraintsLengthPt);

#endif
