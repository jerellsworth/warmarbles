#include "shared.h"

int main(bool hard_reset) {
    if (!hard_reset) SYS_hardReset();
    while (TRUE) {
        WM_Game *g = WM_Game_init(1);
        WM_Game_run(g);
        WM_Game_del(g);
    }
	return 0;
}
