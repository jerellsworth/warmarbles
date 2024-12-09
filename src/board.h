#ifndef BOARD_H
#define BOARD_H

#include "bh.h"

#define BOARD_HEIGHT FIX16(216)
#define BOARD_WIDTH FIX16(272)

struct Board_s {
    u8 PLACEHOLDER;
};

Board *Board_init(void);
void Board_del(Board *b);

#endif
