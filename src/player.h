#ifndef PLAYER_H
#define PLAYER_H

#include "bh.h"

struct Player_s {
    u8 cooldown;
    Game *game;
    Guy *guy;
    u8 joy;
    u8 ai;

    fix16 ai_dy;
    u16 ai_frames_alive;
    u16 ai_frames_next_choice;
};

Player *Player_init(Game *g, Guy *guy, u8 joy, u8 ai);
void Player_del(Player *p);

void Player_update(Player *p);

#endif
