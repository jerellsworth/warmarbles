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

bool Physics_check_collision(Physics *p1, Physics *p2) {
    fix16 p1_next_x = p1->x + p1->dx;
    fix16 p1_next_y = p1->y + p1->dy;
    fix16 p2_next_x = p2->x + p2->dx;
    fix16 p2_next_y = p2->y + p2->dy;
    fix16 dx = p1_next_x - p2_next_x;
    fix16 dy = p1_next_y - p2_next_y;
    fix32 dist = fix16MulTo32(dx, dx) + fix16MulTo32(dy, dy);
    
    // TODO just hardcode the thresh depending on what things are bumping into eachother
    fix16 r_plus_r = p1->r + p2->r;
    fix32 thresh = fix16MulTo32(r_plus_r, r_plus_r);

    if (dist <= thresh) {
        fix16 total_mass = p1->m + p2->m;
        fix16 diff_mass = p1->m - p2->m;
        fix16 rat_mass = fix16Div(diff_mass, total_mass);
        p1->dx = fix16Mul(rat_mass, p1->dx) + fix16Mul(fix16Div(p2->m << 1, total_mass), p2->dx);
        p1->dy = fix16Mul(rat_mass, p1->dy) + fix16Mul(fix16Div(p2->m << 1, total_mass), p2->dy);
        p2->dx = fix16Mul(rat_mass, p2->dx) + fix16Mul(fix16Div(p1->m << 1, total_mass), p1->dx);
        p2->dy = fix16Mul(rat_mass, p2->dy) + fix16Mul(fix16Div(p1->m << 1, total_mass), p1->dy);
        return TRUE;
    }
    return FALSE;
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
    // TODO divide into sectors and only calc collisions between objects in the same sector
    for (u8 i = 0; i < PHYSICS_MAX_OBJECTS; ++i) {
        Physics *pi = ALL_PHYSICS[i];
        if (!pi) continue;
        for (u8 j = i + 1; j < PHYSICS_MAX_OBJECTS; ++j) {
            Physics *pj = ALL_PHYSICS[j];
            if (!pj) continue;
            Physics_check_collision(pi, pj);
        }
    }
    for (u8 i = 0; i < PHYSICS_MAX_OBJECTS; ++i) {
        Physics *p = ALL_PHYSICS[i];
        if (!p) continue;
        Physics_update(p);
    }
}

Physics *Physics_init_marble(fix16 x, fix16 y) {
    Physics *p = Physics_init();
    if (!p) return NULL;

    p->r = FIX16(8);
    p->m = FIX16(1);

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
