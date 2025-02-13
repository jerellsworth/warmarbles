#ifndef GUY_H
#define GUY_H

#include "bh.h"

#define GUY_FRAMES_PER_ANIM 5
#define GUY_ENGLISH_PER_FRAME FIX16(0.2)

struct Guy_s {
    Game *game;
    fix16 x;
    fix16 y;
    bool reversed;
    Sprite *sprite;
    u16 throw_frames;
    bool walking_m0;
    bool walking_m1;
    Physics *holding;
    fix16 throw_dy; // TODO depends on frames holding direction during throw
    fix16 throw_dx;

    fix16 x_offset_center;
    fix16 x_offset_marble;
};

Guy *Guy_init(fix16 x, fix16 y, bool reversed, Game *g);
void Guy_del(Guy *t);
void Guy_move(Guy *t, fix16 dx, fix16 dy);
void Guy_grab(Guy *g);
void Guy_throw(Guy *t);
void Guy_update(Guy *t);
void Guy_throw_cancel(Guy *g);

#endif
