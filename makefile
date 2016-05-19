# C compiler to use
CC=gcc

# general flags
FLAGS=-g -I/usr/local/include -Wall -pedantic -std=c99

# SDL related flags/options
SDL_FLAGS=-L/usr/local/lib -lm -lSDL2 -lSDL2_ttf -lSDL2_image

# name of the executable
EXE=burakkujaku

# files needed for compilation
FILES=main.c game.c gui.c general.c

all:
	$(CC) $(FILES) $(FLAGS) $(SDL_FLAGS) -o $(EXE)
