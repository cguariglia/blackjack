#include <stdio.h>
#include <stdlib.h>

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

// Pushes an element into the stack
// Returns pointer to the new top
c_node * push(stack *s, card data) {
	c_node *new_node;
	 
	new_node = (c_node *)allocate(sizeof(c_node)); 
	
	new_node->c_data = data;
	new_node->next = s->top;
	
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

// Inserts a new node given the desired index
void insert_node(p_node **head, player new, int index) {
	int i = 0;
	p_node *cur = *head;
	p_node *new_node;
	
	new_node = (p_node *)allocate(sizeof(p_node));
	new_node->p_data = new;
	
	// For the first position
	if(index == 0) {
		new_node->next = *head;
		*head = new_node;
		return; // Exit function
	}
	
	// All other positions
	while((i + 1) != index) {
		cur = cur->next;
		i++;
	}
	
	new_node->next = cur->next;
	cur->next = new_node;
}

// Frees memory of the player list
void free_list(p_node **head) {
	p_node *cur = NULL;
	
	// Starts on the second node, or the first node if there is only one node on the first place
	for(cur = (*head)->next; cur != *head; cur = cur->next)
		free(cur);
	
	free(*head);
	*head = NULL;
} 

void read_settings(char *filename, p_node **head, int *deck_num) {
	FILE *f;
	int player_num, i, args;
	char line[BUFFER], ai[2];
	player temp;
	
	f = fopen(filename, "r");
	
	fgets(line, BUFFER, f);
	sscanf(line, "%d-%d", deck_num, &player_num);
	if(args < 2 || deck_num < 4 || deck_num > 8 || player_num < 1 || player_num > 4) {
		printf("Invalid number of decks and/or players! Please try again.");
		exit(EXIT_FAILURE);
	}
	
	for(i = 0; i < player_num; i++) {
		fgets(line, BUFFER, f);
		args = sscanf(line, "%d-%s-%d-%d", &temp.type, temp.name, &temp.money, &temp.bet);
		if(args < 4) {
			printf("Invalid settings file! Check line %d. Please try again.", i + 2);
			exit(EXIT_FAILURE);
		}
}
