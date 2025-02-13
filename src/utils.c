#include "shared.h"

void *st_calloc(u16 nitems, u16 size) {
    u16 bytes = nitems * size;
    void *p = malloc(bytes);
    if (!p) {
        VDP_drawTextBG(BG_A, "NULL POINTER RETURNED FROM MALLOC", 1, 1);
        while (TRUE) {
            SYS_doVBlankProcess();
            VDP_waitVSync();
        }
    }
    memset(p, 0, bytes);
    return p;
}

u16 random_with_max(u16 max) {
    u16 mask;
    if (max <= 1) {
        mask = 1;
    } else if (max <= 3) {
        mask = 3;
    } else if (max <= 7) {
        mask = 7;
    } else if (max <= 15) {
        mask = 15;
    } else if (max <= 31) {
        mask = 31;
    } else if (max <= 63) {
        mask = 63;
    } else if (max <= 127) {
        mask = 127;
    } else if (max <= 255) {
        mask = 255;
    } else if (max <= 511) {
        mask = 511;
    } else if (max <= 1023) {
        mask = 1023;
    } else if (max <= 2047) {
        mask = 2047;
    } else if (max <= 4095) {
        mask = 4095;
    } else if (max <= 8191) {
        mask = 8191;
    } else if (max <= 16383) {
        mask = 16383;
    } else if (max <= 32767) {
        mask = 32767;
    } else {
        mask = 65535;
    }
    u16 r;
    while (TRUE) {
        r = random() & mask;
        if (r <= max) return r;
    }
}

u16 theta_add(u16 theta, s16 dtheta) {
    s16 sum = theta + dtheta;
    if (sum >= 1024) return sum - 1024;
    if (sum < 0) return sum + 1024;
    return sum;
}

char *heap_str(char *str) {
    char *new = st_calloc(sizeof(char), strlen(str) + 1);
    strcpy(new, str);
    return new;
}

void agg_and_fade_in_all(
    const u16 *pal_0_data,
    const u16 *pal_1_data,
    const u16 *pal_2_data,
    const u16 *pal_3_data,
    u16 numFrame
) {
    u16 *pal_full = st_calloc(64, sizeof(u16));
    if (pal_0_data) memcpy(pal_full, pal_0_data, 16 * sizeof(u16));
    if (pal_1_data) memcpy(pal_full + 16, pal_1_data, 16 * sizeof(u16));
    if (pal_2_data) memcpy(pal_full + 32, pal_2_data, 16 * sizeof(u16));
    if (pal_3_data) memcpy(pal_full + 48, pal_3_data, 16 * sizeof(u16));
    PAL_fadeInAll(pal_full, numFrame, FALSE);
    free(pal_full);
}

FORCE_INLINE fix32 fix16MulTo32(fix16 val1, fix16 val2)
{
    return muls(val1, val2) >> FIX16_FRAC_BITS;
}

/* 
 * Numerically stable vector normalization in SGDK.
 * 
 * Licensed under Apache 2 
 */
fix16 exp2(fix16 x) {
    /* 
     * Estimate fractional exponent by Taylor series.
     * This is presumably the slow part, but it's only 4
     * multiplications so could be worse.
     */
    fix16 ln2x = fix16Mul(LN2, x);
    fix16 ln2x_2 = fix16Mul(ln2x, ln2x);
    fix16 ln2x_3 = fix16Mul(ln2x_2, ln2x);
    return FIX16(1) + \
         ln2x + \
         (ln2x_2 >> 1) + \
         fix16Mul(ln2x_3, FIX16(0.17));
}

fix16 adaptiveFix32Log2(fix32 x) {
    // Use appropriate log function depending on size of number
    if (x <= FIX32(511)) {
        return fix16Log2(fix32ToFix16(x));
    }
    return FIX16(getLog2Int(fix32ToRoundedInt(x)));
}

void normalize(fix16 x, fix16 y, fix16 v, fix16 *norm_x, fix16 *norm_y) {
    /*
     * Normalize a vector and multiply by velocity.
     * x, y are coordinates
     * v is velocity.
     * result goes in *norm_x and *norm_y
     *
     * We solve in log2 space first to help with numerical stability.
     * Fix16 really doesn't have a lot of precision, so conventional methods will
     * push normalized vectors towards 0 length.
     *
     * The equation works out to:
     * norm_x = (v * x) / (sqrt(x^2 + y^2))
     * => norm_x = 2^(log2(v) + log2(x) - log2(x^2 + y^2)/2)
     *
     * SGDK provides precalc'd log2 tables, so the only expensive part is
     * exponentiation, which we do with a small Taylor series in exp2.
     */
    if (x == 0 && y == 0) {
        *norm_x = 0;
        *norm_y = 0;
        return;
    } else if (x == 0) {
        *norm_x = 0;
        *norm_y = y >= 0 ? v : -v;
        return;
    } else if (y == 0) {
        *norm_x = x >= 0 ? v : -v;
        *norm_y = 0;
        return;
    }
    // since we're in logspace, we work in the (+,+) quadrant and fix sign later
    bool x_pos = x >= 0;
    bool y_pos = y >= 0;
    if (!x_pos) x = -x;
    if (!y_pos) y = -y;
    /* 
     * We can't guarantee that the sum of two squared fix16s fit in fix16, so
     * we briefly expand to fix32 math. This is more expensive on the 68k,
     * so we get back into fix16 as soon as possible
     */
    fix32 x_rd = fix16ToFix32(x);
    fix32 y_rd = fix16ToFix32(y);
    fix32 dist_sq = fix32Mul(x_rd, x_rd) + fix32Mul(y_rd, y_rd);
    /*
     * Now that we're in logspace, we can go back to fix16 since log2(x^2 + y^2)
     * is a much smaller number than x^2 + y^2
     */
    fix16 dist_exp = adaptiveFix32Log2(dist_sq) >> 1;
    /*
     * Implementing:
     * norm_x = 2^(log2(v) + log2(x) - log2(x^2 + y^2)/2)
     * Note that dist_exp has already been divided by 2 in the previous step
     */
    fix16 log2v = fix16Log2(v);
    fix16 raw_norm_x = exp2(log2v + fix16Log2(x) - dist_exp);
    fix16 raw_norm_y = exp2(log2v + fix16Log2(y) - dist_exp);
    /*
     * Here we fix the sign and clamp to velocity. A normalized vector
     * has length <= 1, so a normalized vector * velocity has a length
     * <= v. Sometimes, due to innacuracy, we end up with length > v,
     * so we just artificially fix it.
     */
    *norm_x = x_pos ? min(raw_norm_x, v) : max(-raw_norm_x, -v);
    *norm_y = y_pos ? min(raw_norm_y, v) : max(-raw_norm_y, -v);
}

s16 arcsin_fix_quadrant(fix16 norm_x, fix16 norm_y) {
    s16 q1_theta = arcsin(abs(norm_y));
    if (norm_x >= 0 && norm_y >= 0) return 1024 + q1_theta;
    if (norm_x >= 0 && norm_y < 0) return -q1_theta;
    if (norm_x < 0 && norm_y >= 0) return 512 - q1_theta;
    return 512 + q1_theta;
}

