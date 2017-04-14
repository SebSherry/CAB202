#include "game.h"
#include "cab202_timers.h"

int main() {
	setup_screen();
	play_game();
	cleanup_screen();
	return 0;
}