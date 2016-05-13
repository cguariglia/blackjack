#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "gui.h"

// Renders the playing table(player squares, names, etc) and sidebar
void render_table(playerlist _players, TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer *_renderer) {
    SDL_Color white = {255, 255, 255}; // White
    SDL_Color red =  {255, 0, 0}; // Red
    char name_money_str[STRING_SIZE];
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, playerRect;
    int separatorPos =(int)(0.95f*WIDTH_WINDOW); // Seperates the left from the right part of the window
    //int height;
    p_node *cur;
    int i = 0;
    int height;

    // Set color of renderer to some color
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

    // Clear the window
    SDL_RenderClear(_renderer);

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;

    tableDest.w = separatorPos;
    tableDest.h = HEIGHT_WINDOW;
    
    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);
    
    // Render the IST Logo
    height = render_logo(separatorPos, 0, _img[1], _renderer);

    // Render the student name
    height += render_text(separatorPos+3*MARGIN, height, "THIS IS AN ALFAFA VERSION", _font, &red, _renderer);

    // Render the student number
    render_text(separatorPos+3*MARGIN, height, "BUY WHOLE GAME PLZ", _font, &red, _renderer);

    // Renders the squares + name for each player
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

    // Renders the areas for each player: names and money too !
    for(cur = _players.head; cur->p_data.type != HOUSE_TYPE; cur = cur->next) {
		if(cur->p_data.active == 0)
			continue;
		
		playerRect.x = i*(separatorPos/4-5)+10;
        playerRect.y =(int)(0.55f*HEIGHT_WINDOW);
        playerRect.w = separatorPos/4-5;
        playerRect.h =(int)(0.42f*HEIGHT_WINDOW);
        sprintf(name_money_str, "%s -- %d euros", cur->p_data.name, cur->p_data.money);
        
        // Renders the active player in red
        if(cur->p_data.status == 1) {
            SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
            render_text(playerRect.x+20, playerRect.y-30, name_money_str, _font, &red, _renderer);
        }
        // All other players are rendered in white
        else {
            SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
            render_text(playerRect.x+20, playerRect.y-30, name_money_str, _font, &white, _renderer);
        }
        SDL_RenderDrawRect(_renderer, &playerRect);
        
        i += 1;
    }
	
    // Destroy everything
    SDL_DestroyTexture(table_texture);
}

void render_house_cards(player _house, SDL_Surface **_cards, SDL_Renderer*_renderer) {
    int cards, x, y, i, hand_size;
    int div = WIDTH_WINDOW/CARD_WIDTH;
    c_node *cur;

	hand_size = _house.hand.size;

    // Drawing all house cards
    for(cards = 0; cards < hand_size; cards++) {
		cur = _house.hand.top;
		
		// 'walk' to card to render
		for(i = 0; i < hand_size - cards - 1; i++)
			cur = cur->next;
		
        // calculate its position
        x =(div/2-_house.hand.size/2+cards)*CARD_WIDTH + 15;
        y =(int)(0.26f*HEIGHT_WINDOW);
        
        // Makes the card render upside-down or right-side up
        if(_house.hand.size == 2 && cards == 1 && _house.active != 2)
            render_card(x, y, UPSIDE_DOWN_CARD, _cards, _renderer);
        else
            render_card(x, y, (cur->c_data.id - 1) + (13 * cur->c_data.suit), _cards, _renderer);
    }

}

void render_player_cards(playerlist players, SDL_Surface **_cards, SDL_Renderer*_renderer) {
    int pos, x, y, cards, num_player = 0;
    p_node *cur_player;
    c_node *cur_card;
    int i, card_code, hand_size;

    // For every card of every player
    for(cur_player = players.head; cur_player->p_data.type != HOUSE_TYPE; cur_player = cur_player->next) {
		if(cur_player->p_data.active == 0)
			continue;
			
		hand_size = cur_player->p_data.hand.size;
        for(cards = 0; cards < hand_size; cards++) {
			cur_card = cur_player->p_data.hand.top;
			
			// 'walk' to card to render
			for(i = 0; i < hand_size - cards - 1; i++)
				cur_card = cur_card->next;
			
            // Draw all cards of the player: calculate its position: only 4 positions are available!
            pos = cards % 4;
            x =(int) num_player*((0.95f*WIDTH_WINDOW)/4-5) + (cards/4)*12+15;
            y =(int)(0.55f*HEIGHT_WINDOW)+10;
            if(pos == 1 || pos == 3) x += CARD_WIDTH + 30;
            if(pos == 2 || pos == 3) y += CARD_HEIGHT + 10;
            
            card_code = (cur_card->c_data.id - 1) + (13 * cur_card->c_data.suit); 
            
            // Render it
            render_card(x, y, card_code, _cards, _renderer);
        }
        
        num_player += 1;
    }
}

void render_card(int _x, int _y, int _num_card, SDL_Surface **_cards, SDL_Renderer *_renderer) {
    SDL_Texture *card_text;
    SDL_Rect boardPos;

    // Area that will be occupied by each card
    boardPos.x = _x;
    boardPos.y = _y;
    boardPos.w = CARD_WIDTH;
    boardPos.h = CARD_HEIGHT;

    // Render it
    card_text = SDL_CreateTextureFromSurface(_renderer, _cards[_num_card]);
    SDL_RenderCopy(_renderer, card_text, NULL, &boardPos);

    // Destroy everything
    SDL_DestroyTexture(card_text);
}

void load_cards(SDL_Surface **_cards) {
    int i;
    char filename[STRING_SIZE];

     // Loads all cards to an array
    for(i = 0 ; i < MAX_DECK_SIZE; i++) {
        // Create the filename
        sprintf(filename, ".//cartas//carta_%02d.png", i + 1);
        // Loads the image
        _cards[i] = IMG_Load(filename);
        // Check for errors: deleted files
        if(_cards[i] == NULL) {
            printf("Unable to load image: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
    // Loads the card back
    _cards[i] = IMG_Load(".//cartas//carta_back.jpg");
    if(_cards[i] == NULL) {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void unload_cards(SDL_Surface **_array_of_cards) {
    // unload all cards of the memory: +1 for the card back
    for(int i = 0 ; i < MAX_DECK_SIZE + 1; i++) {
        SDL_FreeSurface(_array_of_cards[i]);
    }
}

int render_logo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer*_renderer)
{
	SDL_Texture *text_IST;
	SDL_Rect boardPos;

    // space occupied by the logo
	boardPos.x = x;
	boardPos.y = y;
	boardPos.w = _logoIST->w;
	boardPos.h = _logoIST->h;

    // render it
	text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
	SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
	SDL_DestroyTexture(text_IST);
	return _logoIST->h;
}

int render_text(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer*_renderer)
{
	SDL_Surface *text_surface;
	SDL_Texture *text_texture;
	SDL_Rect solidRect;

	solidRect.x = x;
	solidRect.y = y;
    // create a surface from the string text with a predefined font
	text_surface = TTF_RenderText_Blended(_font,text,*_color);
	if(!text_surface)
	{
	    printf("TTF_render_text_Blended: %s\n", TTF_GetError());
	    exit(EXIT_FAILURE);
	}
    // create texture
	text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
	SDL_QueryTexture(text_texture, NULL, NULL, &solidRect.w, &solidRect.h);
    // render it !
	SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);

	SDL_DestroyTexture(text_texture);
	SDL_FreeSurface(text_surface);
	return solidRect.h;
}

void init_everything(int width, int height, TTF_Font **_font, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer) {
    init_SDL();
    init_font();
    *_window = create_window(width, height);
    *_renderer = create_renderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if(_img[0] == NULL) {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if(_img[1] == NULL) {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens(loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 *init_SDL: Initializes the SDL2 graphic library
 */
void init_SDL()
	{
    // init SDL library
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
	}
}

/**
 *init_font: Initializes the SDL2_ttf font library
 */
void init_font()
	{
	// Init font library
	if(TTF_Init()==-1) {
	    printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
	}
}

/**
 *create_window: Creates a window for the application
 *\param width width in px of the window
 *\param height height in px of the window
 *\return pointer to the window created
 */
SDL_Window * create_window(int width, int height)
{
    SDL_Window *window;
    // init window
	window = SDL_CreateWindow("BlackJack", WINDOW_POSX, WINDOW_POSY, width+EXTRASPACE, height, 0);
    // check for error !
	if(window == NULL) {
		printf("Failed to create window : %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	return window;
}

/**
 *create_renderer: Creates a renderer for the application
 *\param width width in px of the window
 *\param height height in px of the window
 *\param _window represents the window for which the renderer is associated
 *\return pointer to the renderer created
 */
SDL_Renderer * create_renderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
	renderer = SDL_CreateRenderer(_window, -1, 0);

	if(renderer == NULL)
	{
		printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
	}

	// set size of renderer to the same as window
	SDL_RenderSetLogicalSize(renderer, width+EXTRASPACE, height);

	return renderer;
}

/**
 *Renders the word bust when the player has busted, 'no money' when the player has run out of money and 'game over' when that happens to all players.
 *\param money a player's current money
 *\param bet corresponding to each player 
**/
void render_overlay(TTF_Font *_font, SDL_Renderer *_renderer, playerlist players) {
    SDL_Rect bustRect;
    SDL_Color red =  {255, 0, 0}; 
    SDL_Color yellow = {255, 255, 51};
    SDL_Color green = {0, 201, 0};
    int separatorPos =(int)(0.95f*WIDTH_WINDOW);
    int i = 0;
    char busted[4];
    char no_play[9];
    char blackjack[10];
    p_node *cur_player;
   
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
   
    for(cur_player = players.head; cur_player->p_data.type != players.tail->p_data.type; cur_player = cur_player->next) {	
		if(cur_player->p_data.active == 1) {
			bustRect.y =(int)(0.55f*HEIGHT_WINDOW) + 100;
			// Renders "bust" if the player has busted
				if(has_blackjack(cur_player->p_data) == 1) {
					bustRect.x = i*(separatorPos/4-5)+60;
					bustRect.w = 110;
					bustRect.h = 20;
					SDL_RenderFillRect(_renderer, &bustRect);
					sprintf(blackjack, "BLACKJACK");
					render_text(bustRect.x + 7, bustRect.y - 3, blackjack, _font, &yellow, _renderer);
				}
				if(cur_player->p_data.points > 21) {
					bustRect.x = i*(separatorPos/4-5)+80;
					bustRect.w = 50;
					bustRect.h = 20;
					SDL_RenderFillRect(_renderer, &bustRect);
					sprintf(busted, "BUST");
					render_text(bustRect.x + 4, bustRect.y - 3, busted, _font, &red, _renderer);
				}
				// Renders "no money" if the player has no money
				if(cur_player->p_data.money < cur_player->p_data.bet) {
					bustRect.x = i*(separatorPos/4-5)+60;
					bustRect.w = 100;
					bustRect.h = 20;
					SDL_RenderFillRect(_renderer, &bustRect);
					sprintf(no_play, "NO MONEY");
					render_text(bustRect.x + 7, bustRect.y - 3, no_play, _font, &green, _renderer);
				}
			i += 1;
		}
	}
 }
