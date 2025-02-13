#ifndef WM_GUY_H
#define WM_GUY_H

#include "wm.h"

#define GUY_FRAMES_PER_ANIM 5
#define GUY_ENGLISH_PER_FRAME FIX16(0.2)

struct WM_Guy_s {
    WM_Game *game;
    fix16 x;
    fix16 y;
    bool reversed;
    Sprite *sprite;
    u16 throw_frames;
    bool walking_m0;
    bool walking_m1;
    WM_Physics *holding;
    fix16 throw_dy; // TODO depends on frames holding direction during throw
    fix16 throw_dx;

    fix16 x_offset_center;
    fix16 x_offset_marble;
};

WM_Guy *WM_Guy_init(fix16 x, fix16 y, bool reversed, WM_Game *g);
void WM_Guy_del(WM_Guy *t);
void WM_Guy_move(WM_Guy *t, fix16 dx, fix16 dy);
void WM_Guy_grab(WM_Guy *g);
void WM_Guy_throw(WM_Guy *t);
void WM_Guy_update(WM_Guy *t);
void WM_Guy_throw_cancel(WM_Guy *g);

#endif
