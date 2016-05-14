#ifndef GAME_H
#define GAME_H

#include "general.h"

#define BUFFER 256
#define BLACKJACK 21
#define ACE_ID 13

#define DOUBLE_STATUS 2
#define SURRENDER_STATUS 3

void next_player(p_node **);
void make_ordered_deck(c_node **, int);
card remove_card(c_node **, int);
void make_deck(stack *, int);
void init_deck(stack **, int);
void deal_card(player *, stack *, int);
void first_hand(playerlist *, stack *, int);
int point_calculator(player);
void end_game(playerlist *);
void house_plays(player *, stack *, int);
int has_blackjack(player);
void write_stats(playerlist);
void change_bet(playerlist *);
void load_ai_tables(char ***, char ***);
void print_table(char **, int, int);
void play_ai(player *, player, p_node **, stack *, int, char **, char **);

#endif
