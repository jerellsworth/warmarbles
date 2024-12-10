#include "bh.h"


Game *Game_init(void) {
    Game *g = st_calloc(1, sizeof(Game));
    SPR_init();
    PAL_setPalette(PAL1, PAL_MARBLE.data, DMA);
    return g;
}

void Game_run(Game *g) {
    Board *b = Board_init();
    g->tube1 = Tube_init(0, FALSE);
    Player *p1 = Player_init(g, g->tube1);
    Physics *m1 = Physics_init_marble(FIX16(32), FIX16(40));
    m1->dx = FIX16(1);
    Physics *m2 = Physics_init_marble(FIX16(320 - 32), FIX16(44));
    m2->dx = FIX16(-1);
    while (TRUE) {
        Player_update(p1);
        Physics_update_all();
        SPR_update();
        SYS_doVBlankProcess();
    }
    Board_del(b);
}

void Game_del(Game *g) {
    free(g);
}
