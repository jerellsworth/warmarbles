#include "bh.h"

int main(bool hard_reset) {
    if (!hard_reset) SYS_hardReset();
    while (TRUE) {
        Game *g = Game_init();
        Game_run(g);
        Game_del(g);
    }
	return 0;
}
