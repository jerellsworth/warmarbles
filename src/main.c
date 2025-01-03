#include "bh.h"

int _go_cb(Menu_Item *mi) {
    Menu *m = mi->parent;
    m->completed = TRUE;
    return 0;
}

int _go_to_go_cb(Menu_Item *mi) {
    Menu *m = mi->parent;
    m->cursor = m->last;
    Menu_refresh_cursor(m);
    return 0;
}

void title(u8 *players) {
    XGM_startPlay(XGM_theme);
    VDP_drawImage(BG_B, &IMG_PRODUCTION, 0, 0);
    agg_and_fade_in_all(
        PAL_PRODUCTION.data,
        NULL,
        NULL,
        NULL,
        30);
    for (u16 i = 0; i < 300; ++i) {
        if (JOY_readJoypad(JOY_ALL) & BUTTON_START) break;
        SYS_doVBlankProcess();
    }
    PAL_fadeOutAll(30, FALSE);

    VDP_drawImage(BG_B, &IMG_TITLE, 0, 0);
    agg_and_fade_in_all(
        PAL_TITLE_1.data,
        PAL_MARBLE.data,
        NULL,
        NULL,
        30);

    Menu *m = Menu_new(15, 21);

    Menu_Item *mi_players = Menu_add_item(m, "Players:");
    Menu_Item_add_option(mi_players, "1");
    Menu_Item_add_option(mi_players, "2");
    mi_players->select_cb = &_go_to_go_cb;

    Menu_Item *play = Menu_add_item(m, "Go");
    play->select_cb = &_go_cb;

    Menu_run(m);

    *players = mi_players->option_selected + 1;
    Menu_del(m);
}

int main(bool hard_reset) {
    if (!hard_reset) SYS_hardReset();
    u8 players;
    VDP_loadFont(&TLS_FONT, DMA);
    SFX *sfx = SFX_init();
    title(&players);
    while (TRUE) {
        Game *g = Game_init(players, sfx);
        Game_run(g);
        Game_del(g);
    }
    SFX_del(sfx);
	return 0;
}
