#ifndef GAME_H
#define GAME_H

#include "general.h"

#define BUFFER 256
#define BLACKJACK 21

void next_player(p_node **);
void make_ordered_deck(c_node **, int);
card remove_card(c_node **, int);
void make_deck(stack *, int);
void init_deck(stack **, int);
void deal_card(player *dest, stack *source) ;

#endif
