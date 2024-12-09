#include "bh.h"

Player *Player_init(Game *g, Tube *t) {
    Player *p = st_calloc(1, sizeof(Player));
    p->game = g;
    p->tube = t;
    return p;
}

void Player_del(Player *p) {
    free(p);
}

void Player_update(Player *p) {
    // TODO
}
