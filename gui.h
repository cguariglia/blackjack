#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "general.h"
#include "game.h"

#define STRING_SIZE 100       // max size for some strings
#define WIDTH_WINDOW 900      // window width
#define HEIGHT_WINDOW 525     // window height
#define CARD_WIDTH 67         // card width
#define CARD_HEIGHT 97        // card height
#define WINDOW_POSX 500       // initial position of the window: x
#define WINDOW_POSY 250       // initial position of the window: y
#define EXTRASPACE 150
#define MARGIN 5
#define MAX_PLAYERS 4         // maximum number of simultaneous active players
#define HOUSE_TYPE 2          

void init_everything(int, int, TTF_Font **, SDL_Surface **, SDL_Window **, SDL_Renderer **);
void init_SDL();
void init_font();
SDL_Window* create_window(int, int);
SDL_Renderer* create_renderer(int, int, SDL_Window *);
int render_text(int, int, const char*, TTF_Font *, SDL_Color *, SDL_Renderer *);
int render_logo(int, int, SDL_Surface *, SDL_Renderer *);
void render_table(int [], TTF_Font *, SDL_Surface **, SDL_Renderer *);
void render_card(int, int, int, SDL_Surface **, SDL_Renderer *);
void render_house_cards(int [], int, SDL_Surface **, SDL_Renderer *);
void render_player_cards(int [][MAX_CARD_HAND], int [], SDL_Surface **, SDL_Renderer *);
void load_cards(SDL_Surface **);
void unload_cards(SDL_Surface **);

#endif
