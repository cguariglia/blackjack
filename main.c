#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "general.h"
#include "game.h"
#include "gui.h"

int main(int argc, char **argv) {

    // SDL variables
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    TTF_Font *large_serif = NULL; // larger version of serif
    SDL_Surface *cards[MAX_DECK_SIZE+1], *imgs[2];
    SDL_Event event;
    int delay = 33;
    int quit = 0;
    playerlist *players;
    p_node *current;
    stack *deck;
    int deck_num; //Number of decks chosen by the player
    
    srand(time(NULL)); // seeds
    
    // check arguments
    if(argc != 2) {
		printf("Not enough arguments (or too many)! Please specify the filename only.\n");
		return EXIT_FAILURE;
	}
	
    players = (playerlist *)allocate(sizeof(playerlist));
    read_settings(argv[1], players, &deck_num);

	init_deck(&deck, deck_num);
	
	// set first player
	current = players->head; 
	current->p_data.status = 1;
	
	printf("Keys:\n(H)it - (S)tand - (Q)uit - (N)ew Game - (B)et - (D)ouble - Surrende(r)\n");
	
	// initialize graphics
	init_everything(WIDTH_WINDOW, HEIGHT_WINDOW, &serif, imgs, &window, &renderer);
    // loads the cards images
    load_cards(cards);
    
    first_hand(players, deck); // deal first round of cards
    
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
                        quit = 1;
                        break;
                        
                    // Pass to the next player
					case SDLK_s:
						// stand
						
						// only go to next player if its not the house playing
						if(current != players->tail)
							next_player(&current);
                        
                        break;
                        
                    // The player asks for another card
					case SDLK_h:
						// hit
						if(current != players->tail)
						    deal_card(&(current->p_data), deck);
						    
                        break;
                        
                    // You can't start a new game unless the current game is over.
                    case SDLK_n:
						//new game
						
						// only start new game if house is finished playing
						if(current == players->tail)
							next_player(&current);
							
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
				} // close event switch
				if(current->p_data.points > BLACKJACK)
					next_player(&current);
			} 
        } // close event loop
        
        // render game table
        render_table(*players, serif, imgs, renderer);
        // render house cards
        render_house_cards(players->tail->p_data, cards, renderer);
        // render player cards
        render_player_cards(*players, cards, renderer);
        // render bust/game over rectangle
        //render_bust(serif, large_serif, renderer, bust, money, bet, &game_over);
        // render in the screen all changes above
        SDL_RenderPresent(renderer);
    	// add a delay
		SDL_Delay(delay);
    }


    // free memory allocated for images and textures and close everything including fonts
    free_list(players);   
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
