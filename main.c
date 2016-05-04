#include <stdio.h>
#include <stdlib.h>

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
    
    int money[MAX_PLAYERS] = {0};
	int player_cards[MAX_PLAYERS][MAX_CARD_HAND] = {{0}};
    int house_cards[MAX_CARD_HAND] = {0};
    int pos_house_hand = 0;
    int pos_player_hand[MAX_PLAYERS] = {0};
    
    int a_player = 0; //Current player
    int decknum; //Number of decks chosen by the player
    int deck_size; //Size of the array of cards
    
    int house_money = 0;
    int game_over = 0; //If this reaches 4, then no player can play
    
    // These variables are important for statistics/graphical functions
    int bet[MAX_PLAYERS] = {0}; //Bet for each of the players
    int wins[MAX_PLAYERS] = {0};
    int losses[MAX_PLAYERS] = {0};
    int ties[MAX_PLAYERS] = {0};
    int bust[MAX_PLAYERS] = {0};
    
    //Seeds based on an integer
    srand(456);

    //Asks for player settings
    SettingsReader(&decknum, money, bet);
    deck_size = decknum * MAX_DECK_SIZE;

    int deck[deck_size];

    //Makes the first deck
    MakeDeck(decknum, deck);
    shuffler(deck, deck_size);

    FirstHand(deck, player_cards, house_cards, pos_player_hand, &pos_house_hand, deck_size, money, bet);

	printf("Keys: \n(H)it - (S)tand - (Q)uit - (N)ew Game - (B)et - (D)ouble - Surrende(r)\n");
	
	// initialize graphics
	InitEverything(WIDTH_WINDOW, HEIGHT_WINDOW, &serif, &large_serif, imgs, &window, &renderer);
    // loads the cards images
    LoadCards(cards);

 	while (quit == 0) {
		while (money[a_player] - bet[a_player] < 0 && a_player < MAX_PLAYERS)
			a_player += 1;
        // while there's events to handle
        while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				// The window gets closed
                quit = 1;
                WritesStats(wins, losses, ties, money, house_money);
            }
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) { 
				    //You can quit with the 'q' key too
                    case SDLK_q:
                        //quit
                        break;
                    //Pass to the next player
					case SDLK_s:
						// stand
                        break;
                    //The player asks for another card
					case SDLK_h:
						// hit
                        break;
                    //You can't start a new game unless the current game is over.
                    case SDLK_n:
						//new game
                        break;
                    case SDLK_r:
						// surrender
						break;
					case SDLK _d:
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
        if (a_player == 4) {
			PlayerHouse(house_cards, &pos_house_hand, deck_size, deck, &a_player);
            house_money += MoneyCalculator(bet, player_cards, money, house_cards, wins, losses, ties, pos_player_hand, pos_house_hand);
        }
        // render game table
        RenderTable(money, serif, imgs, renderer, a_player);
        // render house cards
        RenderHouseCards(house_cards, pos_house_hand, cards, renderer, a_player);
        // render player cards
        RenderPlayerCards(player_cards, pos_player_hand, cards, renderer);
        // render bust/game over rectangle
        RenderBust(serif, large_serif, renderer, bust, money, bet, &game_over);
        // render in the screen all changes above
        SDL_RenderPresent(renderer);
    	// add a delay
		SDL_Delay(delay);
    }


    // free memory allocated for images and textures and close everything including fonts
    UnLoadCards(cards);
    TTF_CloseFont(serif);
    TTF_CloseFont(large_serif);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
