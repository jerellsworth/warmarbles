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
    u16 joy = JOY_readJoypad(JOY_1);
    if (joy & BUTTON_UP) {
        Tube_move(p->tube, -FIX16(4));
    } else if (joy & BUTTON_DOWN) {
        Tube_move(p->tube, FIX16(4));
    }
    if (p->cooldown > 0) {
        --p->cooldown;
        return;
    }
}
