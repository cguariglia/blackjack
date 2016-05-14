#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "general.h"
#include "game.h"

// moves pointer to the next player in the table
void next_player(p_node **cur_player) {
	
	// change status of current player (before changing)
	if((*cur_player)->p_data.status == 1)
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
			cur->p_data.status = 0;
			for(i = 0; i < 2; i++)
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
		// Player has left the game
		if(cur_player->p_data.active == 0)
			continue;

		// Player has surrendered
		if(cur_player->p_data.status == SURRENDER_STATUS) {
			cur_player->p_data.money -= (int)(cur_player->p_data.bet / 2); 
			players->tail->p_data.money += (int)(cur_player->p_data.bet / 2);
			cur_player->p_data.losses += 1;
			cur_player->p_data.status = 0; // Gets player status back to normal
		}

		// Player has a blackjack (on the first hand) and the house doesn't
		else if(has_blackjack(cur_player->p_data) == 1 && players->tail->p_data.points != BLACKJACK) {
			cur_player->p_data.money += (int)(cur_player->p_data.bet * 1.5);
			players->tail->p_data.money -= (int)(cur_player->p_data.bet * 1.5);
			cur_player->p_data.wins += 1;
		}
		// Player has busted
		else if(cur_player->p_data.points > BLACKJACK) {
			cur_player->p_data.money -= cur_player->p_data.bet;
			players->tail->p_data.money += cur_player->p_data.bet;
			cur_player->p_data.losses += 1;
		}
		// The house has a blackjack
		else if(players->tail->p_data.points == BLACKJACK && cur_player->p_data.points != BLACKJACK) {
			cur_player->p_data.money -= cur_player->p_data.bet;
			players->tail->p_data.money += cur_player->p_data.bet;
			cur_player->p_data.losses += 1;
		}
		// The house doesn't have a blackjack
		else if(players->tail->p_data.points < BLACKJACK) {
			// But the player has more points
			if(cur_player->p_data.points > players->tail->p_data.points) {
				cur_player->p_data.money += cur_player->p_data.bet;
				players->tail->p_data.money -= cur_player->p_data.bet;
				cur_player->p_data.wins += 1;
			}
			// But the player has less points
			else {
				cur_player->p_data.money -= cur_player->p_data.bet;
				players->tail->p_data.money += cur_player->p_data.bet;
				cur_player->p_data.losses += 1;
			}
		}
		// Otherwise, there's a tie
		else
			cur_player->p_data.ties += 1;

		// reset player bet if they did a double
		if(cur_player->p_data.status == DOUBLE_STATUS) {
			cur_player->p_data.bet /= 2;
			cur_player->p_data.status = 0;
		}
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

void write_stats(playerlist players) {
	p_node *cur;
	FILE *stats;

    // open file
    stats = fopen("stats.txt", "w");

    // print format
    fprintf(stats, "Player Name | Player Type --> Wins | Losses | Ties | Money\n\n");

	for(cur = players.head; cur != players.tail; cur = cur->next)
		fprintf(stats, "%s | %s --> %d | %d | %d | %d\n", cur->p_data.name, cur->p_data.type == 1 ? "HU" : "AI", cur->p_data.wins, cur->p_data.losses, cur->p_data.ties, cur->p_data.money);

	fprintf(stats, "\nHouse Profit: %d", players.tail->p_data.money);

	// close file
	fclose(stats);
}

void change_bet(playerlist *players) {
	p_node *cur;
	char name[BUFFER], n_bet[BUFFER];
	int new_bet, args = 1;

	printf("Whose bet do you want to change?\n");
	fgets(name, BUFFER, stdin);

	name[strlen(name) - 1] = '\0';

	for(cur = players->head; cur != players->tail; cur = cur->next) {
		if(strcmp(name, cur->p_data.name) == 0) {
			if(cur->p_data.active == 0) {
				printf("That player has already left the table!\n");
				return;
			}
			else {
				do {
					if(args != 1)
						printf("That wasn't a valid number! Please try again.\n");
					printf("New bet: ");
					fgets(n_bet, BUFFER, stdin);
					args = sscanf(n_bet, "%d", &new_bet);
				} while(args != 1);

				cur->p_data.bet = new_bet;
				printf("%s's bet changed successfully to %d euros.\n", name, new_bet);

				return;
			}
		}
	}

	printf("That player is not playing this game! Please try again.");
}

// load the AI decision tables from a file
void load_ai_tables(ai_info *info) {
	int i, line = 0, col = 0;
	char c, **hard, **soft;
	FILE *f;

	// allocate memory for tables
	hard = (char **)allocate(sizeof(char *) * 10);
	soft = (char **)allocate(sizeof(char *) * 7);
	for(i = 0; i < 10; i++) {
		hard[i] = (char *)allocate(sizeof(char) * 10);
		soft[i] = (char *)allocate(sizeof(char) * 10);
	}

	f = fopen("ai_config.txt", "r");
	if(f == NULL) {
		printf("There was an error while opening the AI configuration file.\n");
		exit(EXIT_FAILURE);
	}

	// load tables
	c = fgetc(f);
	while(!feof(f)) {
		if(c == '\n') {
			line += 1;
			col = 0;
		}
		else {
			// load into the hard table
			if(line < 10)
				hard[line][col] = c;
			// load into soft table
			else if(line > 10 && line < 18)
				soft[line - 11][col] = c;

			col += 1;
		}

		c = fgetc(f);
	}

	// save tables to info struct
	info->hard_table = hard;
	info->soft_table = soft;

	fclose(f);
}

void print_table(char **table, int lines, int cols) {
	int i, j;

	for(i = 0; i < lines; i++) {
		for(j = 0; j < cols; j++) {
			printf("%c ", table[i][j]);
		}
		printf("\n");
	}
}

void play_ai(p_node **current, player house, stack *deck, int decks, ai_info info) {
	int aces = 0, line, col;
	char decision;
	card house_card;
	c_node *cur;
	player *ai = &((*current)->p_data);

	house_card = house.hand.top->next->c_data; // house's visible card

	// count aces
	for(cur = ai->hand.top; cur != NULL; cur = cur->next)
		if(cur->c_data.id == ACE_ID)
			aces += 1;

	// position in the table
	if(house_card.id == ACE_ID)
		col = 9;
	else if(house_card.id >= 9 && house_card.id <= 12)
		col = 8;
	else
		col = house_card.id - 1;

	// hard hand
	if(aces == 0) { 
		if(ai->points <= 8)
			line = 0;
		else if(ai->points >= 17)
			line = 9;
		else
			line = ai->points - 8;
		
		decision = info.hard_table[line][col];
	}
	// soft hand
	else {
		if(ai->points >= 19)
			line = 6;
		else
			line = ai->points - 13;

		decision = info.soft_table[line][col];
	}
	
	// make move
	if(decision == 'H') { // hit
		deal_card(ai, deck, decks);
	}
	else if(decision == 'S') { // stand
		next_player(current);
	}
	else if(decision == 'D') { // double
		ai->bet *= 2;
		ai->status = DOUBLE_STATUS;
		deal_card(ai, deck, decks);
		next_player(current);
	}
	else if(decision == 'R') { // surrender
		ai->status = SURRENDER_STATUS;
		next_player(current);
	}
}
