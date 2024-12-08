#ifndef PHYSICS_H
#define PHYSICS_H

#include "bh.h"

#define PHYSICS_MAX_OBJECTS 80

struct Physics_s {
    u8 reg_idx;
    fix16 x;
    fix16 y;
    fix16 dx;
    fix16 dy;

    Sprite *sprite;
    fix16 sprite_offset_x;
    fix16 sprite_offset_y;
};

extern Physics **ALL_PHYSICS;

Physics *Physics_init(void);
void Physics_del(Physics *p);

void Physics_update(Physics *p);
void Physics_update_all(void);

Physics *Physics_init_marble(fix16 x, fix16 y);

#endif
