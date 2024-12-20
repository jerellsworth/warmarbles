#include "bh.h"

Physics *_all_physics[PHYSICS_MAX_OBJECTS];
Physics **ALL_PHYSICS = _all_physics;

fix32 _THRESH[][3] = {{0, 0, 0},{0, 0, 0},{0, 0, 0}};

u16 _BUMPER_TILE;

FORCE_INLINE u8 _type_to_thresh_idx(Physics *p) {
    switch (p->type) {
        case PHYSICS_T_TARGET:
            return 1;
        case PHYSICS_T_BUMPER:
            return 2;
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

void Physics_engine_init(Game *g) {
    _BUMPER_TILE = g->next_tile_idx;
    VDP_loadTileSet(&TLS_BUMPER, _BUMPER_TILE, DMA);
    g->next_tile_idx += TLS_BUMPER.numTile;
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
    if (p->tile_h > 0 && p->tile_w > 0) {
        VDP_clearTileMapRect(
            BG_A,
            p->tile_x,
            p->tile_y,
            p->tile_w,
            p->tile_h
        );
    }
    ALL_PHYSICS[p->reg_idx] = NULL;
    free(p);
}

void _bumper_draw(Physics *p) {
    u16 base_tile = p->tile_idx + p->anim * 144 + p->anim_frame * 4;
    for (u8 r = 0; r < p->tile_h; ++r) {
        VDP_fillTileMapRectInc(
            BG_A,
            TILE_ATTR_FULL(p->pal, TRUE, FALSE, FALSE, base_tile),
            p->tile_x,
            p->tile_y + r,
            p->tile_w,
            1
            );
        base_tile += 36; // TODO don't hardcode this
    }
}

bool _special_collision_handle(Physics *p1, Physics *p2) {
    Physics *bumper = NULL;
    Physics *other = NULL;
    // It should only be one or the other unless something really bad happened
    if (p1->type == PHYSICS_T_BUMPER) {
        bumper = p1;
        other = p2;
    } else if (p2->type == PHYSICS_T_BUMPER) {
        bumper = p2;
        other = p1;
    }

    if (!bumper) return FALSE;

    if ((!bumper->broken) && (bumper->collided_frames > 200)) {
        bumper->broken = TRUE;
        bumper->pal = PAL1;
        _bumper_draw(bumper);
    }
    if (bumper->broken) {
        if (other->type == PHYSICS_T_MARBLE && other->anim_frames == 0) {
            SPR_setAnim(other->sprite, 1);
            other->anim_frames = 9;
        }
        fix16 other_dx_delta = other->dx >> 5;
        fix16 other_dy_delta = other->dy >> 5;
        other->dx += other_dx_delta;
        other->dx += other_dy_delta;
        return TRUE;
    }

    bumper->anim = 1;    
    bumper->anim_frame = 0;
    _bumper_draw(bumper);
    return FALSE;

}

void _special_anim_handle(Physics *p) {
    // TODO there's a lot of hardcoded garbage here. Refactor
    if (p->type == PHYSICS_T_BUMPER) {
        if (!(p->frames_alive & 7)) {
            if (p->anim == 0) {
                if (p->anim_frame == 2) {
                    p->anim_frame = 0;
                } else {
                    ++p->anim_frame;
                }
            } else {
                if (p->anim_frame == 8) {
                    p->anim_frame = 0;
                    p->anim = 0;
                } else {
                    ++p->anim_frame;
                }
            }
            _bumper_draw(p);
        }
    } else if (p->type == PHYSICS_T_MARBLE) {
        if (p->anim_frames > 0) {
            --p->anim_frames;
            if (p->anim_frames == 0) {
                SPR_setAnim(p->sprite, 0);
            }
        }
    }
}

bool Physics_check_collision(Physics *p1, Physics *p2) {
    if (p1->stationary && p2->stationary) return FALSE;
    fix16 p1_next_x = p1->x + p1->dx;
    fix16 p2_next_x = p2->x + p2->dx;
    fix16 dx = p1_next_x - p2_next_x;
    if (abs(dx) > FIX16(33)) return FALSE;
    fix16 p1_next_y = p1->y + p1->dy;
    fix16 p2_next_y = p2->y + p2->dy;
    fix16 dy = p1_next_y - p2_next_y;
    if (abs(dy) > FIX16(33)) return FALSE;
    fix32 dist = fix16MulTo32(dx, dx) + fix16MulTo32(dy, dy);
    
    //fix16 r_plus_r = p1->r + p2->r;
    //fix32 thresh = fix16MulTo32(r_plus_r, r_plus_r);
    fix32 thresh = _thresh(p1, p2);

    if (dist <= thresh) {

        if (p1->collided_frames < 65535) ++p1->collided_frames;
        if (p2->collided_frames < 65535) ++p2->collided_frames;

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

        if (_special_collision_handle(p1, p2)) {
            return TRUE;
        }
        
        if (!p1->stationary) {
            p1->dx -= fix16Mul(imp_x, p1->inv_m);
            p1->dy -= fix16Mul(imp_y, p1->inv_m);
        }
        if (!p2->stationary) {
            p2->dx += fix16Mul(imp_x, p2->inv_m);
            p2->dy += fix16Mul(imp_y, p2->inv_m);
        }


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

void _handle_tray(Physics *p, u8 tray_no) {
    if (!p->in_tray) {
        if (tray_no == 0) {
            p->x = p->r;
        } else {
            p->x = FIX16(320) - p->r;
        }
        p->dx = 0;
        p->in_tray = TRUE;
        p->tray_no = tray_no;
        Game_change_tray_marbles(p->game, tray_no, 1);
    }
    _apply_drag(p, 6);
}

void Physics_update(Physics *p) {
    ++p->frames_alive;

    _special_anim_handle(p);

    if (p->type == PHYSICS_T_MARBLE && !(p->frames_alive & 15)) {

        s16 r = fix16ToInt(p->y) >> 5;
        s16 c = (fix16ToInt(p->x) - 24) >> 5;
        if (r < 0 || r >= BOARD_HEIGHT_TILES >> 2 || c < 0 || c >= BOARD_WIDTH_TILES >> 2) return;
        ++p->game->board->traffic[r][c];

        /*
        // TODO dbg
        char buf[3];
        sprintf(buf, "%02d", p->game->board->traffic[r][c]);
        VDP_drawText(buf, (c << 2) + 3, r << 2);
        */
    }

    if (p->stationary) return;
    if (!(p->dx || p->dy)) return;

    _apply_drag(p, 0);
    if (p->type == PHYSICS_T_TARGET) {
        if (p->dx > PHYSICS_MAX_VELOCITY_VECTOR_TARGET) p->dx = PHYSICS_MAX_VELOCITY_VECTOR_TARGET;
        if (p->dx < -PHYSICS_MAX_VELOCITY_VECTOR_TARGET) p->dx = -PHYSICS_MAX_VELOCITY_VECTOR_TARGET;
        if (p->dy > PHYSICS_MAX_VELOCITY_VECTOR_TARGET) p->dy = PHYSICS_MAX_VELOCITY_VECTOR_TARGET;
        if (p->dy < -PHYSICS_MAX_VELOCITY_VECTOR_TARGET) p->dy = -PHYSICS_MAX_VELOCITY_VECTOR_TARGET;
    } else {
        if (p->dx > PHYSICS_MAX_VELOCITY_VECTOR) p->dx = PHYSICS_MAX_VELOCITY_VECTOR;
        if (p->dx < -PHYSICS_MAX_VELOCITY_VECTOR) p->dx = -PHYSICS_MAX_VELOCITY_VECTOR;
        if (p->dy > PHYSICS_MAX_VELOCITY_VECTOR) p->dy = PHYSICS_MAX_VELOCITY_VECTOR;
        if (p->dy < -PHYSICS_MAX_VELOCITY_VECTOR) p->dy = -PHYSICS_MAX_VELOCITY_VECTOR;
    }

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
            _handle_tray(p, 0);
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
            _handle_tray(p, 1);
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
        TILE_ATTR(PAL3, TRUE, FALSE, FALSE) 
        );
    p->type = PHYSICS_T_TARGET;
    return p;
}

Physics *Physics_init_bumper(fix16 x, fix16 y, Game *g) {
    Physics *p = Physics_init(g);
    if (!p) return NULL;

    p->r = FIX16(16);
    p->m = FIX16(4);
    p->inv_m = FIX16(0.25);
    p->stationary = TRUE;

    p->x = x;
    p->y = y;
    p->sprite_offset_x = FIX16(16);
    p->sprite_offset_y = FIX16(16);

    p->tile_x = fix16ToInt(x - p->sprite_offset_x) >> 3;
    p->tile_y = fix16ToInt(y - p->sprite_offset_y) >> 3;
    p->tile_w = 4;
    p->tile_h = 4;
    p->tile_idx = _BUMPER_TILE;
    p->pal = PAL3;
    _bumper_draw(p);

    p->type = PHYSICS_T_BUMPER;
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
