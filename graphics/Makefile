UNAME_S := $(shell uname -s)
CC      = gcc
ifeq ($(UNAME_S), Linux) #LINUX
	CFLAGS = -Wall -O3 -std=c99 -Werror -pedantic `pkg-config --cflags --libs MagickWand` -lglut -lGL -pthread
endif
ifeq ($(UNAME_S), Darwin) #APPLE
	CFLAGS = -Wall -O3 -std=c99 -pedantic `pkg-config --cflags --libs MagickWand` -pthread -Wno-deprecated-declarations -Wno-unused-command-line-argument
endif
BUILD 	= graphicsRunner.o drawingTiles.o
COMPONENTS = graphicsRunner.o drawingTiles.o ../tiles.h


all: $(BUILD)

graphicsRunner.o: graphicsRunner.h drawingTiles.h graphicsRunner.c ../utils/tiles.h
drawingTiles.o: graphicsRunner.h


clean:
	rm -f $(wildcard *.o) $(BUILD)
