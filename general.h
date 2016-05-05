#ifndef GENERAL_H
#define GENERAL_H

#define BUFFER 256
#define MAX_NAME 8
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
	char *name[MAX_NAME];
	int type; // 0 - normal, 1 - AI, 2 - house
	stack hand;
	int money;
	int bet;
	int wins;
	int losses;
	int ties;
	int active; // 0 - player has left the table, 1 - player is playing
	int points;
	int status; // 0 - inactive, 1 - active, 2 - blackjack, 3 - bust
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

void * allocate(size_t);
c_node * push(stack *, card);
c_node * pop(stack *);
int is_empty(stack);
void free_stack(stack *);
void insert_node(playerlist *, player, int);
void free_list(playerlist *;
void swap(int [], int, int);
void read_settings(char *, playerlist *, int *);
	
#endif
