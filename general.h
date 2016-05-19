#ifndef GENERAL_H
#define GENERAL_H

#include <stdlib.h>

#define BUFFER 256
#define MAX_NAME 9
#define HU_TYPE 0
#define AI_TYPE 1
#define HOUSE_TYPE 2

	
typedef struct {
	int id;
	int suit;	
} card;

// Defines node used to create player hands/house hand/deck
typedef struct nodetemp {
	card c_data;
	struct nodetemp *next;
} c_node;

// Defines the stack (used for everything card-related)
typedef struct {
	c_node *top;
	int size;
} stack;

typedef struct {
	char name[MAX_NAME];
	int type; // 0 - normal, 1 - AI, 2 - house
	stack hand;
	int money;
	int bet;
	int wins;
	int losses;
	int ties;
	int active; // 0 - player has left the table, 1 - player is playing
	int points;
	int status; // 0 - inactive, 1 - active, 2 - double, 3 - surrender
	int seat; // From 1 to 4, left to right
} player;

// Defines node used to create player list
typedef struct playertemp {
	player p_data;
	struct playertemp *next;
} p_node;

typedef struct {
	p_node *head;
	p_node *tail;
	int size;
} playerlist;

typedef struct {
	char **hard_table;
	char **soft_table;
	int delay;
	int count; // for card counting
} ai_info;

void * allocate(size_t);
c_node * push(stack *, card);
c_node * pop(stack *);
int is_empty(stack);
void free_stack(stack *);
void insert_node(playerlist *, player, int);
void free_list(playerlist *);
void read_settings(char *, playerlist *, int *);
void free_tables(ai_info);
void print_players(playerlist);
	
#endif
