#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "general.h"

// Wrapper function around malloc, to check for memory allocation failure
void * allocate(size_t size) {
	void *ptr;
	
	ptr = malloc(size);
	
	if(ptr == NULL) {
		printf("Error allocating memory!");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

// Functions used for stacks start here

// Pushes an element into the stack
// Returns pointer to the new top
c_node * push(stack *s, card data) {
	c_node *new_node;
	 
	new_node = (c_node *)allocate(sizeof(c_node)); 
	
	new_node->c_data = data;
	new_node->next = s->top;
	s->top = new_node;
	
	s->size += 1;
	
	return new_node;
}

// Removes an element from the stack
// Returns pointer to removed node
c_node * pop(stack *s) {
	c_node *temp = s->top;
	
	s->top = s->top->next;
	
	s->size -= 1;
	
	return temp;
}

// Checks if the stack is empty
int is_empty(stack s) {
	return (s.top == NULL);
}

// Removes all elements from a stack
void free_stack(stack *s) {
	while(is_empty(*s) == 0)
		free(pop(s));
}

// End of stack-related functions, start of list-related functions

// Inserts a new node given the desired index
void insert_node(playerlist *players, player new, int index) {
	int i = 0;
	p_node *cur = players->head;
	p_node *new_node;
	
	new_node = (p_node *)allocate(sizeof(p_node));
	new_node->p_data = new;
	
	// initialize tail for first element
	if(players->size == 0)
		players->tail = new_node;
	
	// For the first position
	if(index == 0) {
		new_node->next = players->head;
		players->head = new_node;
		players->tail->next = new_node; // update tail
	}
	else {
		// All other positions
		while((i + 1) != index) {
			cur = cur->next;
			i += 1;
		}
		
		new_node->next = cur->next;
		cur->next = new_node;
	}
	
	// Change tail if element was inserted at the end of the list
	if(index == players->size)
		players->tail = new_node;
	
	players->size += 1;
}

// Frees memory of the player list
void free_list(playerlist *players) {
	p_node *cur = NULL;
	
	// Starts on the second node, or the first node if there is only one node on the first place
	for(cur = players->head->next; cur != players->head; cur = cur->next)
		free(cur);
	
	free(players->head);
	players->head = NULL;
} 

// Other general functions

void read_settings(char *filename, playerlist *players, int *deck_num) {
	FILE *f;
	int player_num, i, args;
	char line[BUFFER];
	char temptype[2];
	player temp;
	
	// initialize player variables
	temp.wins = 0; temp.losses = 0; temp.ties = 0;
	temp.points = 0; temp.active = 1; temp.status = 0;
	temp.type = 0; temp.seat = 0;
	
	// initialize player hand
	temp.hand = *((stack *)allocate(sizeof(stack)));
	temp.hand.top = NULL;
	temp.hand.size = 0;
	
	f = fopen(filename, "r");
	
	fgets(line, BUFFER, f);
	args = sscanf(line, "%d-%d", deck_num, &player_num);
	if(args < 2 || *deck_num < 4 || *deck_num > 8 || player_num < 1 || player_num > 4) {
		printf("Invalid number of decks and/or players! Please try again.");
		exit(EXIT_FAILURE);
	}
	
	players->size = 0;
	
	for(i = 0; i < player_num; i++) {
		fgets(line, BUFFER, f);
		args = sscanf(line, "%[^-]-%[^-]-%d-%d", temptype, temp.name, &temp.money, &temp.bet);
		if(args < 4) {
			printf("Invalid settings file! Check line %d. Please try again.", i + 2);
			exit(EXIT_FAILURE);
		}
		
		temp.name[strlen(temp.name)] = '\0';
		
		// Check for errors regarding the start money
		if(temp.money < 10 || temp.money > 500) {
			printf("Starting money must a whole number between 10 and 500! Check line %d.", i + 2);
			exit(EXIT_FAILURE);
		}
		
		// Check for errors regarding the bet
		if(temp.bet < 2 || temp.bet > (temp.money / 4)) {
			printf("Betting money invalid. Must be between 2 and 25%% of the starting money. Check line %d.", i + 2);
			exit(EXIT_FAILURE);
		}
		
		// Check for errors on the type of the player
		if(strcmp("HU", temptype) == 0)
			temp.type = 0;
		else if(strcmp("EA", temptype) == 0)
			temp.type = 1;
		else {
			printf("Invalid player type in line %d! Please try again.", i + 2);
			exit(EXIT_FAILURE);
		}
		
		temp.seat = i + 1;
		
		insert_node(players, temp, players->size);
		printf("inserted: %s\n", temp.name);
	}
	
	fclose(f);
	
	// Initializes house
	temp.type = 2;
	temp.status = 0;
	temp.seat = 5;
	
	insert_node(players, temp, players->size);
}

void print_players(playerlist players) {
	p_node *p;
	
	for(p = players.head; p != players.tail; p = p->next) {
		printf("%s\n", p->p_data.name);
	}
}
