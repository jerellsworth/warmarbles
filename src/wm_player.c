#include "wm.h"

WM_Player *WM_Player_init(WM_Game *g, WM_Guy *guy, u8 joy, u8 ai) {
    WM_Player *p = st_calloc(1, sizeof(WM_Player));
    p->game = g;
    p->guy = guy;
    p->joy = joy;
    p->ai = ai;

    p->ai_dy = FIX16(2);
    return p;
}

void WM_Player_del(WM_Player *p) {
    free(p);
}

void _ai(WM_Player *p) {
    if (p->game->state == WM_GAME_STATE_PAUSED) return;
    ++p->ai_frames_alive;
    WM_Guy *g = p->guy;
    if (g->throw_frames == WM_GUY_FRAMES_PER_ANIM * (10 - 5) + 1) {
        WM_Physics *target = p->game->target;
        fix16 throw_center_x = g->x + g->x_offset_marble;
        fix16 throw_center_y = g->y + FIX16(8);
        fix16 dx = target->x - throw_center_x;
        fix16 dy = target->y - throw_center_y;
        fix16 norm_x, norm_y;
        normalize(dx, dy, FIX16(4.5), &norm_x, &norm_y);
        g->throw_dx = min(norm_x, -FIX16(1));
        g->throw_dy = norm_y;
    } else if (g->throw_frames > 0) {
    } else {
        if (p->ai_frames_next_choice > 0) {
            --p->ai_frames_next_choice;
        } else {
            u16 marbles_up = 0;
            u16 marbles_down = 0;
            for (u8 i = 0; i < WM_PHYSICS_MAX_OBJECTS; ++i) {
                WM_Physics *ph = WM_ALL_PHYSICS[i];
                if (!ph) continue;
                if (ph->type != WM_PHYSICS_T_MARBLE) continue;
                if (ph->in_tray && ph->tray_no == 1) {
                    // TODO only works if ai is on right side
                    if (ph->y < g->y + FIX16(64)) {
                        marbles_up += 10;
                    } else {
                        marbles_down += 10;
                    }
                } else {
                    if (ph->y < g->y + FIX16(64)) {
                        marbles_up += 1;
                    } else {
                        marbles_down += 1;
                    }
                }
            }
            if (marbles_up > marbles_down) {
                p->ai_dy = -FIX16(2);
            } else {
                p->ai_dy = FIX16(2);
            }
            p->ai_frames_next_choice = 15;
        }
        WM_Guy_move(g, 0, p->ai_dy);
    }
    if (!(p->ai_frames_alive & 7)) {
        WM_Guy_grab(g);
        WM_Guy_throw(g);
    }
}

void WM_Player_update(WM_Player *p) {
    if (p->ai) return _ai(p);
    u16 joy = JOY_readJoypad(p->joy);
    if (p->cooldown > 0) --p->cooldown;
    if (p->cooldown == 0 && (joy & BUTTON_START)) {
        if (p->game->state == WM_GAME_STATE_IN_PROGRESS) {
            WM_SFX_incidental(p->game->sfx, WM_SND_SAMPLE_CONFIRM);
            p->game->state = WM_GAME_STATE_PAUSED;
            p->cooldown = 30;
        } else if (p->game->state == WM_GAME_STATE_PAUSED) {
            WM_SFX_incidental(p->game->sfx, WM_SND_SAMPLE_CONFIRM);
            p->cooldown = 30;
            p->game->state = WM_GAME_STATE_IN_PROGRESS;
        }
    }
    if (p->game->state == WM_GAME_STATE_PAUSED) return;

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
    WM_Guy_move(p->guy, dx, dy);
    if (joy & (BUTTON_A | BUTTON_B | BUTTON_C)) {
        if (p->guy->holding && p->cooldown == 0) {
            WM_Guy_throw(p->guy);
        } else if (!p->guy->holding) {
            WM_Guy_grab(p->guy);
            p->cooldown = 10;
        }
    }
}
