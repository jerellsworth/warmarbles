#include "bh.h"

Board *Board_init(void) {
    Board *b = st_calloc(1, sizeof(Board));
    VDP_drawImage(BG_B, &IMG_BOARD, 0, 0);
    return b;
}

void Board_reset(Board *b) {
    for (u8 r = 0; r < BOARD_HEIGHT_TILES; ++r) {
        for (u8 c = 0; c < BOARD_WIDTH_TILES; ++c) {
            b->traffic[r][c] = 0;
        }
    }
}

void Board_del(Board *b) {
    free(b);
}
