#ifndef WM_SFX_H
#define WM_SFX_H

#include "wm.h"

#define WM_SND_SAMPLE_CONFIRM 64
#define WM_SND_SAMPLE_THROW 65
#define WM_SND_SAMPLE_SCORE 66
#define WM_SND_SAMPLE_COL_MARBLE_MARBLE 67
#define WM_SND_SAMPLE_COL_MARBLE_BUMPER 68
#define WM_SND_SAMPLE_COL_MARBLE_TARGET 69
#define WM_SND_SAMPLE_COL_MARBLE_ACCEL 70
#define WM_SND_SAMPLE_MARBLE_ENTER 71

struct WM_SFX_s {
    u8 next_channel;
};

WM_SFX *WM_SFX_init(void);
void WM_SFX_del(WM_SFX *s);
void WM_SFX_incidental(WM_SFX *s, u8 sample);

#endif
