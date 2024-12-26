#include "bh.h"

Game *Game_init(u8 n_players, SFX *sfx) {
    Game *g = st_calloc(1, sizeof(Game));
    g->n_players = n_players;
    g->sfx = sfx;
    SPR_init();
    VDP_setTextPalette(PAL1);
    PAL_setPalette(PAL1, PAL_MARBLE.data, DMA);
    PAL_setPalette(PAL2, PAL_GUY.data, DMA);
    PAL_setPalette(PAL3, PAL_TARGET.data, DMA);
    XGM_startPlay(XGM_battle);
    return g;
}

Physics *Game_enter_marble(Game *g) {
    SFX_incidental(g->sfx, SND_SAMPLE_MARBLE_ENTER);
    fix16 x, y;
    x = FIX16(160);
    if (random_with_max(1)) {
        y = BOARD_HEIGHT >> 2;
    } else {
        y = BOARD_HEIGHT - (BOARD_HEIGHT >> 2);
    }
    Physics *m = Physics_init_marble(x, y, g);
    return m;
}

void Game_score(Game *g, u8 player) {
    SFX_incidental(g->sfx, SND_SAMPLE_SCORE);
    if (player == 0) {
        ++g->p1_score;
    } else {
        ++g->p2_score;
    }
    g->state = GAME_STATE_END_OF_ROUND;
    Game_draw_score(g);
}

void Game_run(Game *g) {
    Board *b = Board_init(g);
    Physics_engine_init(g);
    g->board = b;
    Game_draw_score(g);
    g->guy1 = Guy_init(0, 0, FALSE, g);
    g->guy2 = Guy_init(FIX16(320 - 32), 0, TRUE, g);
    Player *p1 = Player_init(g, g->guy1, JOY_1, 0);
    Player *p2 = Player_init(
        g,
        g->guy2,
        g->n_players > 1 ? JOY_2 : 0,
        g->n_players > 1 ? 0 : 1
        );
    u8 winning_player;
    Guy *losing_guy;
    Board_reset(b);
    u16 rounds_to_bumper = 1;
    while (TRUE) {
        p1->cooldown = 60;
        p2->cooldown = 60;
        Physics *target = Physics_init_target(FIX16(160), FIX16(112), g);
        g->marbles_in_tray[0] = 0;
        g->marbles_in_tray[1] = 0;
        u8 frames_to_marble = 60;
        g->state = GAME_STATE_IN_PROGRESS;
        while (g->state != GAME_STATE_END_OF_ROUND) {
            Player_update(p1);
            Player_update(p2);
            if (g->state == GAME_STATE_IN_PROGRESS) {
                --frames_to_marble;
                if (frames_to_marble == 0) {
                    frames_to_marble = 60;
                    if (Physics_count_type(PHYSICS_T_MARBLE) < GAME_N_MARBLES) {
                        Game_enter_marble(g);
                    }
                }
                Guy_update(g->guy1);
                Guy_update(g->guy2);
                Physics_update_all();
                SPR_update();
            }
            SYS_doVBlankProcess();
        }
        Guy_throw_cancel(g->guy1);
        Guy_throw_cancel(g->guy2);
        Physics_del_type(PHYSICS_T_MARBLE);
        Physics_del(target);

        if (g->p1_score >= GAME_WINNING_SCORE) {
            winning_player = 0;
            losing_guy = g->guy2;
            break;
        } else if (g->p2_score >= GAME_WINNING_SCORE) {
            winning_player = 1;
            losing_guy = g->guy1;
            break;
        }

        --rounds_to_bumper;
        if (rounds_to_bumper == 0) {
            rounds_to_bumper = 3;
            Board_add_doodad(b, PHYSICS_T_BUMPER);
        }
    }
    Board_clear_doodads(b);
    XGM_startPlay(XGM_ending_sting);
    VDP_drawImageEx(
        BG_A,
        winning_player == 0 ? &IMG_PLAYER_ONE_WINS : &IMG_PLAYER_TWO_WINS,
        TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, g->next_tile_idx),
        0,
        0,
        FALSE,
        TRUE
    );
    SPR_setAnim(losing_guy->sprite, 3);
    u16 end_game_frames = 0;
    while (TRUE) {
        ++end_game_frames;
        if (end_game_frames >= 5 * 10) {
            SPR_setVisibility(losing_guy->sprite, HIDDEN);
        }
        if (JOY_readJoypad(JOY_ALL) & BUTTON_START) break;
        
        SPR_update();
        SYS_doVBlankProcess();
    }

    Player_del(p1);
    Player_del(p2);
    Board_del(b);
}

void Game_del(Game *g) {
    VDP_init();
    free(g);
}

void Game_draw_score(Game *g) {
    char buf[3];
    sprintf(buf, "%02d", g->p1_score);
    VDP_drawText(buf, 14, 27);
    sprintf(buf, "%02d", g->p2_score);
    VDP_drawText(buf, 24, 27);
}

void Game_change_tray_marbles(Game *g, u8 tray, u8 diff) {
    g->marbles_in_tray[tray] += diff;

    /*
    char buf[4];
    sprintf(buf, "%d,%d", g->marbles_in_tray[0], g->marbles_in_tray[1]);
    VDP_drawText(buf, 0, 0);
    */
    for (u8 i = 0; i < 2; ++i) {
        if (g->marbles_in_tray[i] >= GAME_N_MARBLES) {
            Game_score(g, 1 - i);
        }
    }
}
