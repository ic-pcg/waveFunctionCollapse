#ifndef DRAWINGTILES_H
#define DRAWINGTILES_H

typedef enum drawingStyle { AverageColor, TilesMatrix } DrawingStyle;

// undecide tiles are drawn as average colors of possible tiles
void drawTileAverage(int y, int x);
// set all pixels white
void resetTile(int y, int x);
void drawTileMatrix(int y, int x);
#endif
