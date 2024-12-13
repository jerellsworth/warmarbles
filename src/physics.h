#ifndef PHYSICS_H
#define PHYSICS_H

#include "bh.h"

#define PHYSICS_MAX_OBJECTS 20
#define PHYSICS_NEARBY_THRESH FIX32(64)
#define PHYSICS_MAX_VELOCITY_VECTOR FIX16(6)

#define DRAG FIX16(0.025)

enum PhysicsType_e {
    PHYSICS_T_MARBLE,
    PHYSICS_T_TARGET
};

struct Physics_s {
    Game *game;
    u8 reg_idx;

    fix16 x;
    fix16 y;
    fix16 dx;
    fix16 dy;

    fix16 r; // radius
    fix16 m; // mass
    fix16 inv_m; // 1/mass

    Sprite *sprite;
    fix16 sprite_offset_x;
    fix16 sprite_offset_y;

    u8 frames_alive;

    bool has_collision;
    bool in_tray;
    u8 tray_no;
    PhysicsType type;
};

extern Physics **ALL_PHYSICS;

Physics *Physics_init(Game *g);
void Physics_del(Physics *p);

bool Physics_check_collision(Physics *p1, Physics *p2);
void Physics_update(Physics *p);
void Physics_update_all(void);

Physics *Physics_init_marble(fix16 x, fix16 y, Game *g);
Physics *Physics_init_target(fix16 x, fix16 y, Game *g);

Physics *Physics_find_nearby(fix16 x, fix16 y, PhysicsType t);

#endif
