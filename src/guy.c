#include "bh.h"

Guy *Guy_init(fix16 x, fix16 y, bool reversed) {
    Guy *t = st_calloc(1, sizeof(Guy));
    t->x = x;
    t->y = y;
    t->sprite = SPR_addSprite(
        &SPR_GUY,
        fix16ToRoundedInt(x),
        fix16ToRoundedInt(y),
        TILE_ATTR(PAL2, TRUE, FALSE, reversed) 
        );
    return t;
}

void Guy_del(Guy *t) {
    SPR_releaseSprite(t->sprite);
    free(t);
}

void Guy_move(Guy *g, fix16 dx, fix16 dy) {
    if (dx == 0 && dy == 0) return;
    if (g->throw_frames > 0) {
        if (dx > 0) g->throw_dx += GUY_ENGLISH_PER_FRAME;
        else if (dx < 0) g->throw_dx -= GUY_ENGLISH_PER_FRAME;
        if (dy > 0) g->throw_dy += GUY_ENGLISH_PER_FRAME;
        if (dy < 0) g->throw_dy -= GUY_ENGLISH_PER_FRAME;
        return;
    }
    g->walking_m0 = TRUE;
    if (!g->walking_m1) {
        SPR_setAnim(g->sprite, 1);
    }
    g->y += dy;
    if (g->y <= -FIX16(40)) {
        g->y = -FIX16(40);
    } else if (g->y >= FIX16(224 - 72)) {
        g->y = FIX16(224 - 72);
    }
    SPR_setPosition(
        g->sprite,
        fix16ToRoundedInt(g->x),
        fix16ToRoundedInt(g->y)
    );
}

void Guy_throw(Guy *g) {
    if (g->throw_frames > 0) return;
    fix16 center_x = g->x + FIX16(12);
    fix16 center_y = g->y + FIX16(64);
    Physics *near = Physics_find_nearby(center_x, center_y);
    if (!near) return;
    g->throw_frames = GUY_FRAMES_PER_ANIM * 10;
    g->holding = near;
    near->has_collision = FALSE;
    near->x = g->x + FIX16(20);
    near->y = g->y + FIX16(48);
    near->dx = 0;
    near->dy = FIX16(-40 / 2 / GUY_FRAMES_PER_ANIM); // traveling 40 pixels in 2 animations which take GUY_FRAMES_PER_ANIM frames
    g->throw_dy = 0;
    g->throw_dx = FIX16(3);
    SPR_setAnim(g->sprite, 2);
}

void Guy_update(Guy *g) {
    if (g->throw_frames == 0 && g->walking_m1 && (!g->walking_m0)) {
        SPR_setAnim(g->sprite, 0);
    }
    g->walking_m1 = g->walking_m0;
    g->walking_m0 = FALSE;
    if (g->throw_frames > 0) {
        --g->throw_frames;
        if (g->throw_frames == GUY_FRAMES_PER_ANIM * (10 - 2)) {
            g->holding->dy = 0;
        } else if (g->throw_frames == GUY_FRAMES_PER_ANIM * (10 - 5)) {
            g->holding->dx = g->throw_dx;
            g->holding->dy = g->throw_dy;
            g->holding = NULL;
        } else if (g->throw_frames == 0) {
            SPR_setAnim(g->sprite, 0);
        }
    }
}
