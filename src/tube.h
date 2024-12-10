#ifndef TUBE_H
#define TUBE_H

#include "bh.h"

struct Tube_s {
    fix16 x;
    fix16 y;
    Sprite *sprite;
    u8 frames_until_loaded;
    bool loaded;
};

Tube *Tube_init(fix16 x, bool reversed);
void Tube_del(Tube *t);
void Tube_move(Tube *t, fix16 dy);

#endif
