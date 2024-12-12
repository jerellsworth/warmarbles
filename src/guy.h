#ifndef GUY_H
#define GUY_H

#include "bh.h"

struct Guy_s {
    fix16 x;
    fix16 y;
    Sprite *sprite;
    u16 throw_frames;
    bool walking_m0;
    bool walking_m1;
};

Guy *Guy_init(fix16 x, fix16 y, bool reversed);
void Guy_del(Guy *t);
void Guy_move(Guy *t, fix16 dx, fix16 dy);
void Guy_throw(Guy *t);
void Guy_update(Guy *t);

#endif
