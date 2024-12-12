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
    if (joy & BUTTON_UP) {
        Guy_move(p->guy, 0, -FIX16(4));
    } else if (joy & BUTTON_DOWN) {
        Guy_move(p->guy, 0, FIX16(4));
    }
    if (p->cooldown > 0) {
        --p->cooldown;
        return;
    }
    if (joy & BUTTON_B) {
        Guy_throw(p->guy);
    }
}
