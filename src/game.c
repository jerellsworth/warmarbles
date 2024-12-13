#include "bh.h"

Game *Game_init(void) {
    Game *g = st_calloc(1, sizeof(Game));
    SPR_init();
    PAL_setPalette(PAL1, PAL_MARBLE.data, DMA);
    PAL_setPalette(PAL2, PAL_GUY.data, DMA);
    return g;
}

Physics *Game_enter_marble(void) {
    fix16 x, y, dx, dy;
    x = FIX16(160);
    if (random_with_max(1)) {
        y = BOARD_HEIGHT >> 2;
    } else {
        y = BOARD_HEIGHT - (BOARD_HEIGHT >> 2);
    }
    u16 theta = random_with_max(1023);
    dx = cosFix16(theta) << 2;
    dy = sinFix16(theta) << 2;
    Physics *m = Physics_init_marble(x, y);
    m->dx = dx;
    m->dy = dy;
    return m;
}

void Game_run(Game *g) {
    Board *b = Board_init();
    Physics *marbles[GAME_N_MARBLES];
    g->guy1 = Guy_init(0, 0, FALSE);
    Player *p1 = Player_init(g, g->guy1);
    Physics *target = Physics_init_target(FIX16(160), FIX16(112));
    u8 n_marbles = 0;
    u8 frames_to_marble = 60;
    while (TRUE) {
        --frames_to_marble;
        if (frames_to_marble == 0) {
            frames_to_marble = 60;
            if (n_marbles < GAME_N_MARBLES) {
                marbles[n_marbles] = Game_enter_marble();
                ++n_marbles;
            }
        }
        Player_update(p1);
        Guy_update(g->guy1);
        Physics_update_all();
        SPR_update();
        SYS_doVBlankProcess();
    }
    for (u8 i = 0; i < n_marbles; ++i) {
        Physics_del(marbles[i]);
    }
    Physics_del(target);
    Player_del(p1);
    Board_del(b);
    
}

void Game_del(Game *g) {
    free(g);
}
