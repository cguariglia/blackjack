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

void deal_card(player *dest, stack *source, int deck_num) {
	c_node *temp;
	
	 // Check if deck is empty
	 if(is_empty(*source))
		init_deck(&source, deck_num);
	
	temp = pop(source);
	push(&(dest->hand), temp->c_data);
	
	// Calculates points after new card has been dealt
	dest->points = point_calculator(*dest); 
}

// Deal two cards to every player and the house
void first_hand(playerlist *players, stack *deck, int deck_num) {
	int i;
	p_node *cur;
	
	players->tail->p_data.active = 1;
	
	// For each player and the house
	for(cur = players->head; cur != NULL; cur = cur->next) {
		free_stack(&(cur->p_data.hand));
		
		if(cur->p_data.active == 1) {
			for(i = 0; i < 2; i++) // give two cards
				deal_card(&(cur->p_data), deck, deck_num);
		
			// If the house has recieved cards then stop dealing
			if(cur->next == players->head)
				break;
		}
	}
}

int point_calculator(player _player) {
	c_node *cur_card;
	int sum = 0;
	
	for(cur_card = _player.hand.top; cur_card != NULL; cur_card = cur_card->next) {
		if(cur_card->c_data.id <= 9)
			sum += (cur_card->c_data.id + 1);
		else if(cur_card->c_data.id >= 10 && cur_card->c_data.id <= 12)
			sum += 10;
	}
	
	for(cur_card = _player.hand.top; cur_card != NULL; cur_card = cur_card->next) {
		if(cur_card->c_data.id == 13) {
			sum += 11;
			if(sum > 21)
				sum -= 10;
		}
	}
		
	return sum;
}

void end_game(playerlist *players) { 
	p_node *cur_player;
	
	// Calculates money and wins/losses/ties for each player and the house
	for(cur_player = players->head; cur_player != players->tail; cur_player = cur_player->next) {
		if(cur_player->p_data.active == 0)
			continue;
		
		if(cur_player->p_data.points == BLACKJACK && cur_player->p_data.hand.size == 2 && players->tail->p_data.points != BLACKJACK) {
			cur_player->p_data.money += (int)(cur_player->p_data.bet * 1.5);
			players->tail->p_data.money -= (int)(cur_player->p_data.bet * 1.5);
			cur_player->p_data.wins += 1;
		}
		else if(players->tail->p_data.points > BLACKJACK) {
			cur_player->p_data.money -= cur_player->p_data.bet;
			players->tail->p_data.money += cur_player->p_data.bet;
			cur_player->p_data.wins += 1;
		}
		else if(players->tail->p_data.points == BLACKJACK && cur_player->p_data.points != BLACKJACK) {
			cur_player->p_data.money -= cur_player->p_data.bet;
			players->tail->p_data.money -= cur_player->p_data.bet;
			cur_player->p_data.losses += 1;
		}
		else if(players->tail->p_data.points <= BLACKJACK) {
			if(cur_player->p_data.points > players->tail->p_data.points) {
				cur_player->p_data.money += cur_player->p_data.bet;
				players->tail->p_data.money -= cur_player->p_data.bet;
				cur_player->p_data.wins += 1;
			}
			else {
				cur_player->p_data.money -= cur_player->p_data.bet;
				players->tail->p_data.money += cur_player->p_data.bet;
				cur_player->p_data.losses += 1;
			}
		}
		else
			cur_player->p_data.ties += 1;
	}
}

void house_plays(player *house, stack *deck, int deck_num) {
	c_node *cur;
	int aces = 0;
	
	while(house->points < 17)
		deal_card(house, deck, deck_num);
		
	if(house->points == 17) {
		for(cur = house->hand.top; cur != NULL; cur = cur->next)
			if(cur->c_data.id == 13) 
				aces += 1;
	
		if(aces == 1)
			deal_card(house, deck, deck_num);
	}
	
	house->status = 0;
	house->active = 2;
}

int has_blackjack(player _player) {
	if(_player.hand.size == 2)
		return (point_calculator(_player) == BLACKJACK);
	else
		return 0;
}
