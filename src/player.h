#ifndef PLAYER_H
#define PLAYER_H

#include "bh.h"

struct Player_s {
    Game *game;
    Guy *guy;
    u8 cooldown;
};

Player *Player_init(Game *g, Guy *guy);
void Player_del(Player *p);

void Player_update(Player *p);

#endif
