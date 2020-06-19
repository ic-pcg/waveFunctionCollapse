CC      = gcc
CFLAGS  = -Wall -O3 -std=c99 -Werror -pedantic `pkg-config --cflags --libs MagickWand` -lglut -lGL -pthread
BUILD 	= graphicsRunner.o drawingTiles.o
COMPONENTS = graphicsRunner.o drawingTiles.o ../tiles.h


all: $(BUILD)

graphicsRunner.o: graphicsRunner.h drawingTiles.h graphicsRunner.c ../utils/tiles.h
drawingTiles.o: graphicsRunner.h


clean:
	rm -f $(wildcard *.o) $(BUILD)
