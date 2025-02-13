#ifndef WM_PLAYER_H
#define WM_PLAYER_H

#include "wm.h"

struct WM_Player_s {
    u8 cooldown;
    WM_Game *game;
    WM_Guy *guy;
    u8 joy;
    u8 ai;

    fix16 ai_dy;
    u16 ai_frames_alive;
    u16 ai_frames_next_choice;
};

WM_Player *WM_Player_init(WM_Game *g, WM_Guy *guy, u8 joy, u8 ai);
void WM_Player_del(WM_Player *p);

void WM_Player_update(WM_Player *p);

#endif
