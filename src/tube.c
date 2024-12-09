#include "bh.h"

Tube *Tube_init(fix16 x, bool reversed) {
    // TODO
    return NULL;
}

void Tube_del(Tube *t) {
    SPR_release(t->sprite);
    free(t);
}
