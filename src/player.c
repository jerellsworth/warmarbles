#include "bh.h"

Player *Player_init(Game *g, Guy *guy, u8 joy, u8 ai) {
    Player *p = st_calloc(1, sizeof(Player));
    p->game = g;
    p->guy = guy;
    p->joy = joy;
    p->ai = ai;

    p->ai_dy = FIX16(2);
    return p;
}

void Player_del(Player *p) {
    free(p);
}

void _ai(Player *p) {
    ++p->ai_frames_alive;
    Guy *g = p->guy;
    if (g->y <= -FIX16(40)) {
        p->ai_dy = FIX16(2);
    } else if (g->y >= FIX16(224 - 72)) {
        p->ai_dy = -FIX16(2);
    }
    Guy_move(g, 0, p->ai_dy);
    if (!(p->ai_frames_alive & 7)) {
        Guy_throw(g);
    }
}

void Player_update(Player *p) {
    if (p->ai) return _ai(p);
    u16 joy = JOY_readJoypad(p->joy);
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
