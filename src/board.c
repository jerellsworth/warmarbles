#include "bh.h"

Board *Board_init(Game *g) {
    Board *b = st_calloc(1, sizeof(Board));
    b->game = g;
    VDP_drawImage(BG_B, &IMG_BOARD, 0, 0);
    g->next_tile_idx = TILE_USER_INDEX + (IMG_BOARD.tileset)->numTile;
    return b;
}

void Board_reset(Board *b) {
    for (u8 r = 0; r < BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < BOARD_WIDTH_TILES >> 2; ++c) {
            b->traffic[r][c] = 0;
        }
    }
}

void Board_clear_doodads(Board *b) {
    for (u8 r = 0; r < BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < BOARD_WIDTH_TILES >> 2; ++c) {
            Physics *p = b->doodads[r][c];
            if (p) {
                Physics_del(p);
                b->doodads[r][c] = NULL;
            }
        }
    }
}

void Board_del(Board *b) {
    Board_clear_doodads(b);
    free(b);
}

void Board_most_used_cell(Board *b, u16 *row, u16 *col) {
    // TODO don't pick cells in the middle of the board ever
    u16 max_usage = 0;
    for (u8 r = 0; r < BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < BOARD_WIDTH_TILES >> 2; ++c) {
            if ((c == 3 || c == 4) && (r >= 2 && r <= 4)) continue;
            Physics *doodad = b->doodads[r][c];
            if (!doodad) {
                max_usage = max(max_usage, b->traffic[r][c]);
            }
        }
    }
    u16 candidates[(BOARD_HEIGHT_TILES * BOARD_WIDTH_TILES) >> 4];
    u16 n_candidates = 0;
    u16 cell_no = 0;
    for (u8 r = 0; r < BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < BOARD_WIDTH_TILES >> 2; ++c) {
            if ((c == 3 || c == 4) && (r >= 2 && r <= 4)) {
                ++cell_no;
                continue;
            }
            u16 usage = b->traffic[r][c];
            Physics *doodad = b->doodads[r][c];
            if (usage == max_usage && (!doodad)) {
                candidates[n_candidates] = cell_no;
                ++n_candidates;
            }
            ++cell_no;
        }
    }
    u16 cell_no_winner = candidates[random_with_max(n_candidates - 1)];
    *row = cell_no_winner / (BOARD_WIDTH_TILES >> 2);
    *col = cell_no_winner % (BOARD_WIDTH_TILES >> 2);
}

Physics *Board_add_doodad(Board *b, PhysicsType t) {

    u16 row, col;
    Board_most_used_cell(b, &row, &col);
    fix16 x = FIX16((col << 5) + 16 + 24);
    if (col > 3) x += FIX16(16);
    fix16 y = FIX16((row << 5) + 16);
    if (row > 2) y += FIX16(16);
    Physics *doodad = Physics_init_bumper(
        x,
        y,
        b->game
    );
    b->doodads[row][col] = doodad;
    ++b->n_doodads;
    return doodad;
}
