#ifndef GAME_H
#define GAME_H

#include "general.h"

#define BUFFER 256

typedef struct card {
	int id;
	int suit;
};
	
typedef struct player {
	char *name[BUFFER];
	int type; // 0 - normal, 1 - AI
	c_node *hand;
	int money;
	int bet;
	int wins;
	int losses;
	int ties;
	int active;
	int points;
	int status; // 0 - blackjack, 1 - bust, 2 - stand, 3 - hit, 4 - deal
};

#endif
