#include "run_wfc.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "graphics/drawingTiles.h"
#include "graphics/graphicsRunner.h"
#include "utils/image_utils.h"
#include "utils/matrix_utils.h"
#include "utils/parse_xml.h"
#include "utils/superposition.h"
#include "utils/tiles.h"
#include "utils/wave_function_collapse.h"

#define MAX_CONSTRAINTS (100)

#define WINDOW_HEIGHT 1000
#define WINDOW_WIDTH 1000

#define SLEEP_NS 5000000

#define SAVE_RESULT_IMAGE true
#define FILE_NAME "./sample.jpg\0"

// can set the drawing style here, try if you wish
const DrawingStyle DRAWING_STYLE = AverageColor;

static void* wfcWrapper(void* void_arguments);

// entry point
int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage %s filename\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char* filename = argv[1];
  Tile** tileTypes = (Tile**)calloc(MAX_TILE_TYPES, sizeof(Tile*));
  int tileTypesLength = 0;
  int sizeX = 0;
  int sizeY = 0;
  Constraint** constraints =
      (Constraint**)calloc(MAX_CONSTRAINTS, sizeof(Constraint));
  int constraintsLength = 0;

  // interpreting the input
  parseXml(filename, tileTypes, &tileTypesLength, &sizeX, &sizeY, constraints,
           &constraintsLength);

  if (tileTypesLength > MAX_TILE_TYPES) {
    fprintf(stderr, "Too many tile types were specified in %s: %d (max %ld)\n",
            filename, tileTypesLength, MAX_TILE_TYPES);
    exit(EXIT_FAILURE);
  }

  Superposition** matrix =
      initialiseSuperpositionMatrix(sizeX, sizeY, tileTypesLength);

  // set the initial positions of the chosen tiles
  applyConstraints(matrix, constraints, constraintsLength, sizeX, sizeY);

  srand(time(0));

  initialiseGraphics(tileTypes, tileTypesLength, WINDOW_HEIGHT, WINDOW_WIDTH,
                     sizeX, sizeY, DRAWING_STYLE, matrix);

  WfcArguments_t wfcArguments;
  MainArguments_t main_arguments;

  wfcArguments.matrix = matrix;
  wfcArguments.sizeX = sizeX;
  wfcArguments.sizeY = sizeY;
  wfcArguments.tileTypes = tileTypes;
  wfcArguments.tileTypesLengthPt = &tileTypesLength;

  struct timespec timer;
  timer.tv_sec = 0;
  timer.tv_nsec = SLEEP_NS;
  wfcArguments.sleepTimer = &timer;

  main_arguments.argc = &argc;
  main_arguments.argv = &argv;

  pthread_t wfc_id, graphics_id;

  // concurrent part
  pthread_create(&wfc_id, NULL, wfcWrapper, (void*)&wfcArguments);
  pthread_create(&graphics_id, NULL, runGraphics, (void*)&main_arguments);

  bool wfcReturnValue;

  pthread_join(wfc_id, (void*)&wfcReturnValue);

  // check if valid solution found
  if (wfcReturnValue) {
    if (SAVE_RESULT_IMAGE) {
      saveResultImage(FILE_NAME);
    }

    printf("WFC terminated successfully\n");
#ifdef __APPLE__
    sleep(10);
#endif
    pthread_join(graphics_id, NULL);

    int** tileIdMatrix = initialiseTileMatrix(sizeX, sizeY, matrix);
    printTileMatrix(tileIdMatrix, sizeX, sizeY);

    freeTileMatrix(tileIdMatrix);
  } else {
    printf("WFC could not find a valid tiling\n");
    pthread_join(graphics_id, NULL);
  }

  freeGraphicsMemory();

  // freeing whats produced in the parser
  for (int i = 0; i < tileTypesLength; i++) {
    freeTile(tileTypes[i]);
  }
  free(tileTypes);

  freeSuperpositionMatrix(matrix);

  return EXIT_SUCCESS;
}

static void* wfcWrapper(void* void_arguments) {
  WfcArguments_t* wfcArguments = (WfcArguments_t*)void_arguments;
  bool returnValue;

  returnValue = waveFunctionCollapse(
      wfcArguments->matrix, wfcArguments->sizeX, wfcArguments->sizeY,
      wfcArguments->tileTypes, wfcArguments->tileTypesLengthPt,
      wfcArguments->sleepTimer);

  cleanAutorotations();
  return (void*)returnValue;
}
