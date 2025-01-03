#include "bh.h"

Guy *Guy_init(fix16 x, fix16 y, bool reversed, Game *game) {
    Guy *g = st_calloc(1, sizeof(Guy));
    g->game = game;
    g->x = x;
    g->y = y;
    g->reversed = reversed;
    const SpriteDefinition *spr_def;
    if (reversed) {
        spr_def = &SPR_GUY2;
        g->x_offset_center = FIX16(40 - 12);
        g->x_offset_marble = FIX16(40 - 20);
    } else {
        spr_def = &SPR_GUY;
        g->x_offset_center = FIX16(12);
        g->x_offset_marble = FIX16(20);
    }
    g->sprite = SPR_addSprite(
        spr_def,
        fix16ToRoundedInt(x),
        fix16ToRoundedInt(y),
        TILE_ATTR(PAL2, TRUE, FALSE, reversed) 
        );
    return g;
}

void Guy_del(Guy *t) {
    SPR_releaseSprite(t->sprite);
    free(t);
}

void Guy_move(Guy *g, fix16 dx, fix16 dy) {
    if (dx == 0 && dy == 0) return;
    if (g->throw_frames > 0) {
        if (dx > 0 && (!g->reversed)) g->throw_dx += GUY_ENGLISH_PER_FRAME;
        else if (dx < 0 && g->reversed) g->throw_dx -= GUY_ENGLISH_PER_FRAME;
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
    fix16 center_x = g->x + g->x_offset_center;
    fix16 center_y = g->y + FIX16(64);
    Physics *near = Physics_find_nearby(center_x, center_y, PHYSICS_T_MARBLE);
    if (!near) return;
    g->throw_frames = GUY_FRAMES_PER_ANIM * 10;
    g->holding = near;
    near->has_collision = FALSE;
    if (near->in_tray) {
        near->in_tray = FALSE;
        Game_change_tray_marbles(g->game, near->tray_no, -1);
    }
    near->x = g->x + g->x_offset_marble;
    near->y = g->y + FIX16(48);
    near->dx = 0;
    near->dy = FIX16(-40 / 2 / GUY_FRAMES_PER_ANIM); // traveling 40 pixels in 2 animations which take GUY_FRAMES_PER_ANIM frames
    g->throw_dy = 0;
    g->throw_dx = g->reversed ? -FIX16(3) : FIX16(3);
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
            SFX_incidental(g->game->sfx, SND_SAMPLE_THROW);
            g->holding = NULL;
        } else if (g->throw_frames == 0) {
            SPR_setAnim(g->sprite, 0);
        }
    }
}

void Guy_throw_cancel(Guy *g) {
    if (g->holding) {
        g->holding->has_collision = TRUE;
        g->holding->dx = 0;
        g->holding->dy = 0;
        g->holding = NULL;
    }
    g->throw_frames = 0;
    SPR_setAnim(g->sprite, 0);
}
