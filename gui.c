#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "gui.h"


// Renders the playing table(player squares, names, etc) and sidebar
void render_table(playerlist _players, ai_info ai, int seat, TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer *_renderer) {
    SDL_Color white = {255, 255, 255}; // White
    SDL_Color red =  {255, 0, 0}; // Red
    SDL_Color grey = {160, 160, 160}; // Grey
    char name_money_str[STRING_SIZE], add[STRING_SIZE];
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, playerRect;
    int separatorPos =(int)(0.95f*WIDTH_WINDOW); // Seperates the left from the right part of the window
    p_node *last, *cur = _players.head;
    int i = 0;

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
    
    last = NULL;
    // Renders everything, for every player. Last is used instead of cur because this loop needs to run one more time than that
    while(last != _players.tail) {
		// Either the player isn't playing or it's already been rendered
		if(cur->p_data.active == 0 || cur->p_data.seat < i) {
			last = cur;
			cur = cur->next;
			continue;
		}
		
		// Position and strings used for the rectangles
		playerRect.x = i*(separatorPos/4-5)+10;
        playerRect.y =(int)(0.55f*HEIGHT_WINDOW);
        playerRect.w = separatorPos/4-5;
        playerRect.h =(int)(0.42f*HEIGHT_WINDOW);
        sprintf(name_money_str, "%s -- %d euros -- %d pts", cur->p_data.name, cur->p_data.money, cur->p_data.points);
        sprintf(add, "No Player");
        
        // A seat hasn't been rendered, because there is no player in that seat
        if(cur->p_data.seat > (i + 1)) {
            SDL_SetRenderDrawColor(_renderer, 160, 160, 160, 255);
            render_text(playerRect.x + 20, playerRect.y - 30, add, _font, &grey, _renderer);
			SDL_RenderDrawRect(_renderer, &playerRect);

			// Fill player space
			if(seat == (i + 1)) {
				SDL_SetRenderDrawColor(_renderer, 160, 160, 160, 85);
				SDL_RenderFillRect(_renderer, &playerRect);
			}
            i += 1;
			continue;
		}
		if(cur == _players.tail)
			break;

        // Renders the active player in red
        else if(cur->p_data.status == 1) {
            SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
            render_text(playerRect.x + 20, playerRect.y - 30, name_money_str, _font, &red, _renderer);
        }
        // All other players are rendered in white
        else {
            SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
            render_text(playerRect.x + 20, playerRect.y - 30, name_money_str, _font, &white, _renderer);
        }
        SDL_RenderDrawRect(_renderer, &playerRect);
        
        last = cur;
        cur = cur->next;
        i += 1;
    }
    
    render_sidebar(_players, ai, _font, _img, _renderer);
    
    // Destroy everything
    SDL_DestroyTexture(table_texture);
}

void render_sidebar(playerlist players, ai_info ai, TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer *_renderer) {
    SDL_Color black = {0, 0, 0}; // Black
    SDL_Color red =  {255, 0, 0}; // Red
    SDL_Color grey = {160, 160, 160}; // Grey
    SDL_Color color;
    int i = 0, height = 0;
    int separatorPos =(int)(0.95f*WIDTH_WINDOW); // Seperates the left from the right part of the window
    char delay_str[STRING_SIZE], name_str[STRING_SIZE], money_str[STRING_SIZE];
    
    p_node *last, *cur = players.head;
    
    // Render the IST Logo, centered
    height = render_logo(separatorPos + 25, 0, _img[1], _renderer);

	last = NULL;
	while(last != players.tail) {
		if(cur->p_data.active == 0 || cur->p_data.seat < i) {
				last = cur;
				cur = cur->next;
				continue;
			}
	
		// Strings used to render the side info in case no player is playing in that spot
		sprintf(name_str, "No Player | --");
		sprintf(money_str, "Money: -- | Bet: --");
		
		
		if(cur->p_data.seat > (i + 1)) {
			height += render_text(separatorPos+3*MARGIN, height, name_str, _font, &grey, _renderer);
			height += render_text(separatorPos+3*MARGIN, height, money_str, _font, &grey, _renderer);
			height += 15;
			i += 1;
			continue;
		}
			
		if(cur == players.tail)
			break;
		
		// make strings
		sprintf(name_str, "%s | %s", cur->p_data.name, (cur->p_data.type == AI_TYPE ? "AI":"HU"));
		sprintf(money_str, "Money: %d | Bet: %d", cur->p_data.money, cur->p_data.bet);
		
		if(cur->p_data.status == 1) {
			// render in red
			SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
			color = red;
		}
		else if(cur->p_data.status == 0) {
			// render in black
			SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
			color = black;
		}
		
		height += render_text(separatorPos+3*MARGIN, height, name_str, _font, &color, _renderer);
		height += render_text(separatorPos+3*MARGIN, height, money_str, _font, &color, _renderer);
		height += 15;
		
		// go to next player
		last = cur;
		cur = cur->next;
		i += 1;
	}
	
    // Render AI delay
    height = 432;
    sprintf(delay_str, "AI Delay: %d second%c", ai.delay / 1000, ai.delay > 1000 ? 's' : ' ');
    height += render_text(separatorPos+3*MARGIN, height, delay_str, _font, &black, _renderer);

    // Render student names
    height = 471;
    height += render_text(separatorPos+3*MARGIN, height, "Carolina Guariglia | 83993", _font, &black, _renderer);
    height += render_text(separatorPos+3*MARGIN, height, "Miguel Paradinha | 84150", _font, &black, _renderer);
}

void render_house_cards(player _house, SDL_Surface **_cards, SDL_Renderer*_renderer) {
    int cards, x, y, i, hand_size;
    int div = WIDTH_WINDOW/CARD_WIDTH;
    c_node *cur;

	hand_size = _house.hand.size;

    // Drawing all house cards
    for(cards = 0; cards < hand_size; cards++) {
		cur = _house.hand.top;
		
		// We have to render the last card in the stack first, then the one before that and so forth
		for(i = 0; i < hand_size - cards - 1; i++)
			cur = cur->next;
		
        // Calculate its position
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
    int pos, x, y, cards;
    int num_player = 0; // Rendering player positions, 0 to 3
    p_node *cur_player;
    c_node *cur_card;
    int i, card_code, hand_size;

    // For every card of every player
    for(cur_player = players.head; cur_player->p_data.type != HOUSE_TYPE; cur_player = cur_player->next) {
		if(cur_player->p_data.active == 0 || cur_player->p_data.seat < num_player)
			continue;
		
		// Render in the correct player seating position
		while(cur_player->p_data.seat > (num_player + 1))
			num_player += 1;
			
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
            
            // Determines 
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
    // Unload all cards of the memory: +1 for the card back
    for(int i = 0 ; i < MAX_DECK_SIZE + 1; i++) {
        SDL_FreeSurface(_array_of_cards[i]);
    }
}

int render_logo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer*_renderer) {
	SDL_Texture *text_IST;
	SDL_Rect boardPos;

    // Space occupied by the logo
	boardPos.x = x;
	boardPos.y = y;
	boardPos.w = _logoIST->w;
	boardPos.h = _logoIST->h;

    // Renders it
	text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
	SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // Destroys associated texture
	SDL_DestroyTexture(text_IST);
	return _logoIST->h;
}

int render_text(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer*_renderer){
	SDL_Surface *text_surface;
	SDL_Texture *text_texture;
	SDL_Rect solidRect;

	solidRect.x = x;
	solidRect.y = y;
    // create a surface from the string text with a predefined font
	text_surface = TTF_RenderText_Blended(_font,text,*_color);
	if(!text_surface) {
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
		printf("Failed to initialize SDL : %s\n", SDL_GetError());
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

// create_window: Creates a window for the application
SDL_Window * create_window(int width, int height) {
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
SDL_Renderer * create_renderer(int width, int height, SDL_Window *_window) {
    SDL_Renderer *renderer;
    // init renderer
	renderer = SDL_CreateRenderer(_window, -1, 0);

	if(renderer == NULL) {
		printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
	}

	// set size of renderer to the same as window
	SDL_RenderSetLogicalSize(renderer, width+EXTRASPACE, height);
	
	// enable alpha for the renderer
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	return renderer;
}


 // Renders the word bust when the player has busted, 'blackjack' when the player has a blackjack
void render_overlay(TTF_Font *_font, SDL_Renderer *_renderer, playerlist players) {
    SDL_Rect overRect;
    SDL_Color red =  {255, 0, 0}; 
    SDL_Color yellow = {255, 255, 51};
    int separatorPos =(int)(0.95f*WIDTH_WINDOW);
    int i = 0;
    char busted[4];
    char blackjack[10];
    p_node *cur_player;
   
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
   
    for(cur_player = players.head; cur_player->p_data.type != players.tail->p_data.type; cur_player = cur_player->next) {	
		if(cur_player->p_data.active == 1) {
			while(cur_player->p_data.seat > (i + 1))
				i += 1;
			
			overRect.y =(int)(0.55f*HEIGHT_WINDOW) + 100;
			// Renders "bust" if the player has busted
				if(has_blackjack(cur_player->p_data) == 1) {
					overRect.x = i*(separatorPos/4-5)+60;
					overRect.w = 110;
					overRect.h = 20;
					SDL_RenderFillRect(_renderer, &overRect);
					sprintf(blackjack, "BLACKJACK");
					render_text(overRect.x + 7, overRect.y - 3, blackjack, _font, &yellow, _renderer);
				}
				if(cur_player->p_data.points > 21) {
					overRect.x = i*(separatorPos/4-5)+80;
					overRect.w = 50;
					overRect.h = 20;
					SDL_RenderFillRect(_renderer, &overRect);
					sprintf(busted, "BUST");
					render_text(overRect.x + 4, overRect.y - 3, busted, _font, &red, _renderer);
				}
			i += 1;
		}
	}
 }
