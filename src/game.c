#include "bh.h"


Game *Game_init(void) {
    Game *g = st_calloc(1, sizeof(Game));
    PAL_setPalette(PAL1, PAL_MARBLE.data, DMA);
    return g;
}

void Game_run(Game *g) {
    Board *b = Board_init();
    Physics_init_marble(FIX16(160), FIX16(112));
    while (TRUE) {
        Physics_update_all();
        SPR_update();
        SYS_doVBlankProcess();
    }
    Board_del(b);
}

void Game_del(Game *g) {
    free(g);
}
