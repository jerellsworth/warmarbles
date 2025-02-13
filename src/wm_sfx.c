#include "wm.h"

WM_SFX *WM_SFX_init(void) {
    XGM_setPCM(WM_SND_SAMPLE_CONFIRM, WAV_WM_CONFIRM, sizeof(WAV_WM_CONFIRM));
    XGM_setPCM(WM_SND_SAMPLE_THROW, WAV_WM_THROW, sizeof(WAV_WM_THROW));
    XGM_setPCM(WM_SND_SAMPLE_SCORE, WAV_WM_SCORE, sizeof(WAV_WM_SCORE));
    XGM_setPCM(WM_SND_SAMPLE_COL_MARBLE_MARBLE, WAV_WM_COL_MARBLE_MARBLE, sizeof(WAV_WM_COL_MARBLE_MARBLE));
    XGM_setPCM(WM_SND_SAMPLE_COL_MARBLE_BUMPER, WAV_WM_COL_MARBLE_BUMPER, sizeof(WAV_WM_COL_MARBLE_BUMPER));
    XGM_setPCM(WM_SND_SAMPLE_COL_MARBLE_TARGET, WAV_WM_COL_MARBLE_TARGET, sizeof(WAV_WM_COL_MARBLE_TARGET));
    XGM_setPCM(WM_SND_SAMPLE_COL_MARBLE_ACCEL, WAV_WM_COL_MARBLE_ACCEL, sizeof(WAV_WM_COL_MARBLE_ACCEL));
    XGM_setPCM(WM_SND_SAMPLE_MARBLE_ENTER, WAV_WM_MARBLE_ENTER, sizeof(WAV_WM_MARBLE_ENTER));

    WM_SFX *s = st_calloc(1, sizeof(WM_SFX));
    s->next_channel = SOUND_PCM_CH1;
    return s;
}

void WM_SFX_del(WM_SFX *s) {
    free(s);
}

void WM_SFX_incidental(WM_SFX *s, u8 sample) {
    XGM_startPlayPCM(sample, 14, s->next_channel);
    if (s->next_channel == SOUND_PCM_CH4) {
        s->next_channel = SOUND_PCM_CH1;
        return;
    }
    ++s->next_channel;
}
