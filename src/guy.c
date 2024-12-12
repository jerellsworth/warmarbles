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
    if (g->throw_frames > 0) return;
    g->y += dy;
    g->x += dx;
    //if (t->g <= 0) t->g = 0;
    //if (t->g >= FIX16(192)) t->g = FIX16(192);
    SPR_setPosition(
        g->sprite,
        fix16ToRoundedInt(g->x),
        fix16ToRoundedInt(g->y)
    );
}

void Guy_throw(Guy *g) {
    if (g->throw_frames > 0) return;
    g->throw_frames = 5 * 10;
    SPR_setAnim(g->sprite, 2);
}

void Guy_update(Guy *g) {
    if (g->throw_frames > 0) {
        --g->throw_frames;
        if (g->throw_frames == 0) {
            SPR_setAnim(g->sprite, 0);
        }
    }
}
