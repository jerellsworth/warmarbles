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

void Board_del(Board *b) {
    for (u8 r = 0; r < BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < BOARD_HEIGHT_TILES >> 2; ++c) {
            Physics *p = b->doodads[r][c];
            if (p) Physics_del(p);
        }
    }
    free(b);
}

void Board_most_used_cell(Board *b, u16 *row, u16 *col) {
    // TODO don't pick cells in the middle of the board ever
    u16 max_usage = 0;
    for (u8 r = 0; r < BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < BOARD_WIDTH_TILES >> 2; ++c) {
            Physics *doodad = b->doodads[r][c];
            if (!doodad) {
                max_usage = max(max_usage, b->traffic[r][c]);
            }
        }
    }
    u16 candidates[BOARD_HEIGHT_TILES * BOARD_WIDTH_TILES >> 4];
    u16 n_candidates = 0;
    u16 cell_no = 0;
    for (u8 r = 0; r < BOARD_HEIGHT_TILES >> 2; ++r) {
        for (u8 c = 0; c < BOARD_WIDTH_TILES >> 2; ++c) {
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
    Physics *doodad = Physics_init_bumper(
        FIX16((col << 5) + 16 + 24),
        FIX16((row << 5) + 16),
        b->game
    );
    b->doodads[row][col] = doodad;
    ++b->n_doodads;
    return doodad;
}
