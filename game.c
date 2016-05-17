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

void deal_card(player *dest, stack *source, int deck_num, ai_info *ai) {
	c_node *temp;

	// Check if deck is empty
	if(is_empty(*source))
		init_deck(&source, deck_num);
		
	// Update card counting for AI's
	if(source->top->c_data.id >= 9)
		ai->count -= 1;
	else if(source->top->c_data.id <= 5)
		ai->count += 1;
	printf("%d deal it\n", ai->count);

	temp = pop(source);
	push(&(dest->hand), temp->c_data);

	// Calculates points after new card has been dealt
	dest->points = point_calculator(*dest);
	
}

// Deal two cards to every player and the house
void first_hand(playerlist *players, stack *deck, ai_info *info, int deck_num) {
	int i;
	p_node *cur;

	players->tail->p_data.active = 1;

	// For each player and the house
	for(cur = players->head; cur != NULL; cur = cur->next) {
		free_stack(&(cur->p_data.hand));

		if(cur->p_data.active == 1) {
			cur->p_data.status = 0;
			
			for(i = 0; i < 2; i++) {
				if(i == 0 && cur == players->tail) {
					if(deck->top->c_data.id >= 9)
						info->count += 1;
					else if(deck->top->c_data.id <= 5)
						info->count -= 1;
				}	
				deal_card(&(cur->p_data), deck, deck_num, info);
				
			}

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
		
		if(cur_player->p_data.money < cur_player->p_data.bet)
			cur_player->p_data.active = 0;
	}
}

void house_plays(player *house, stack *deck, ai_info *ai_tables, int deck_num) {
	c_node *cur;
	int aces = 0;

	while(house->points < 17)
		deal_card(house, deck, deck_num, ai_tables);

	if(house->points == 17) {
		for(cur = house->hand.top; cur != NULL; cur = cur->next)
			if(cur->c_data.id == 13) 
				aces += 1;

		if(aces == 1)
			deal_card(house, deck, deck_num, ai_tables);
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
	int new_bet = 0, args = 1;

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
					if(args != 1 || cur->p_data.money < new_bet)
						printf("That wasn't a valid number! Please try again.\n");
					printf("New bet: ");
					fgets(n_bet, BUFFER, stdin);
					args = sscanf(n_bet, "%d", &new_bet);
				} while(args != 1 || cur->p_data.money < new_bet);

				cur->p_data.bet = new_bet;
				printf("%s's bet changed successfully to %d euros.\n", name, new_bet);

				return;
			}
		}
	}

	printf("That player is not playing this game! Press B to try again.\n");
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
		
		if(aces == 2)
			decision = 'H';
		else
			decision = info.soft_table[line][col];
	}
	
	// correct decision if it isn't possible to double
	if(decision == 'D' && ai->hand.size > 2) {
		if(aces == 0) // hard hand
			decision = 'H';
		else { // soft hand
			if(ai->points == 18)
				decision = 'S';
			else
				decision = 'H';
		}
	}
	
	// correct decision if surrendering is impossible
	if(decision == 'S' && ai->hand.size > 2)
		decision = 'H';
	
	// make move
	if(decision == 'H') { // hit
		deal_card(ai, deck, decks, &info);
	}
	else if(decision == 'S') { // stand
		next_player(current);
	}
	else if(decision == 'D') { // double
		ai->bet *= 2;
		ai->status = DOUBLE_STATUS;
		deal_card(ai, deck, decks, &info);
		next_player(current);
	}
	else if(decision == 'R') { // surrender
		ai->status = SURRENDER_STATUS;
		next_player(current);
	}
}

int get_seat(int x, int y) {
	
    int separatorPos =(int)(0.95f*WIDTH_WINDOW);
	int starting_y = (int)(0.55f*HEIGHT_WINDOW);
	int starting_x = 10;
	int rect_height = (int)(0.42f*HEIGHT_WINDOW);
	int rect_width = separatorPos/4-5;
	int seat;
	
	if(y >= starting_y && y <= starting_y + rect_height) {
		if(x >= starting_x && x <= ((rect_width * 4) + starting_x)) {
			seat = ((x - 10) / rect_width) + 1;
		}
		else 
			seat = -1;
	}
	else
		seat = -1;

	return seat;
}

int verify_seat(playerlist players, int seat) {
	p_node *cur;
	
	if(seat <= 0)
		return 0;
		
	for(cur = players.head; cur != players.tail; cur = cur->next) {
		if(cur->p_data.seat == seat && cur->p_data.active == 1)
			return 0;
	}
	
	return 1;	
}

void add_player(playerlist *players, int seat) {
	char line[BUFFER];
	player temp;
	p_node *cur;
	int args = 1, index;
	
	// initialize player variables
	temp.wins = 0; temp.losses = 0; temp.ties = 0;
	temp.points = 0; temp.active = 1; temp.status = 0;
	temp.type = 0; temp.seat = 0;
	
	// initialize player hand
	temp.hand = *((stack *)allocate(sizeof(stack)));
	temp.hand.top = NULL;
	temp.hand.size = 0;
	
	temp.money = 10;
	temp.bet = 2;
	temp.type = 1;
	
	do {
	printf("What is the new player's name? ");
	fgets(line, BUFFER, stdin);
	args = sscanf(line, "%s", temp.name); 
	} while(args != 1);
	
	// search if player name is taken
	for(cur = players->head; cur != players->tail; cur = cur->next) {
		if(strcmp(cur->p_data.name, temp.name) == 0) {
			printf("That name is taken. Please choose another name.\n");
			add_player(players, seat);
			return;
		}
	}
	
	do {
		if(temp.money < 10 || temp.money > 500 || args != 1)
			printf("Starting money must a whole number between 10 and 500!\n");
		printf("What is %s's starting money? ", temp.name);
		fgets(line, BUFFER, stdin);
		args = sscanf(line, "%d", &temp.money);
	} while(temp.money < 10 || temp.money > 500 || args != 1);
	
	// Check for errors regarding the bet
	do {
		if(temp.bet < 2 || temp.bet > (temp.money / 4) || args != 1)
			printf("Betting money invalid. Must be a whole number between 2 and 25%% of the starting money.\n");
		printf("What is %s's starting bet? ", temp.name);
		fgets(line, BUFFER, stdin);
		args = sscanf(line, "%d", &temp.bet);
	} while(temp.bet < 2 || temp.bet > (temp.money / 4) || args != 1);
		
	// Check for errors on the type of the player
	do {
		if(temp.type != 0 && temp.type != 1)
			printf("That's not a valid player type!\n");
		printf("What is the player's type? (HU or EA) ");
		fgets(line, BUFFER, stdin);
		line[strlen(line) - 1] = '\0';
		if(strcmp("HU", line) == 0)
			temp.type = 0;
		else if(strcmp("EA", line) == 0)
			temp.type = 1;
		else
			temp.type = 3;
	} while(temp.type != 0 && temp.type != 1);
	
	temp.seat = seat;
	
	// find correct index to insert
	index = 0;
	cur = players->head;
	while(cur->p_data.seat < seat) {
		index += 1;
		cur = cur->next;
	}
	
	insert_node(players, temp, index);
}
