#include "bh.h"



SFX *SFX_init(void) {
    XGM_setPCM(SND_SAMPLE_CONFIRM, WAV_CONFIRM, sizeof(WAV_CONFIRM));
    XGM_setPCM(SND_SAMPLE_THROW, WAV_THROW, sizeof(WAV_THROW));
    XGM_setPCM(SND_SAMPLE_SCORE, WAV_SCORE, sizeof(WAV_SCORE));
    XGM_setPCM(SND_SAMPLE_COL_MARBLE_MARBLE, WAV_COL_MARBLE_MARBLE, sizeof(WAV_COL_MARBLE_MARBLE));
    XGM_setPCM(SND_SAMPLE_COL_MARBLE_BUMPER, WAV_COL_MARBLE_BUMPER, sizeof(WAV_COL_MARBLE_BUMPER));
    XGM_setPCM(SND_SAMPLE_COL_MARBLE_TARGET, WAV_COL_MARBLE_TARGET, sizeof(WAV_COL_MARBLE_TARGET));
    XGM_setPCM(SND_SAMPLE_COL_MARBLE_ACCEL, WAV_COL_MARBLE_ACCEL, sizeof(WAV_COL_MARBLE_ACCEL));

    SFX *s = st_calloc(1, sizeof(SFX));
    s->next_channel = SOUND_PCM_CH1;
    return s;
}

void SFX_del(SFX *s) {
    free(s);
}

void SFX_incidental(SFX *s, u8 sample) {
    XGM_startPlayPCM(sample, 14, s->next_channel);
    if (s->next_channel == SOUND_PCM_CH4) {
        s->next_channel = SOUND_PCM_CH1;
        return;
    }
    ++s->next_channel;
}
