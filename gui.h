#ifndef GUI_H
#define GUI_H

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
#define UPSIDE_DOWN_CARD 52
#define MAX_DECK_SIZE 52     

void init_everything(int, int, TTF_Font **, SDL_Surface **, SDL_Window **, SDL_Renderer **);
void init_SDL();
void init_font();
SDL_Window* create_window(int, int);
SDL_Renderer* create_renderer(int, int, SDL_Window *);
int render_text(int, int, const char*, TTF_Font *, SDL_Color *, SDL_Renderer *);
int render_logo(int, int, SDL_Surface *, SDL_Renderer *);
void render_table(playerlist, TTF_Font *, SDL_Surface **, SDL_Renderer *);
void render_card(int, int, int, SDL_Surface **, SDL_Renderer *);
void render_house_cards(player, SDL_Surface **, SDL_Renderer *);
void render_player_cards(playerlist, SDL_Surface **, SDL_Renderer *);
void load_cards(SDL_Surface **);
void unload_cards(SDL_Surface **);
void render_overlay(TTF_Font *, SDL_Renderer *, playerlist);

#endif
