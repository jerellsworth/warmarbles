#ifndef GAME_H
#define GAME_H

#include "bh.h"

#define GAME_N_MARBLES 6

enum GameState_e {
    GAME_STATE_IN_PROGRESS,
    GAME_STATE_END_OF_ROUND
};

struct Game_s {
    Guy *guy1;
    Guy *guy2;
    u8 p1_score;
    u8 p2_score;
    GameState state;
};

Game *Game_init(void);
void Game_run(Game *g);
void Game_del(Game *g);
void Game_draw_score(Game *g);
void Game_score(Game *g, u8 player);

#endif
