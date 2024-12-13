#ifndef PLAYER_H
#define PLAYER_H

#include "bh.h"

struct Player_s {
    Game *game;
    Guy *guy;
    u8 joy;
    u8 ai;

    fix16 ai_dy;
    u16 ai_frames_alive;
};

Player *Player_init(Game *g, Guy *guy, u8 joy, u8 ai);
void Player_del(Player *p);

void Player_update(Player *p);

#endif
