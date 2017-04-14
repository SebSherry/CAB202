#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "cab202_sprites.h"
#include "cab202_timers.h"
#include "cab202_graphics.h"
#include "player.h"
#include "aliens.h"
#include "game.h"

char levels[5][18] = {"Basic","Harmonic Motion","Falling Motion","Drunken Motion","Aggressive Motion"};

void setup_game(Game * game) {
	game->delay = 25;
	reset_stats(game);
	setup_player(game);
	setup_aliens(game);
	setup_bullet(game);
	setup_alien_bullets(game);
	draw_screen(game);
	
}

void draw_screen(Game * game) {
	//Draw game/scoreboard divider
	draw_line(0,(screen_height() - 3),screen_width(),(screen_height() - 3),'_');
	//Draw score
	draw_string(screen_width()-20, screen_height()-2, "Score:");
	draw_int( screen_width()-13, screen_height()-2, (int) game->score);
	//Draw Lives
	draw_string(screen_width()-9, screen_height()-2, "Lives:");
	draw_int( screen_width()-2, screen_height()-2, (int) game->lives);
	//Draw level
	draw_string((screen_width()/2)-7, screen_height()-1, "Level: ");
	draw_int((screen_width()/2), screen_height()-1, (int) game->level);
	draw_string((screen_width()/2)+2, screen_height()-1, "-");
	draw_string((screen_width()/2)+4, screen_height()-1, (char *)levels[game->level-1]);
	//Draw Name/Number 
	draw_string(0,screen_height()-2,"Sebastian Sherry (n9161350)");
	//Game over message
	if (game->game_over == true) {
		draw_string((screen_width()/2)-24,screen_height()/2,"Game Over! Press 'r' to try again or 'q' to quit");
	}
	/*//Draw a
	draw_string(0, 0, "a");
	draw_double(0, 1, game->alien_a);
	//Draw b
	draw_string(4, 0, "h");
	draw_double(4, 1, game->alien_h);
	//Draw c
	draw_string(8, 0, "k");
	draw_double(8, 1, game->alien_k); 
	// x
	draw_string(12, 0, "x:");
	draw_double(12, 1, game->aliens[game->alien_moving]->x);
	// y
	draw_string(22, 0, "y:");
	draw_double(22, 1, game->aliens[game->alien_moving]->y);*/
}

void draw_game(Game * game) {
	clear_screen();
	draw_aliens(game);
	draw_player(game);
	draw_player_bullet(game);
	draw_alien_bullet(game);
	draw_screen(game);
	show_screen();
}

void reset_game(Game * game) {
	reset_stats(game);
	reset_player(game);
	reset_aliens(game);
	setup_bullet(game);
	setup_alien_bullets(game);
	draw_game(game);
}

void reset_stats(Game * game) {
	game->lives = 3;
	game->score = 0;
	game->level = 1;
	game->game_over = false;
	
}
void play_game() {
	Game game;
	
	game.game_clock = create_timer(CYCLE);
	game.quit = false;
	
	setup_game(&game);
	draw_game(&game);
	
	while (!game.quit) {
		
		if (timer_expired(game.game_clock)) {
			//turn result
			int key = get_char();

			if ( key == 'q' || key == 'Q' ) {
				return;
			} else if ( key == 'r' || key == 'R' ) {
				reset_game(&game);
			
			} else if ((key == 'l' || key == 'L') && game.game_over != true) {
				game.level += 1;
				if (game.level == 6) {
					game.level = 1;
				}
				reset_aliens(&game);
				setup_bullet(&game);
				setup_alien_bullets(&game);
			}
			if (game.game_over != true) {
				bool update = true;
		
				if (turn_player(&game,key) == true) {
					update = true;
				}
				if (update_aliens(&game) == true) {
					update = true;
				} 
				if (move_player_bullet(&game) == true) {
					update = true;
				}
				if (alien_bullets_turn(&game) == true) {
					update = true;
				}
			
				if (update == true) {
					draw_game(&game);
				}
			} else {
				draw_game(&game);
			}
		}
	}
}
