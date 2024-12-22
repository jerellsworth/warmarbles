#ifndef GAME_H
#define GAME_H

#include "bh.h"

#define GAME_N_MARBLES 5

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
    u8 marbles_in_tray[2];
    Board *board;
    u16 next_tile_idx;
};

Game *Game_init(void);
void Game_run(Game *g);
void Game_del(Game *g);
void Game_draw_score(Game *g);
void Game_score(Game *g, u8 player);

// tray is 0 for left, 1 for right
void Game_change_tray_marbles(Game *g, u8 tray, u8 diff);

#endif
