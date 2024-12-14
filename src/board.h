#ifndef BOARD_H
#define BOARD_H

#include "bh.h"

#define BOARD_HEIGHT_TILES 27
#define BOARD_WIDTH_TILES 34
#define BOARD_HEIGHT FIX16(216)
#define BOARD_WIDTH FIX16(272)

struct Board_s {
    u16 traffic[BOARD_HEIGHT_TILES][BOARD_WIDTH_TILES];
};

void Board_reset(Board *b);
Board *Board_init(void);
void Board_del(Board *b);

#endif
