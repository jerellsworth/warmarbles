#include "bh.h"

Board *Board_init(void) {
    Board *b = st_calloc(1, sizeof(Board));
    VDP_drawImage(BG_B, &IMG_BOARD, 0, 0);
    return b;
}

void Board_del(Board *b) {
    free(b);
}
