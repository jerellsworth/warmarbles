#include "bh.h"

Player *Player_init(Game *g, Guy *guy) {
    Player *p = st_calloc(1, sizeof(Player));
    p->game = g;
    p->guy = guy;
    return p;
}

void Player_del(Player *p) {
    free(p);
}

void Player_update(Player *p) {
    u16 joy = JOY_readJoypad(JOY_1);
    fix16 dx, dy;
    if (joy & BUTTON_UP) {
        dy = -FIX16(4);
    } else if (joy & BUTTON_DOWN) {
        dy = FIX16(4);
    }
    if (joy & BUTTON_LEFT) {
        dx = -FIX16(4);
    } else if (joy & BUTTON_RIGHT) {
        dx = FIX16(4);
    }
    Guy_move(p->guy, dx, dy);
    if (joy & BUTTON_B) {
        Guy_throw(p->guy);
    }
}
