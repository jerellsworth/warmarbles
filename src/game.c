#include "bh.h"

Game *Game_init(void) {
    Game *g = st_calloc(1, sizeof(Game));
    SPR_init();
    VDP_loadFont(&TLS_FONT, DMA);
    VDP_setTextPalette(PAL1);
    PAL_setPalette(PAL1, PAL_MARBLE.data, DMA);
    PAL_setPalette(PAL2, PAL_GUY.data, DMA);
    return g;
}

Physics *Game_enter_marble(Game *g) {
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
    Physics *m = Physics_init_marble(x, y, g);
    m->dx = dx;
    m->dy = dy;
    return m;
}

void Game_score(Game *g, u8 player) {
    if (player == 0) {
        ++g->p1_score;
    } else {
        ++g->p2_score;
    }
    g->state = GAME_STATE_END_OF_ROUND;
    Game_draw_score(g);
}

void Game_run(Game *g) {
    Board *b = Board_init();
    Game_draw_score(g);
    g->guy1 = Guy_init(0, 0, FALSE);
    g->guy2 = Guy_init(FIX16(320 - 32), 0, TRUE);
    Player *p1 = Player_init(g, g->guy1, JOY_1, 0);
    Player *p2 = Player_init(g, g->guy2, 0, 1);
    while (TRUE) {
        Physics *marbles[GAME_N_MARBLES];
        Physics *target = Physics_init_target(FIX16(160), FIX16(112), g);
        u8 n_marbles = 0;
        u8 frames_to_marble = 60;
        g->state = GAME_STATE_IN_PROGRESS;
        while (g->state == GAME_STATE_IN_PROGRESS) {
            --frames_to_marble;
            if (frames_to_marble == 0) {
                frames_to_marble = 60;
                if (n_marbles < GAME_N_MARBLES) {
                    marbles[n_marbles] = Game_enter_marble(g);
                    ++n_marbles;
                }
            }
            Player_update(p1);
            Player_update(p2);
            Guy_update(g->guy1);
            Guy_update(g->guy2);
            Physics_update_all();
            SPR_update();
            SYS_doVBlankProcess();
        }
        for (u8 i = 0; i < n_marbles; ++i) {
            Physics_del(marbles[i]);
        }
        Physics_del(target);
    }
    Player_del(p1);
    Player_del(p2);
    Board_del(b);
    
}

void Game_del(Game *g) {
    free(g);
}

void Game_draw_score(Game *g) {
    char buf[3];
    sprintf(buf, "%02d", g->p1_score);
    VDP_drawText(buf, 14, 27);
    sprintf(buf, "%02d", g->p2_score);
    VDP_drawText(buf, 24, 27);
}
