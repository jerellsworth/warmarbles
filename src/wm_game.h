#ifndef WM_GAME_H
#define WM_GAME_H

#include "bh.h"

#define WM_GAME_N_MARBLES 5
#define WM_GAME_WINNING_SCORE 7

enum WM_GameState_e {
    WM_GAME_STATE_IN_PROGRESS,
    WM_GAME_STATE_PAUSED,
    WM_GAME_STATE_END_OF_ROUND
};

struct WM_Game_s {
    WM_Guy *guy1;
    WM_Guy *guy2;
    WM_Physics *target;
    u8 p1_score;
    u8 p2_score;
    WM_GameState state;
    u8 marbles_in_tray[2];
    WM_Board *board;
    u16 next_tile_idx;
    u8 n_players;
    WM_SFX *sfx;
};

WM_Game *Game_init(u8 n_players);
void WM_Game_run(WM_Game *g);
void WM_Game_del(WM_Game *g);
void WM_Game_draw_score(WM_Game *g);
void WM_Game_score(WM_Game *g, u8 player);

// tray is 0 for left, 1 for right
void WM_Game_change_tray_marbles(WM_Game *g, u8 tray, u8 diff);

#endif
