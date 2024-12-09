#ifndef PLAYER_H
#define PLAYER_H

#include "bh.h"

struct Player_s {
    Game *game;
    Tube *tube;
};

Player *Player_init(Game *g, Tube *t);
void Player_del(Player *p);

void Player_update(Player *p);

#endif
