#include "bh.h"

struct Game_s {
    Tube *tube1;
    Tube *tube2;
};

Game *Game_init(void);
void Game_run(Game *g);
void Game_del(Game *g);
