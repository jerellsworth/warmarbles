#include "bh.h"

struct Game_s {
    Guy *guy1;
    Guy *guy2;
};

Game *Game_init(void);
void Game_run(Game *g);
void Game_del(Game *g);
