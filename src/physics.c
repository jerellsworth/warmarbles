#include "bh.h"

Physics *_all_physics[PHYSICS_MAX_OBJECTS];
Physics **ALL_PHYSICS = _all_physics;

Physics *Physics_init(void) {
    u8 reg_idx = 0;
    for (; reg_idx < PHYSICS_MAX_OBJECTS; ++reg_idx) {
        if (!ALL_PHYSICS[reg_idx]) {
            goto found_idx;
        }
    }
    return NULL;
    found_idx:
    Physics *p = st_calloc(1, sizeof(Physics));
    p->reg_idx = reg_idx;
    ALL_PHYSICS[reg_idx] = p;
    return p;
}

void Physics_del(Physics *p) {
    if (p->sprite) {
        SPR_releaseSprite(p->sprite);
    }
    ALL_PHYSICS[p->reg_idx] = NULL;
    free(p);
}

void Physics_update(Physics *p) {
    if (!(p->dx || p->dy)) return;
    p->x += p->dx;
    p->y += p->dy;
    if (p->sprite) {
        SPR_setPosition(
            p->sprite,
            fix16ToRoundedInt(p->x - p->sprite_offset_x),
            fix16ToRoundedInt(p->y - p->sprite_offset_y)
        );
    }
}

void Physics_update_all(void) {
    // TODO collisions
    for (u8 i = 0; i < PHYSICS_MAX_OBJECTS; ++i) {
        Physics *p = ALL_PHYSICS[i];
        if (!p) continue;
        Physics_update(p);
    }
}

Physics *Physics_init_marble(fix16 x, fix16 y) {
    Physics *p = Physics_init();
    if (!p) return NULL;
    p->x = x;
    p->y = y;
    p->sprite_offset_x = FIX16(8);
    p->sprite_offset_y = FIX16(8);
    p->sprite = SPR_addSprite(
        &SPR_MARBLE,
        fix16ToRoundedInt(x - p->sprite_offset_x),
        fix16ToRoundedInt(y - p->sprite_offset_y),
        TILE_ATTR(PAL1, TRUE, FALSE, FALSE) 
        );
    return p;
}
