#ifndef GENERAL_H
#define GENERAL_H

#include "game.h"

// Defines node used to create player hands/house hand/deck
typedef struct nodetemp {
	card *c;
	struct nodetemp *next;
} c_node;

// Defines node used to create player list
typedef struct playertemp {
	player *p;
	struct playertemp *next;
} p_node;

void * allocate(size_t);
c_node * push(c_node *, card);
c_node * pop(c_node **);
int is_empty(c_node *);
void clear_stack(c_node **);
void insert_node(p_node *, int);
	
#endif
