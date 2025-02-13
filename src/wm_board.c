#include "wm.h"

WM_Board *WM_Board_init(WM_Game *g) {
    WM_Board *b = st_calloc(1, sizeof(WM_Board));
    b->game = g;
    VDP_drawImage(BG_B, &IMG_WM_BOARD, 0, 0);
    g->next_tile_idx = TILE_USER_INDEX + (IMG_WM_BOARD.tileset)->numTile;
    return b;
}

void WM_Board_reset(WM_Board *b) {
    for (u8 r = 0; r < WM_BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < WM_BOARD_WIDTH_TILES >> 2; ++c) {
            b->traffic[r][c] = 0;
        }
    }
}

void WM_Board_clear_doodads(WM_Board *b) {
    for (u8 r = 0; r < WM_BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < WM_BOARD_WIDTH_TILES >> 2; ++c) {
            WM_Physics *p = b->doodads[r][c];
            if (p) {
                WM_Physics_del(p);
                b->doodads[r][c] = NULL;
            }
        }
    }
}

void WM_Board_del(WM_Board *b) {
    WM_Board_clear_doodads(b);
    free(b);
}

void WM_Board_most_used_cell(WM_Board *b, u16 *row, u16 *col) {
    // TODO don't pick cells in the middle of the board ever
    u16 max_usage = 0;
    for (u8 r = 0; r < WM_BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < WM_BOARD_WIDTH_TILES >> 2; ++c) {
            if ((c == 3 || c == 4) && (r >= 2 && r <= 4)) continue;
            WM_Physics *doodad = b->doodads[r][c];
            if (!doodad) {
                max_usage = max(max_usage, b->traffic[r][c]);
            }
        }
    }
    u16 candidates[(WM_BOARD_HEIGHT_TILES * WM_BOARD_WIDTH_TILES) >> 4];
    u16 n_candidates = 0;
    u16 cell_no = 0;
    for (u8 r = 0; r < WM_BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < WM_BOARD_WIDTH_TILES >> 2; ++c) {
            if ((c == 3 || c == 4) && (r >= 2 && r <= 4)) {
                ++cell_no;
                continue;
            }
            u16 usage = b->traffic[r][c];
            WM_Physics *doodad = b->doodads[r][c];
            if (usage == max_usage && (!doodad)) {
                candidates[n_candidates] = cell_no;
                ++n_candidates;
            }
            ++cell_no;
        }
    }
    u16 cell_no_winner = candidates[random_with_max(n_candidates - 1)];
    *row = cell_no_winner / (WM_BOARD_WIDTH_TILES >> 2);
    *col = cell_no_winner % (WM_BOARD_WIDTH_TILES >> 2);
}

WM_Physics *Board_add_doodad(WM_Board *b, WM_PhysicsType t) {

    u16 row, col;
    WM_Board_most_used_cell(b, &row, &col);
    fix16 x = FIX16((col << 5) + 16 + 24);
    if (col > 3) x += FIX16(16);
    fix16 y = FIX16((row << 5) + 16);
    if (row > 2) y += FIX16(16);
    WM_Physics *doodad = WM_Physics_init_bumper(
        x,
        y,
        b->game
    );
    b->doodads[row][col] = doodad;
    ++b->n_doodads;
    return doodad;
}
