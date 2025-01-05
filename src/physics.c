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

    if ((!bumper->broken) && (bumper->collided_frames > PHYSICS_FRAMES_TO_BUMPER_BREAK)) {
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

    /*
    if (other->type == PHYSICS_T_MARBLE) {
        *imp_str = *imp_str + (*imp_str >> 1);
    } else {
        *imp_str = *imp_str << 1;
    }
    */
    bumper->anim = 1;    
    bumper->anim_frame = 0;
    _bumper_draw(bumper);
    return FALSE;

}

bool _special_phys_handle(Physics *p) {
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
        if (p->init_frames > 0) {
            --p->init_frames;
            if (p->init_frames == 0) {
                SPR_setAnim(p->sprite, 0);
                p->has_collision  = TRUE;

                u8 left_marbles = 0;
                u8 right_marbles = 0;
                u8 side;
                // TODO HERE
                for (u8 i = 0; i < PHYSICS_MAX_OBJECTS; ++i) {
                    Physics *pi = ALL_PHYSICS[i];
                    if (!pi) continue;
                    if (pi->type != PHYSICS_T_MARBLE) continue;
                    if (pi->x < FIX16(160)) {
                        ++left_marbles;
                    } else if (pi->x > FIX16(160)) {
                        ++right_marbles;
                    }
                }
                if (left_marbles > right_marbles) {
                    side = 1;
                } else if (left_marbles < right_marbles) {
                    side = 0;
                } else {
                    side = random_with_max(1);
                }
                u16 theta;
                if (side == 0) {
                    theta = 320 + random_with_max(384);
                } else {
                    theta = theta_add(832, random_with_max(384)); 
                }

                fix16 dx = cosFix16(theta) << 2;
                fix16 dy = sinFix16(theta) << 2;
                p->dx = dx;
                p->dy = dy;
                return FALSE;
            }
        }
        if (p->anim_frames > 0) {
            --p->anim_frames;
            if (p->anim_frames == 0) {
                SPR_setAnim(p->sprite, 0);
            }
        }
        if ((!p->in_tray) && (!p->held) && p->ttl == 0 && abs(p->dx) <= PHYSICS_SLOW_THRESH) {
            ++p->slow_frames;
            if (p->slow_frames >= 120) {
                p->ttl = 3 * 3;
                SPR_setAnim(p->sprite, 3);
            }
        } else {
            p->slow_frames = 0;
        }
    }
    return FALSE;
}

void _collision_sfx(Physics *p1, Physics *p2) {
    Physics *pi, *pj;
    if (p1->type <= p2->type) {
        pi = p1;
        pj = p2;
    } else {
        pi = p2;
        pj = p1;
    }
    u8 sample = 0;
    switch (pi->type) {
        case PHYSICS_T_MARBLE:
            switch (pj->type) {
                case PHYSICS_T_MARBLE:
                    sample = SND_SAMPLE_COL_MARBLE_MARBLE;   
                    break;
                case PHYSICS_T_TARGET:
                    sample = SND_SAMPLE_COL_MARBLE_TARGET;    
                    break;
                case PHYSICS_T_BUMPER:
                    if (pj->broken) {
                        /*
                        if (pj->anim_frames == 0) {
                            sample = SND_SAMPLE_COL_MARBLE_ACCEL;
                        }
                        */
                    } else {
                        sample = SND_SAMPLE_COL_MARBLE_BUMPER;    
                    }
                    break;
            }
            break;
        case PHYSICS_T_TARGET:
        case PHYSICS_T_BUMPER:
        default:
            break;
    }
    if (sample > 0) SFX_incidental(p1->game->sfx, sample);
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

        fix16 current_dx = p1->x - p2->x;
        fix16 current_dy = p1->y - p2->y;
        fix32 current_dist = fix16MulTo32(current_dx, current_dx) + fix16MulTo32(current_dy, current_dy);
        if (thresh - current_dist >= FIX32(8)) {
            // If objects are already stuck together, delete the marbles
            bool rm_marble = FALSE;
            if (p1->type == PHYSICS_T_MARBLE && p2->type == PHYSICS_T_TARGET && p1->ttl == 0) {
                p1->ttl = 3 * 3;
                SPR_setAnim(p1->sprite, 3);
                rm_marble = TRUE;
            }
            if (p2->type == PHYSICS_T_MARBLE && p1->type == PHYSICS_T_TARGET && p2->ttl == 0) {
                p2->ttl = 3 * 3;
                SPR_setAnim(p2->sprite, 3);
                rm_marble = TRUE;
            }
            if (rm_marble) return TRUE;
        }

        if (p1->collided_frames < 65535) ++p1->collided_frames;
        if (p2->collided_frames < 65535) ++p2->collided_frames;


        //if (dot <= 0) return FALSE;


        _collision_sfx(p1, p2);

        /* TODO if one of them is a bumper the math is totally different
         * draw a line tangent to the circle at a collision point
         * draw a line perpendicular to the first line going through the
           same collision point
         * calculate angle between vector of marble and 2nd line
         * rerflect the angle across the second line
         * marble exits with the second angle, same velocity * dampening
         * coefficient
         */
        if (_special_collision_handle(p1, p2)) {
            return TRUE;
        }

        Physics *stationary = NULL;
        Physics *mobile = NULL;
        if (p1->stationary) {
            stationary = p1;
            mobile = p2;
        } else if (p2->stationary) {
            stationary = p2;
            mobile = p1;
        }

        if (stationary) {
            dx = mobile->x - stationary->x;
            dy = mobile->y - stationary->y;
            fix16 norm_dx, norm_dy, norm_velx, norm_vely;
            normalize(dx, dy, FIX16(1), &norm_dx, &norm_dy);
            s16 theta_norm = arcsin_fix_quadrant(norm_dx, norm_dy);
            normalize(mobile->dx, mobile->dy, FIX16(1), &norm_velx, &norm_vely);
            s16 theta_vel_in = arcsin_fix_quadrant(norm_velx, norm_vely);
            s16 theta_vel_out = theta_add(
                theta_norm,
                -theta_add(theta_vel_in, -theta_norm)
            );
            fix16 speed = fix16Sqrt(
                fix16Mul(
                    mobile->dx,
                    mobile->dx
                )
                + fix16Mul(
                    mobile->dy,
                    mobile->dy
                )
            );

            mobile->dx = fix16Mul(cosFix16(theta_vel_out), speed);
            mobile->dy = fix16Mul(sinFix16(theta_vel_out), speed);

        } else {
            // https://gamedev.stackexchange.com/a/7901
            fix16 norm_x, norm_y;
            normalize(dx, dy, FIX16(1), &norm_x, &norm_y);
            fix16 diff_dx = p1->dx - p2->dx;
            fix16 diff_dy = p1->dy - p2->dy;

            fix16 dot = fix16Mul(norm_x, diff_dx) + fix16Mul(norm_y, diff_dy);

            fix16 imp_str = fix16Mul(
                dot,
                (p1->inv_m + p2->inv_m)
            );

            fix16 imp_x = fix16Mul(imp_str, norm_x);
            fix16 imp_y = fix16Mul(imp_str, norm_y);
            
            p1->dx -= fix16Mul(imp_x, p1->inv_m);
            p1->dy -= fix16Mul(imp_y, p1->inv_m);
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

    if (p->ttl > 0) {
        --p->ttl;
        if (p->ttl == 0) {
            Physics_del(p);
            return;
        }
    }

    if (_special_phys_handle(p)) return;

    if (p->type == PHYSICS_T_MARBLE && !(p->frames_alive & 15)) {

        fix16 x, y;
        y = p->y;
        x = p->x;
        if (y >= FIX16(32 * 3 + 16)) {
            // bottom half of board
            if(y < FIX16(32 * 3 + 16 + 16)) {
                // in the part with no bumpers
                goto post_update;
            }
            // else, we need to correct to the right row
            y -= FIX16(16);
        }
        if (x >= FIX16(32 * 2 + 16 + 24)) {
            if (x < FIX16(32 * 2 + 16 + 24 + 16)) {
                goto post_update;
            }
            x -= FIX16(16);
        }
        s16 r = fix16ToInt(y) >> 5;
        s16 c = (fix16ToInt(x) - 24) >> 5;
        if (r < 0 || r >= BOARD_HEIGHT_TILES >> 2 || c < 0 || c >= BOARD_WIDTH_TILES >> 2) return;
        ++p->game->board->traffic[r][c];

        /*
        // TODO dbg
        char buf[3];
        sprintf(buf, "%02d", p->game->board->traffic[r][c]);
        VDP_drawText(
            buf,
            (c << 2) + 3 + (c > 3 ? 2 : 0), 
            (r << 2) + (r > 2 ? 2 : 0)
        );
        */
    }
    post_update:

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
    p->init_frames = 3 * 4;
    p->has_collision = FALSE;
    SPR_setAnim(p->sprite, 2);
    return p;
}

Physics *Physics_init_target(fix16 x, fix16 y, Game *g) {
    Physics *p = Physics_init(g);
    if (!p) return NULL;

    p->r = FIX16(16);
    p->m = FIX16(8);
    p->inv_m = FIX16(0.125);

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

u8 Physics_count_type(PhysicsType t) {
    u8 ret = 0;
    for (u8 i = 0; i < PHYSICS_MAX_OBJECTS; ++i) {
        Physics *pi = ALL_PHYSICS[i];
        if (pi && pi->type == t) ++ret;
    }
    return ret;
}

void Physics_del_type(PhysicsType t) {
    for (u8 i = 0; i < PHYSICS_MAX_OBJECTS; ++i) {
        Physics *pi = ALL_PHYSICS[i];
        if (pi && pi->type == t) {
            Physics_del(pi); 
        }
    }
}
