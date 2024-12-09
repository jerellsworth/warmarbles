#include "bh.h"

void *st_calloc(u16 nitems, u16 size);
u16 random_with_max(u16 max);
u16 theta_add(u16 theta, s16 dtheta);
char *heap_str(char *str);
void agg_and_fade_in_all(
    const u16 *pal_0_data,
    const u16 *pal_1_data,
    const u16 *pal_2_data,
    const u16 *pal_3_data,
    u16 numFrame
);
fix32 fix16MulTo32(fix16 val1, fix16 val2);
