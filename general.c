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
c_node * push(c_node *top, card data) {
	c_node *new_node;
	 
	new_node = (c_node *)allocate(sizeof(c_node)); 
	
	new_node->c = data;
	new_node->next = top;
	
	return new_node;
}

// Removes an element from the stack
// Returns pointer to removed node
c_node * pop(c_node **top) {
	c_node *temp = *top;
	
	*top = (*top)->next;
	
	return temp;
}

// Checks if the stack is empty
int is_empty(c_node *stack) {
	return stack == NULL;
}

// Removes all elements from a stack
void clear_stack(c_node **top) {
	while(is_empty(*top) == 0)
		pop(top);
}

// Inserts a new node given the desired index
void insert_node(p_node **head, p_node *new, int index) {
	int i = 0;
	p_node *cur = *head;
	
	// For the first position
	if(index == 0) {
		new->next = *head;
		head = new;
		return; // Exit function
	}
	
	// All other positions
	while((i + 1) != index) {
		cur = cur->next;
		i++;
	}
	
	new->next = cur->next;
	cur->next = new;
}
