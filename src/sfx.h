#ifndef SFX_H
#define SFX_H

#include "bh.h"

#define SND_SAMPLE_CONFIRM 64
#define SND_SAMPLE_THROW 65
#define SND_SAMPLE_SCORE 66
#define SND_SAMPLE_COL_MARBLE_MARBLE 67
#define SND_SAMPLE_COL_MARBLE_BUMPER 68
#define SND_SAMPLE_COL_MARBLE_TARGET 69
#define SND_SAMPLE_COL_MARBLE_ACCEL 70

struct SFX_s {
    u8 next_channel;
};

SFX *SFX_init(void);
void SFX_del(SFX *s);
void SFX_incidental(SFX *s, u8 sample);

#endif
