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

void Guy_move(Guy *t, fix16 dx, fix16 dy) {
    t->y += dy;
    t->x += dx;
    //if (t->y <= 0) t->y = 0;
    //if (t->y >= FIX16(192)) t->y = FIX16(192);
    SPR_setPosition(
        t->sprite,
        fix16ToRoundedInt(t->x),
        fix16ToRoundedInt(t->y)
    );
}
