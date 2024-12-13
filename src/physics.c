#include "bh.h"

Physics *_all_physics[PHYSICS_MAX_OBJECTS];
Physics **ALL_PHYSICS = _all_physics;

fix32 _THRESH[][2] = {{0, 0},{0, 0}};

FORCE_INLINE u8 _type_to_thresh_idx(Physics *p) {
    switch (p->type) {
        case PHYSICS_T_TARGET:
            return 1;
        default:
            return 0;
    }
}

FORCE_INLINE fix32 _thresh(Physics *p1, Physics *p2) {
    fix32 ret = _THRESH[_type_to_thresh_idx(p1)][_type_to_thresh_idx(p2)];
    if (ret == 0) {
        fix16 r_plus_r = p1->r + p2->r;
        ret = fix16MulTo32(r_plus_r, r_plus_r);
        _THRESH[_type_to_thresh_idx(p1)][_type_to_thresh_idx(p2)] = ret;
    }
    return ret;
}

Physics *Physics_init(Game *g) {
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
    p->game = g;
    ALL_PHYSICS[reg_idx] = p;
    p->has_collision = TRUE;
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
    
    fix16 r_plus_r = p1->r + p2->r;
    fix32 thresh = fix16MulTo32(r_plus_r, r_plus_r);
    //fix32 thresh = _thresh(p1, p2);

    if (dist <= thresh) {
        // https://gamedev.stackexchange.com/a/7901
        fix16 norm_x, norm_y;
        normalize(dx, dy, FIX16(1), &norm_x, &norm_y);
        fix16 diff_dx = p1->dx - p2->dx;
        fix16 diff_dy = p1->dy - p2->dy;
        fix16 dot = fix16Mul(norm_x, diff_dx) + fix16Mul(norm_y, diff_dy);

        //if (dot <= 0) return FALSE;

        fix16 imp_str = fix16Mul(
            //dot + (dot >> 1),
            dot,
            (p1->inv_m + p2->inv_m)
        );
        fix16 imp_x = fix16Mul(imp_str, norm_x);
        fix16 imp_y = fix16Mul(imp_str, norm_y);

        p1->dx -= fix16Mul(imp_x, p1->inv_m);
        p1->dy -= fix16Mul(imp_y, p1->inv_m);
        p2->dx += fix16Mul(imp_x, p2->inv_m);
        p2->dy += fix16Mul(imp_y, p2->inv_m);

        return TRUE;
    }
    return FALSE;
}

void _apply_drag(Physics *p, u8 bitshift) {
    fix16 drag = DRAG << bitshift;
    if (!(p->frames_alive & 7)) {
        if (p->dx > drag) p->dx -= drag;
        if (p->dx < -drag) p->dx += drag;
        if (p->dy > drag) p->dy -= drag;
        if (p->dy < -drag) p->dy += drag;
    }

    if (abs(p->dx) < drag) p->dx = 0;
    if (abs(p->dy) < drag) p->dy = 0;
}

void Physics_update(Physics *p) {
    ++p->frames_alive;

    if (!(p->dx || p->dy)) return;

    _apply_drag(p, 0);
    if (p->dx > PHYSICS_MAX_VELOCITY_VECTOR) p->dx = PHYSICS_MAX_VELOCITY_VECTOR;
    if (p->dx < -PHYSICS_MAX_VELOCITY_VECTOR) p->dx = -PHYSICS_MAX_VELOCITY_VECTOR;
    if (p->dy > PHYSICS_MAX_VELOCITY_VECTOR) p->dy = PHYSICS_MAX_VELOCITY_VECTOR;
    if (p->dy < -PHYSICS_MAX_VELOCITY_VECTOR) p->dy = -PHYSICS_MAX_VELOCITY_VECTOR;

    if (p->y - p->r <= 0) {
        p->y = p->r;
        p->dy = -p->dy;
    } else if (p->y + p->r >= BOARD_HEIGHT) {
        p->y = BOARD_HEIGHT - p->r;
        p->dy = -p->dy;
    }

    /*
    // dbg
    if (p->x >= FIX16(24) + BOARD_WIDTH && p->type == PHYSICS_T_MARBLE) {
        p->x = FIX16(23) + BOARD_WIDTH;
        p->dx = -p->dx;
    }
    */

    if (p->has_collision) {
        if (p->x - p->r <= 0) {
            // you're in the tray. No more horizontal movement allowed
            p->x = p->r;
            p->dx = 0;
            p->in_tray = TRUE;
            _apply_drag(p, 6);
        } else if (p->x >= FIX16(16) && p->x < FIX16(24)) {
            // on the shelf. shove into left tray
            if (p->type == PHYSICS_T_TARGET) Game_score(p->game, 1);
            p->dx = -FIX16(3);
            p->dy = FIX16(3);
        } else if (p->x >= FIX16(24) + BOARD_WIDTH && p->x < FIX16(32) + BOARD_WIDTH) {
            // on the shelf. shove into right tray
            if (p->type == PHYSICS_T_TARGET) Game_score(p->game, 0);
            p->dx = FIX16(3);
            p->dy = FIX16(3);
        } else if (p->x >= FIX16(320)) {
            // you're in the tray. No more horizontal movement allowed
            p->in_tray = TRUE;
            p->x = FIX16(320) - p->r;
            p->dx = 0;
            _apply_drag(p, 6);
        }
    } else {
        if (p->x >= FIX16(24) && p->x <= FIX16(24) + BOARD_WIDTH) {
            p->has_collision = TRUE;
        }
    }
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
        if (!pi->has_collision) continue;
        if (pi->in_tray) continue;
        for (u8 j = i + 1; j < PHYSICS_MAX_OBJECTS; ++j) {
            Physics *pj = ALL_PHYSICS[j];
            if (!pj) continue;
            if (!pj->has_collision) continue;
            if (pj->in_tray) continue;
            Physics_check_collision(pi, pj);
        }
    }
    for (u8 i = 0; i < PHYSICS_MAX_OBJECTS; ++i) {
        Physics *p = ALL_PHYSICS[i];
        if (!p) continue;
        Physics_update(p);
    }
}

Physics *Physics_init_marble(fix16 x, fix16 y, Game *g) {
    Physics *p = Physics_init(g);
    if (!p) return NULL;

    p->r = FIX16(8);
    p->m = FIX16(1);
    p->inv_m = FIX16(1);
    p->type = PHYSICS_T_MARBLE;

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

Physics *Physics_init_target(fix16 x, fix16 y, Game *g) {
    Physics *p = Physics_init(g);
    if (!p) return NULL;

    p->r = FIX16(16);
    p->m = FIX16(16);
    p->inv_m = FIX16(0.0625);

    p->x = x;
    p->y = y;
    p->sprite_offset_x = FIX16(16);
    p->sprite_offset_y = FIX16(16);
    p->sprite = SPR_addSprite(
        &SPR_TARGET,
        fix16ToRoundedInt(x - p->sprite_offset_x),
        fix16ToRoundedInt(y - p->sprite_offset_y),
        TILE_ATTR(PAL1, TRUE, FALSE, FALSE) 
        );
    p->type = PHYSICS_T_TARGET;
    return p;
}

Physics *Physics_find_nearby(fix16 x, fix16 y, PhysicsType t) {
    for (u8 i = 0; i < PHYSICS_MAX_OBJECTS; ++i) {
        Physics *pi = ALL_PHYSICS[i];
        if (!pi) continue;
        if (pi->type != t) continue;
        fix16 dx = x - pi->x;        
        fix16 dy = y - pi->y;        
        fix32 dist = fix16MulTo32(dx, dx) + fix16MulTo32(dy, dy);
        if (dist <= PHYSICS_NEARBY_THRESH) {
            return pi;
        }
    }
    return NULL;
}
