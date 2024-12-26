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
    if (p->game->state == GAME_STATE_PAUSED) return;
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
    if (p->cooldown > 0) --p->cooldown;
    if (p->cooldown == 0 && (joy & BUTTON_START)) {
        if (p->game->state == GAME_STATE_IN_PROGRESS) {
            SFX_incidental(p->game->sfx, SND_SAMPLE_CONFIRM);
            p->game->state = GAME_STATE_PAUSED;
            p->cooldown = 60;
        } else if (p->game->state == GAME_STATE_PAUSED) {
            SFX_incidental(p->game->sfx, SND_SAMPLE_CONFIRM);
            p->cooldown = 60;
            p->game->state = GAME_STATE_IN_PROGRESS;
        }
    }
    if (p->game->state == GAME_STATE_PAUSED) return;

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
