#include "bh.h"

Tube *Tube_init(fix16 x, bool reversed) {
    Tube *t = st_calloc(1, sizeof(Tube));
    t->x = x;
    t->y = FIX16(112);
    t->sprite = SPR_addSprite(
        &SPR_TUBE,
        fix16ToRoundedInt(x),
        fix16ToRoundedInt(t->y),
        TILE_ATTR(PAL1, TRUE, FALSE, reversed) 
        );
    return t;
}

void Tube_del(Tube *t) {
    SPR_releaseSprite(t->sprite);
    free(t);
}
