#include <stdlib.h>
#include <stdio.h>

#include "general.h"
#include "game.h"

// moves pointer to the next player in the table
void next_player(p_node **cur_player) {
	
	// change status of current player (before changing)
	(*cur_player)->p_data.status = 0;
    	 
	do {
		*cur_player = (*cur_player)->next;
	 } while((*cur_player)->p_data.active != 1);
		
	// change status of the (now) current player
	(*cur_player)->p_data.status = 1;
}

void make_ordered_deck(c_node **head, int decks) {
	int i;
	c_node *new_node;

	for(i = 0; i < decks * 52; i++) {
		new_node = (c_node *)allocate(sizeof(c_node));
		
		new_node->c_data.id = (i % 13) + 1;
		new_node->c_data.suit = (i / 13) % 4;
		
		new_node->next = *head;
		*head = new_node;
	}
}

card remove_card(c_node **head, int index) {
	c_node *cur;
	c_node *prev = NULL;
	
	int i = 0;
	
	for(cur = *head; cur != NULL; prev = cur, cur = cur->next) {
		if(i == index) {
			if(prev == NULL)
				*head = cur->next;
			else if(cur->next == NULL)
				prev->next = NULL;
			else
				prev->next = cur->next;
			
			break;
		}
		i += 1;			
	}
	
	return (cur->c_data);
}

void make_deck(stack *deck, int deck_num) {
	c_node *card_list = NULL;
	card removed;
	int size; // number of card left to remove
	
	// create ordered deck as a linked list
	make_ordered_deck(&card_list, deck_num);
	size = 52 * deck_num;
	
	while(card_list != NULL) {
		removed = remove_card(&card_list, rand() % size);
		size -= 1;
		
		push(deck, removed);
	}
}

void init_deck(stack **deck, int deck_num) {
	*deck = (stack *)allocate(sizeof(stack));
	(*deck)->top = NULL; 
	(*deck)->size = 0;
	make_deck(*deck, deck_num);
}

void deal_card(player *dest, stack *source) {
	c_node *temp;
	
	temp = pop(source);
	push(&(dest->hand), temp->c_data);
}
