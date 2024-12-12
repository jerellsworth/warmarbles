#ifndef GUY_H
#define GUY_H

#include "bh.h"

struct Guy_s {
    fix16 x;
    fix16 y;
    Sprite *sprite;
};

Guy *Guy_init(fix16 x, fix16 y, bool reversed);
void Guy_del(Guy *t);
void Guy_move(Guy *t, fix16 dx, fix16 dy);

#endif
