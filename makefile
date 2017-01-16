# C compiler to use
CC=gcc

# general flags
FLAGS=-g -I/usr/local/include -Wall -pedantic -std=c99

# SDL related flags/options
SDL_FLAGS=-L/usr/local/lib -lm -lSDL2 -lSDL2_ttf -lSDL2_image

# name of the executable
EXE=blackjack

# files needed for compilation
FILES=game.h gui.h general.h

# the objects made from the above files
OBJECTS=main.o game.o gui.o general.o

%.o: %.c $(FILES)
	$(CC) $(CFLAGS) -c -o $@ $< 

blackjack: $(OBJECTS)
	$(CC) $(FLAGS) $(SDL_FLAGS) -o $(EXE) $(OBJECTS)

clean:
	rm blackjack *.o
