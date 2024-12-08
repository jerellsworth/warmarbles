#include "bh.h"

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
