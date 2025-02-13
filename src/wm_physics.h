#ifndef WM_PHYSICS_H
#define WM_PHYSICS_H

#include "wm.h"

#define WM_PHYSICS_MAX_OBJECTS 20
#define WM_PHYSICS_NEARBY_THRESH FIX32(64)
#define WM_PHYSICS_MAX_VELOCITY_VECTOR FIX16(6)
#define WM_PHYSICS_MAX_VELOCITY_VECTOR_TARGET FIX16(4)
#define WM_PHYSICS_FRAMES_TO_BUMPER_BREAK 80
#define WM_PHYSICS_SLOW_THRESH FIX16(0.5)

#define WM_DRAG FIX16(0.025)

enum WM_PhysicsType_e {
    WM_PHYSICS_T_MARBLE,
    WM_PHYSICS_T_TARGET,
    WM_PHYSICS_T_BUMPER
};

struct WM_Physics_s {
    WM_Game *game;
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
    bool stationary;
    bool held;

    u8 anim_frames;

    u16 tile_x;
    u16 tile_y;
    u8 tile_w;
    u8 tile_h;
    u16 tile_idx;
    u8 anim;
    u8 anim_frame;
    u8 pal;

    u8 tray_no;
    WM_PhysicsType type;

    u16 collided_frames;
    u16 slow_frames;
    bool broken;

    u8 init_frames;

    u16 ttl;
};

extern WM_Physics **WM_ALL_PHYSICS;

void WM_Physics_engine_init(WM_Game *g);

WM_Physics *WM_Physics_init(WM_Game *g);
void WM_Physics_del(WM_Physics *p);

bool WM_Physics_check_collision(WM_Physics *p1, WM_Physics *p2);
void WM_Physics_update(WM_Physics *p);
void WM_Physics_update_all(void);

WM_Physics *Physics_init_marble(fix16 x, fix16 y, WM_Game *g);
WM_Physics *Physics_init_target(fix16 x, fix16 y, WM_Game *g);
WM_Physics *Physics_init_bumper(fix16 x, fix16 y, WM_Game *g);

WM_Physics *Physics_find_nearby(fix16 x, fix16 y, WM_PhysicsType t);

u8 WM_Physics_count_type(WM_PhysicsType t);
void WM_Physics_del_type(WM_PhysicsType t);

#endif
