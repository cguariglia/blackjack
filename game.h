#ifndef GAME_H
#define GAME_H

#include "general.h"

#define BUFFER 256

#define MAX_PLAYERS 4

#define BLACKJACK 21
#define ACE_ID 13

#define DOUBLE_STATUS 2
#define SURRENDER_STATUS 3

#define WIDTH_WINDOW 900      // window width
#define HEIGHT_WINDOW 525     // window height

void next_player(p_node **);
void make_ordered_deck(c_node **, int);
card remove_card(c_node **, int);
void make_deck(stack *, int);
void init_deck(stack **, int);
void deal_card(player *, stack *, int, ai_info *);
void first_hand(playerlist *, stack *, ai_info *, int);
int point_calculator(player);
void end_game(playerlist *);
void house_plays(player *, stack *, ai_info *, int);
int has_blackjack(player);
void write_stats(playerlist);
void change_bet(playerlist *);
void load_ai_tables(ai_info *, char*);
void print_table(char **, int, int);
void play_ai(p_node **, player, stack *, int, ai_info);
void update_ai_bet(playerlist *, ai_info, stack);
int get_seat(int, int);
int verify_seat(playerlist, int);
void add_player(playerlist *, int);

#endif
