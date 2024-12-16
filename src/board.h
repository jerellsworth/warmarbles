#ifndef BOARD_H
#define BOARD_H

#include "bh.h"

#define BOARD_HEIGHT_TILES 27
#define BOARD_WIDTH_TILES 34
#define BOARD_HEIGHT FIX16(216)
#define BOARD_WIDTH FIX16(272)

struct Board_s {
    Game *game;
    u16 traffic[BOARD_HEIGHT_TILES >> 2][BOARD_WIDTH_TILES >> 2];
    Physics *doodads[BOARD_HEIGHT_TILES >> 2][BOARD_WIDTH_TILES >> 2];
    u8 n_doodads;
};

void Board_reset(Board *b);
Board *Board_init(Game *g);
void Board_del(Board *b);

void Board_most_used_cell(Board *b, u16 *row, u16 *col);
Physics *Board_add_doodad(Board *b, PhysicsType t);

#endif
