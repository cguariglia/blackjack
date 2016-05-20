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
    int mouse_x, mouse_y;
    playerlist *players;
	ai_info ai_tables; // tables for AI's and count for card counting
    p_node *current;
    stack *deck;
    int deck_num; //Number of decks chosen by the player
    int seat;
    int a_pressed = 0;
    
    srand(time(NULL)); // seeds
    
    // check arguments
    if(argc != 3) {
		printf("Not enough arguments (or too many)! Please specify the filename only.\n");
		return EXIT_FAILURE;
	}
	
	players = (playerlist *)allocate(sizeof(playerlist));
    read_settings(argv[1], players, &deck_num);

	init_deck(&deck, deck_num);
	
	printf("Keys:\n(H)it - (S)tand - (B)et - (D)ouble - Surrende(r)\n(Q)uit - (N)ew Game - (A)dd Player\n");
	
	// Load AI decision info
	load_ai_tables(&ai_tables, argv[2]);
	ai_tables.delay = 1000;
	ai_tables.count = 0;
	
	// Initialize graphics
	init_everything(WIDTH_WINDOW, HEIGHT_WINDOW, &serif, imgs, &window, &renderer);
    // loads the cards images
    load_cards(cards);
    
    first_hand(players, deck, &ai_tables, deck_num); // deal first round of cards
    
	// Set first player
	current = players->head; 
	current->p_data.status = 1;
    
 	while (quit == 0) {
		
        // While there's events to handle
        while(SDL_PollEvent(&event)) {

			SDL_GetMouseState(&mouse_x, &mouse_y);

			if (event.type == SDL_QUIT) {
				// The window gets closed
				quit = 1;
            }
			else if(event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) { 
				    // You can quit with the 'q' key too
                    case SDLK_q:
                        // Quit and write stats file
                        if(current == players->tail) {
							quit = 1;
							write_stats(*players);
						}
                        break;
                        
                    // Pass to the next player
					case SDLK_s:
						// Stand -- only go to next player if it's not the house playing
						if(current != players->tail && current->p_data.type == HU_TYPE)
							next_player(&current);
                        
                        break;
                        
                    // The player asks for another card
					case SDLK_h:
						// Hit
						if(current != players->tail && current->p_data.type == HU_TYPE)
						    deal_card(&(current->p_data), deck, deck_num, &ai_tables);    
                        break;
                        
                    // You can't start a new game unless the current game is over.
                    case SDLK_n:
						// The game is only over if the house has finished playing
						if(current == players->tail) {
							first_hand(players, deck, &ai_tables, deck_num);
							next_player(&current);
						}
                        break;
                        
                    case SDLK_r:
						// Surrender
						if(current != players->tail && current->p_data.type == HU_TYPE && current->p_data.hand.size == 2) {
							current->p_data.status = SURRENDER_STATUS;
							next_player(&current);
						}
						break;
						
					case SDLK_d:
						// Double
						if(current != players->tail && current->p_data.type == HU_TYPE && current->p_data.hand.size == 2) {
							current->p_data.bet *= 2;
							current->p_data.status = DOUBLE_STATUS;
							deal_card(&(current->p_data), deck, deck_num, &ai_tables);
							next_player(&current);
						}
						break;
						
					case SDLK_b:
						// Bet
						if(current == players->tail)
							change_bet(players);
						break;
					case SDLK_a:
						if(current == players->tail) {
							printf("Press on an empty seat to add a player.\n");
							seat = -1;
							a_pressed = 1;
						}
						break;
					case SDLK_UP:
						ai_tables.delay += 1000;
						break;
					case SDLK_DOWN:
						if(ai_tables.delay > 1000)
							ai_tables.delay -= 1000;
						break;
					default:
						break;
				} // close event switch
			}
			else if(event.type == SDL_MOUSEBUTTONUP) {
				if(event.button.button == SDL_BUTTON_LEFT) {
					seat = get_seat(mouse_x, mouse_y);
				}
			}
        } // close event loop

        // check if player bust
        if(current->p_data.points > 21 && current != players->tail)
			next_player(&current);
		// check if player has blackjack
        if(has_blackjack(current->p_data) && current != players->tail)
			next_player(&current);
        
        // time for da houzz
        if(current == players->tail && players->tail->p_data.status == 1) {
			house_plays(&(players->tail->p_data), deck, &ai_tables, deck_num);
			end_game(players);
			update_ai_bet(players, ai_tables, *deck);
		}
		
		if(a_pressed == 1) {
			if(verify_seat(*players, seat) == 1) {
				add_player(players, seat);
					a_pressed = 0;
			}
		}
        
        // render game table
        render_table(*players, ai_tables, get_seat(mouse_x, mouse_y), serif, imgs, renderer);
        // render house cards
        render_house_cards(players->tail->p_data, cards, renderer);
        // render player cards
        render_player_cards(*players, cards, renderer);
        // render bust/game over rectangle
        render_overlay(serif, renderer, *players);
        // render in the screen all changes above
        SDL_RenderPresent(renderer);
    	// add a delay
		SDL_Delay(delay);
		
		// them AI's are getting smart
		if(current->p_data.type == AI_TYPE) {
			SDL_Delay(ai_tables.delay);
			play_ai(&current, players->tail->p_data, deck, deck_num, ai_tables);
		}
    }

    // Free memory allocated for images, textures and lists and close everything including fonts
    free_list(players); 
    //free_tables(ai_tables);
    
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
