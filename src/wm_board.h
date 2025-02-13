#ifndef WM_BOARD_H
#define WM_BOARD_H

#include "wm.h"

#define WM_BOARD_HEIGHT_TILES 27
#define WM_BOARD_WIDTH_TILES 34
#define WM_BOARD_HEIGHT FIX16(216)
#define WM_BOARD_WIDTH FIX16(272)

struct WM_Board_s {
    WM_Game *game;
    u16 traffic[WM_BOARD_HEIGHT_TILES >> 2][WM_BOARD_WIDTH_TILES >> 2];
    WM_Physics *doodads[WM_BOARD_HEIGHT_TILES >> 2][WM_BOARD_WIDTH_TILES >> 2];
    u8 n_doodads;
};

void WM_Board_reset(WM_Board *b);
WM_Board *WM_Board_init(WM_Game *g);
void WM_Board_del(WM_Board *b);

void WM_Board_most_used_cell(WM_Board *b, u16 *row, u16 *col);
WM_Physics *WM_Board_add_doodad(WM_Board *b, WM_PhysicsType t);
void WM_Board_clear_doodads(WM_Board *b);

#endif
