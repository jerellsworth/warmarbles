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
    VDP_drawImage(BG_B, &IMG_TITLE, 0, 0);
    XGM_startPlay(XGM_theme);
    PAL_setPalette(PAL1, PAL_MARBLE.data, DMA);
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
    title(&players);
    while (TRUE) {
        Game *g = Game_init(players);
        Game_run(g);
        Game_del(g);
    }
	return 0;
}
