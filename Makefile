CC       = gcc
CFLAGS   = -Wall -O0 -std=c99 -pedantic `pkg-config --cflags --libs MagickWand` -lglut -lGL -pthread -I/usr/include/libxml2 -lxml2 -g
BUILD    = run_wfc
VPATH 	 = utils
GRAPHICS = graphics/graphicsRunner.o graphics/drawingTiles.o


.SUFFIXES: .c .o

.PHONY: all clean

all: $(SPECIAL_OBJS) $(BUILD) 

run_wfc: run_wfc.c wave_function_collapse.o matrix_utils.o tiles.o superposition.o parse_xml.o image_utils.o $(GRAPHICS)
	gcc wave_function_collapse.o matrix_utils.o tiles.o superposition.o parse_xml.o run_wfc.c image_utils.o $(GRAPHICS) -o run_wfc $(CFLAGS)

wave_function_collapse.o: wave_function_collapse.h matrix_utils.o tiles.o superposition.o

matrix_utils.o: matrix_utils.h tiles.o superposition.o

tiles.o: tiles.h

superposition.o: superposition.h

image_utils.o: tiles.h image_utils.h

$(GRAPHICS): graphics/graphicsRunner.c graphics/drawingTiles.c
	cd graphics; make all

parse_xml.o: parse_xml.c tiles.h image_utils.h superposition.o
	gcc -Wall -g -O0 -std=c99 -pedantic -I/usr/include/libxml2 -c utils/parse_xml.c -lxml2

clean:
	rm -f $(wildcard *.o) $(BUILD)
	rm -rf autorotations/
	cd graphics; make clean

