#include "wm.h"

WM_Game *WM_Game_init(u8 n_players, SFX *sfx) {
    WM_Game *g = st_calloc(1, sizeof(WM_Game));
    g->n_players = n_players;
    g->sfx = WM_SFX_init();
    SPR_init();
    PAL_setPalette(PAL0, PAL_WM_BOARD.data, DMA);
    PAL_setPalette(PAL1, PAL_WM_MARBLE.data, DMA);
    PAL_setPalette(PAL2, PAL_WM_GUY.data, DMA);
    PAL_setPalette(PAL3, PAL_WM_TARGET.data, DMA);
    VDP_setTextPalette(PAL1);
    VDP_loadFont(&TLS_WM_FONT, DMA);
    XGM_startPlay(XGM_WM_battle);
    return g;
}

Physics *WM_Game_enter_marble(WM_Game *g) {
    SFX_incidental(g->sfx, WM_SND_SAMPLE_MARBLE_ENTER);
    fix16 x, y;
    x = FIX16(160);
    if (random_with_max(1)) {
        y = WM_BOARD_HEIGHT >> 2;
    } else {
        y = WM_BOARD_HEIGHT - (WM_BOARD_HEIGHT >> 2);
    }
    WM_Physics *m = WM_Physics_init_marble(x, y, g);
    return m;
}

void WM_Game_score(WM_Game *g, u8 player) {
    SFX_incidental(g->sfx, WM_SND_SAMPLE_SCORE);
    if (player == 0) {
        ++g->p1_score;
    } else {
        ++g->p2_score;
    }
    g->state = WM_GAME_STATE_END_OF_ROUND;
    WM_Game_draw_score(g);
}

void WM_Game_run(WM_Game *g) {
    WM_Board *b = WM_Board_init(g);
    WM_Physics_engine_init(g);
    g->board = b;
    WM_Game_draw_score(g);
    g->guy1 = WM_Guy_init(0, 0, FALSE, g);
    g->guy2 = WM_Guy_init(FIX16(320 - 32), 0, TRUE, g);
    WM_Player *p1 = WM_Player_init(g, g->guy1, JOY_1, 0);
    WM_Player *p2 = WM_Player_init(
        g,
        g->guy2,
        g->n_players > 1 ? JOY_2 : 0,
        g->n_players > 1 ? 0 : 1
        );
    u8 winning_player;
    WM_Guy *losing_guy;
    WM_Board_reset(b);
    u16 rounds_to_bumper = 1;
    while (TRUE) {
        p1->cooldown = 60;
        p2->cooldown = 60;
        g->target = WM_Physics_init_target(FIX16(160), FIX16(112), g);
        g->marbles_in_tray[0] = 0;
        g->marbles_in_tray[1] = 0;
        u8 frames_to_marble = 60;
        g->state = WM_GAME_STATE_IN_PROGRESS;
        while (g->state != WM_GAME_STATE_END_OF_ROUND) {
            WM_Player_update(p1);
            WM_Player_update(p2);
            if (g->state == WM_GAME_STATE_IN_PROGRESS) {
                --frames_to_marble;
                if (frames_to_marble == 0) {
                    frames_to_marble = 60;
                    if (WM_Physics_count_type(WM_PHYSICS_T_MARBLE) < WM_GAME_N_MARBLES) {
                        WM_Game_enter_marble(g);
                    }
                }
                WM_Guy_update(g->guy1);
                WM_Guy_update(g->guy2);
                WM_Physics_update_all();
                SPR_update();
            }
            SYS_doVBlankProcess();
        }
        WM_Guy_throw_cancel(g->guy1);
        WM_Guy_throw_cancel(g->guy2);
        WM_Physics_del_type(PHYSICS_T_MARBLE);
        WM_Physics_del(g->target);

        if (g->p1_score >= WM_GAME_WINNING_SCORE) {
            winning_player = 0;
            losing_guy = g->guy2;
            break;
        } else if (g->p2_score >= WM_GAME_WINNING_SCORE) {
            winning_player = 1;
            losing_guy = g->guy1;
            break;
        }

        --rounds_to_bumper;
        if (rounds_to_bumper == 0) {
            rounds_to_bumper = 2;
            WM_Board_add_doodad(b, WM_PHYSICS_T_BUMPER);
        }
    }
    WM_Board_clear_doodads(b);
    XGM_startPlay(XGM_WM_ending_sting);
    VDP_drawImageEx(
        BG_A,
        winning_player == 0 ? &IMG_WM_PLAYER_ONE_WINS : &IMG_WM_PLAYER_TWO_WINS,
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

    PAL_fadeOutAll(30, FALSE);
    WM_Player_del(p1);
    WM_Player_del(p2);
    WM_Board_del(b);
}

void WM_Game_del(Game *g) {
    WM_SFX_del(g->sfx);
    VDP_init();
    free(g);
}

void WM_Game_draw_score(WM_Game *g) {
    char buf[3];
    sprintf(buf, "%02d", g->p1_score);
    VDP_drawText(buf, 14, 27);
    sprintf(buf, "%02d", g->p2_score);
    VDP_drawText(buf, 24, 27);
}

void WM_Game_change_tray_marbles(WM_Game *g, u8 tray, u8 diff) {
    g->marbles_in_tray[tray] += diff;

    /*
    char buf[4];
    sprintf(buf, "%d,%d", g->marbles_in_tray[0], g->marbles_in_tray[1]);
    VDP_drawText(buf, 0, 0);
    */
    for (u8 i = 0; i < 2; ++i) {
        if (g->marbles_in_tray[i] >= WM_GAME_N_MARBLES) {
            Game_score(g, 1 - i);
        }
    }
}
