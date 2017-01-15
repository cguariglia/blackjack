# Blackjack

## Description
This is the final project for our first college programming class.
It was made using C and SDL2.

The last commit before we did before handing in the work was [this one](https://github.com/cguariglia/blackjack/commit/0b1e5871747dac73e59b96d3eabd282e5a3a355c) although we might have changed some minor stuff between that and actually handing it in.
After that we just cleaned up the repo and the code a little.

## Usage
```
./blackjack player_config_file ai_tables_file
```

The the `player_config_file` is were you say how many players are going to play
and how many decks the dealer will use. Format is as follows:

The first line is `DECKS-PLAY`. The number of decks used must be between 4 and 8
and the number of players between 1 and 4 (inclusive). 
The following lines each represent a single player in the format
`TYPE-NAME-MONEY-BET`.

* TYPE: player type (EA or HU);
* NAME: player's name;
* MONEY: starting money (between 10 and 500);
* BET: starting amount to bet (between 2 and 25% of MONEY);

The `ai_tables_file` represents the strategy that the AI player should
use when playing.

## Dependencies
Needs the following C libraries:

* `sdl2`
* `sdl2_ttf`
* `sdl2_image`

## Install
```
git clone https://github.com/cguariglia/blackjack.git
cd blackjack
make
```
