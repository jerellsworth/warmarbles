#ifndef GAME_H
#define GAME_H

#include "bh.h"

#define GAME_N_MARBLES 4

struct Game_s {
    Guy *guy1;
    Guy *guy2;
};

Game *Game_init(void);
void Game_run(Game *g);
void Game_del(Game *g);

#endif
