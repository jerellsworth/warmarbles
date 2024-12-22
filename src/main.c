#include "bh.h"

void title(void) {
    VDP_drawImage(BG_B, &IMG_TITLE, 0, 0);
    XGM_startPlay(XGM_theme);
    while (TRUE) {
        random();
        if (JOY_readJoypad(JOY_ALL) & BUTTON_START) break;
        SYS_doVBlankProcess();
    }
}

int main(bool hard_reset) {
    if (!hard_reset) SYS_hardReset();
    title();
    while (TRUE) {
        Game *g = Game_init();
        Game_run(g);
        Game_del(g);
    }
	return 0;
}
