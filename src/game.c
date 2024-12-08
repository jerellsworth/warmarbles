#include "bh.h"


Game *Game_init(void) {
    Game *g = st_calloc(1, sizeof(Game));
    return g;
}

void Game_run(Game *g) {
    Board *b = Board_init();
    while (TRUE) {
    }
    Board_del(b);
}

void Game_del(Game *g) {
    free(g);
}
