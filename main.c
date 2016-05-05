#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "general.h"
#include "game.h"
#include "gui.h"

int main(int argc, char **argv) {
	SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    // larger version of serif
    TTF_Font *large_serif = NULL;
    SDL_Surface *cards[MAX_DECK_SIZE+1], *imgs[2];
    SDL_Event event;
    int delay = 300;
    int quit = 0;
    
    playerlist players;
        
    int decknum; //Number of decks chosen by the player

	printf("Keys: \n(H)it - (S)tand - (Q)uit - (N)ew Game - (B)et - (D)ouble - Surrende(r)\n");
	
	// initialize graphics
	init_everything(WIDTH_WINDOW, HEIGHT_WINDOW, &serif, &large_serif, imgs, &window, &renderer);
    // loads the cards images
    load_cards(cards);

 	while (quit == 0) {
        // while there's events to handle
        while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				// The window gets closed
                quit = 1;
            }
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) { 
				    // You can quit with the 'q' key too
                    case SDLK_q:
                        // quit
                        break;
                    // Pass to the next player
					case SDLK_s:
						// stand
                        break;
                    // The player asks for another card
					case SDLK_h:
						// hit
                        break;
                    // You can't start a new game unless the current game is over.
                    case SDLK_n:
						//new game
                        break;
                    case SDLK_r:
						// surrender
						break;
					case SDLK_d:
						// double
						break;
					case SDLK_b:
						// bet
						break;
					default:
						break;
				}
			}
        }
        // The house plays

		/*
        // render game table
        render_table(money, serif, imgs, renderer, a_player);
        // render house cards
        render_house_cards(house_cards, pos_house_hand, cards, renderer, a_player);
        // render player cards
        render_player_cards(player_cards, pos_player_hand, cards, renderer);
        // render bust/game over rectangle
        render_bust(serif, large_serif, renderer, bust, money, bet, &game_over);
        // render in the screen all changes above
        SDL_RenderPresent(renderer);
    	// add a delay
		SDL_Delay(delay);
		*/
    }


    // free memory allocated for images and textures and close everything including fonts
    unload_cards(cards);
    TTF_CloseFont(serif);
    TTF_CloseFont(large_serif);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
