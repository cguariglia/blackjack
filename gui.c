/**
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  squares to define the playing positions of each player
 * -  names and the available money for each player
 * \param _money amount of money of each player
 * \param _img surfaces where the table background and IST logo were loaded
 * \param _renderer renderer to handle all rendering in a window
 */

void RenderTable(int _money[], TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer) {
    SDL_Color black = {0, 0, 0}; // Black
    SDL_Color white = {255, 255, 255}; // White
    SDL_Color red =  {255, 0, 0}; // Red
    char name_money_str[STRING_SIZE];
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, playerRect;
    int separatorPos = (int)(0.95f*WIDTH_WINDOW); // Seperates the left from the right part of the window
    int height;

    // set color of renderer to some color
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

    // clear the window
    SDL_RenderClear(_renderer);

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;

    tableDest.w = separatorPos;
    tableDest.h = HEIGHT_WINDOW;

    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(separatorPos, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(separatorPos+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    RenderText(separatorPos+3*MARGIN, height, myNumber, _font, &black, _renderer);

    // renders the squares + name for each player
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

    // renders the areas for each player: names and money too !
    for (int i = 0; i < MAX_PLAYERS; i++) {
		playerRect.x = i*(separatorPos/4-5)+10;
        playerRect.y = (int) (0.55f*HEIGHT_WINDOW);
        playerRect.w = separatorPos/4-5;
        playerRect.h = (int) (0.42f*HEIGHT_WINDOW);
        sprintf(name_money_str,"%s -- %d euros", playerNames[i], _money[i]);
        //Renders the active player in red
        if (i == active_player) {
            SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
            RenderText(playerRect.x+20, playerRect.y-30, name_money_str, _font, &red, _renderer);
        }
        //All other players are rendered in white
        else {
            SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
            RenderText(playerRect.x+20, playerRect.y-30, name_money_str, _font, &white, _renderer);
        }
        SDL_RenderDrawRect(_renderer, &playerRect);
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
}

/**
 * RenderHouseCards: Renders cards of the house
 * \param _house vector with the house cards
 * \param _pos_house_hand position of the vector _house with valid card IDs
 * \param _cards vector with all loaded card images
 * \param _renderer renderer to handle all rendering in a window
 */

void RenderHouseCards(int _house[], int _pos_house_hand, SDL_Surface **_cards, SDL_Renderer* _renderer, int active_player)
{
    int card, x, y;
    int div = WIDTH_WINDOW/CARD_WIDTH;

    // drawing all house cards
    for (card = 0; card < _pos_house_hand; card++) {
        // calculate its position
        x = (div/2-_pos_house_hand/2+card)*CARD_WIDTH + 15;
        y = (int) (0.26f*HEIGHT_WINDOW);
        // Makes the card render upside-down or right-side up
        if (_pos_house_hand == 2 && card == 1 && active_player != 5)
            RenderCard(x, y, MAX_DECK_SIZE, _cards, _renderer);
        else
            RenderCard(x, y, _house[card], _cards, _renderer);
    }

}

/**
 * RenderPlayerCards: Renders the hand, i.e. the cards, for each player
 * \param _player_cards 2D array with the player cards, 1st dimension is the player ID
 * \param _pos_player_hand array with the positions of the valid card IDs for each player
 * \param _cards vector with all loaded card images
 * \param _renderer renderer to handle all rendering in a window
 */

void RenderPlayerCards(int _player_cards[][MAX_CARD_HAND], int _pos_player_hand[], SDL_Surface **_cards, SDL_Renderer* _renderer)
{
    int pos, x, y, num_player, card;

    // for every card of every player
    for (num_player = 0; num_player < MAX_PLAYERS; num_player++) {
        for (card = 0; card < _pos_player_hand[num_player]; card++) {
            // draw all cards of the player: calculate its position: only 4 positions are available !
            pos = card % 4;
            x = (int) num_player*((0.95f*WIDTH_WINDOW)/4-5)+(card/4)*12+15;
            y = (int) (0.55f*HEIGHT_WINDOW)+10;
            if (pos == 1 || pos == 3) x += CARD_WIDTH + 30;
            if (pos == 2 || pos == 3) y += CARD_HEIGHT+ 10;
            // render it !
            RenderCard(x, y, _player_cards[num_player][card], _cards, _renderer);
        }
    }
}

/**
 * RenderCard: Draws one card at a certain position of the window, based on the card code
 * \param _x X coordinate of the card position in the window
 * \param _y Y coordinate of the card position in the window
 * \param _num_card card code that identifies each card
 * \param _cards vector with all loaded card images
 * \param _renderer renderer to handle all rendering in a window
 */

void RenderCard(int _x, int _y, int _num_card, SDL_Surface **_cards, SDL_Renderer* _renderer)
{
    SDL_Texture *card_text;
    SDL_Rect boardPos;

    // area that will be occupied by each card
    boardPos.x = _x;
    boardPos.y = _y;
    boardPos.w = CARD_WIDTH;
    boardPos.h = CARD_HEIGHT;

    // render it !
    card_text = SDL_CreateTextureFromSurface(_renderer, _cards[_num_card]);
    SDL_RenderCopy(_renderer, card_text, NULL, &boardPos);

    // destroy everything
    SDL_DestroyTexture(card_text);
}

/**
 * LoadCards: Loads all images of the cards
 * \param _cards vector with all loaded card images
 */

void LoadCards(SDL_Surface **_cards)
{
    int i;
    char filename[STRING_SIZE];

     // loads all cards to an array
    for (i = 0 ; i < MAX_DECK_SIZE; i++) {
        // create the filename !
        sprintf(filename, ".//cartas//carta_%02d.png", i+1);
        // loads the image !
        _cards[i] = IMG_Load(filename);
        // check for errors: deleted files ?
        if (_cards[i] == NULL) {
            printf("Unable to load image: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
    // loads the card back
    _cards[i] = IMG_Load(".//cartas//carta_back.jpg");
    if (_cards[i] == NULL) {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * UnLoadCards: unloads all card images of the memory
 * \param _cards vector with all loaded card images
 */

void UnLoadCards(SDL_Surface **_array_of_cards)
{
    // unload all cards of the memory: +1 for the card back
    for (int i = 0 ; i < MAX_DECK_SIZE + 1; i++)
    {
        SDL_FreeSurface(_array_of_cards[i]);
    }
}

/**
 * RenderLogo function: Renders the IST Logo on the window screen
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */

int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
	SDL_Texture *text_IST;
	SDL_Rect boardPos;

    // space occupied by the logo
	boardPos.x = x;
	boardPos.y = y;
	boardPos.w = _logoIST->w;
	boardPos.h = _logoIST->h;

    // render it
	text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
	SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
	SDL_DestroyTexture(text_IST);
	return _logoIST->h;
}

/**
 * RenderText function: Renders the IST Logo on the window screen
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string where the text is written
 * \param font TTF font used to render the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
	SDL_Surface *text_surface;
	SDL_Texture *text_texture;
	SDL_Rect solidRect;

	solidRect.x = x;
	solidRect.y = y;
    // create a surface from the string text with a predefined font
	text_surface = TTF_RenderText_Blended(_font,text,*_color);
	if (!text_surface)
	{
	    printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
	    exit(EXIT_FAILURE);
	}
    // create texture
	text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
	SDL_QueryTexture(text_texture, NULL, NULL, &solidRect.w, &solidRect.h);
    // render it !
	SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);

	SDL_DestroyTexture(text_texture);
	SDL_FreeSurface(text_surface);
	return solidRect.h;
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height, TTF_Font **_font, TTF_Font **_large_font, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer) {
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL) {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL) {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    // This is a larger version of the above font
    *_large_font = TTF_OpenFont("FreeSerif.ttf", 80);
    if (!*_font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL()
	{
    // init SDL library
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
	}
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont()
	{
	// Init font library
	if (TTF_Init()==-1) {
	    printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
	}
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window *window;
    // init window
	window = SDL_CreateWindow("BlackJack", WINDOW_POSX, WINDOW_POSY, width+EXTRASPACE, height, 0);
    // check for error !
	if (window == NULL) {
		printf("Failed to create window : %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
	renderer = SDL_CreateRenderer(_window, -1, 0);

	if (renderer == NULL)
	{
		printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
	}

	// set size of renderer to the same as window
	SDL_RenderSetLogicalSize(renderer, width+EXTRASPACE, height);

	return renderer;
}

/**
 * Renders the word bust when the player has busted, 'no money' when the player has run out of money and 'game over' when that happens to all players.
 * \param bust if a player's index == 1, then that player has buste
 * \param money a player's current money
 * \param bet corresponding to each player 
 * \param game_over if this variable >= 4 then no players have money remaining
**/
void RenderBust(TTF_Font *_font, TTF_Font *_large_font, SDL_Renderer* _renderer, int bust[], int money[], int bet[], int *game_over)
{
    SDL_Rect bustRect, overRect;
    SDL_Color red =  {255, 0, 0}; 
    SDL_Color yellow = {255, 255, 51};
    int separatorPos = (int)(0.95f*WIDTH_WINDOW);
    int i;
    char busted[4];
    char no_play[9];
    char over[9];
    *game_over = 1;
   
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
   
    for (i = 0; i < MAX_PLAYERS; i++) {	
		bustRect.y = (int) (0.55f*HEIGHT_WINDOW) + 100;
		// Renders "bust" if the player has busted
		if (bust[i] == 1) {
			bustRect.x = i*(separatorPos/4-5)+80;
			bustRect.w = 50;
			bustRect.h = 20;
			SDL_RenderFillRect(_renderer, &bustRect);
			sprintf(busted, "BUST");
			RenderText(bustRect.x + 4, bustRect.y - 3, busted, _font, &red, _renderer);
		}
		// Renders "no money" if the player has no money
		if (money[i] < bet[i]) {
			bustRect.x = i*(separatorPos/4-5)+60;
			bustRect.w = 100;
			bustRect.h = 20;
			SDL_RenderFillRect(_renderer, &bustRect);
			sprintf(no_play, "NO MONEY");
			RenderText(bustRect.x + 7, bustRect.y - 3, no_play, _font, &yellow, _renderer);
		}
		else
			*game_over = 0;
	}
	
	// Draws game over when no players can play
	if (*game_over == 1) {
		overRect.x = 180;
		overRect.y = 140;
		overRect.h = 90;
		overRect.w = 500;
		SDL_RenderFillRect(_renderer, &overRect);
		sprintf(over, "GAME OVER");
		RenderText(overRect.x + 12, overRect.y - 20, over, _large_font, &red, _renderer);
	}
 }
