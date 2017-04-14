#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "aliens.h"
#include "player.h"
#include "game.h"
#include "cab202_sprites.h"
#include "cab202_graphics.h"
#include "cab202_timers.h"

/*---------------------------------------------------------------------
Player functions */
void setup_player(Game * game) {
	game->player = create_sprite( 0, 0, 1, 1, "$" );
	reset_player(game);
}

void reset_player(Game * game) {
	game->player->x = screen_width()/2;
	game->player->y = screen_height()-4;
	game->is_held = 0;
	draw_player(game);
}

void draw_player(Game * game) {
	draw_sprite(game->player);
}

bool turn_player(Game * game, int key) {
	if ( (key != 'z' && key != 'c') && game->is_held !=0) {
		shoot_curved_bullet(game);
	}
	if ( key == 'a' &&  game->player->x > 0 ) {
		game->player->x--;
		return true;
	}
	else if (key == 'd' &&  game->player->x < screen_width()-1) {
		game->player->x++;
		return false;
	}
	else if (key == 's') {
		if (game->bullet->is_visible == false) {
			game->is_held = 0;
			game->player_a = 0;
			shoot_bullet(game,game->player->x, game->player->y);
		}
		return true;
	} 
	else if (key == 'z') {
		if (game->bullet->is_visible == false) {
			if (game->is_held == 0) {
				game->bullet_curve = create_timer(200);
				game->player_a = 0;
				game->is_held = 1;
			} else if (timer_expired(game->bullet_curve) && game->is_held == 1) {
				game->player_a += CURVE_INCREASE;
			}
			
		}
		return true;
	}
	else if (key == 'c') {
		if (game->bullet->is_visible == false) {
			if (game->is_held == 0) {
				game->bullet_curve = create_timer(200);
				game->player_a = 0;
				game->is_held = -1;
			} else if (timer_expired(game->bullet_curve) && game->is_held == -1) {
				game->player_a += CURVE_INCREASE;
			}
		}
		return true;
	}
	else {
		return false;
	}

}

/*---------------------------------------------------------------------
Bullet functions */

void shoot_curved_bullet(Game * game) {
	if (game->player_a >= CURVE_INCREASE) {
		game->player_a = game->player_a*game->is_held;
		game->player_c = game->player->x;
		game->player_b = -(game->player_a)*game->player->y;
		shoot_bullet(game,game->player->x, game->player->y);
		game->is_held = 0;
	}
}
double cal_para(int x, double a, double b, double c) {
	//k = y value;
	//h = x value;
	//y = a(x-h)^2 + k
	double y = (a*pow(x,2))+(b*x)+c;
	return y;
}
void setup_bullet(Game * game) {
	game->bullet = create_sprite(-1,0,1,1,"|");
	game->bullet->is_visible = false;
	game->bullet->dy = -1;
	game->bullet_timer = create_timer(BULLET_TIME);
}

void shoot_bullet(Game * game,int x, int y) {
	if (game->bullet->is_visible == true) {
		return;
	} else {
		game->bullet->x = x;
		game->bullet->y = y;
		game->bullet->is_visible = true;
	}
}

bool move_player_bullet(Game * game) {
	if (game->bullet->is_visible != true) {
		return false;
	} else if (!timer_expired(game->bullet_timer)){
		return false;
	} else {
		if (game->player_a != 0) {
			game->bullet->y += game->bullet->dy;
			if (game->bullet->y == 0) {
				game->bullet->is_visible = false;
			} 
			game->bullet->x = cal_para(game->bullet->y,game->player_a,game->player_b,game->player_c);
		} else {
			game->bullet->y += game->bullet->dy;
			if (game->bullet->y == 0) {
				game->bullet->is_visible = false;
			} 
		}
		
	}
	int x = game->bullet->x;
	int y = game->bullet->y;
	for (int i = 0;i < ALIEN_NUM;i++) {
		if (game->aliens[i]->x == x && game->aliens[i]->y == y && game->aliens[i]->is_visible == true) {
			game->bullet->is_visible = false;
			game->aliens[i]->is_visible = false;
			game->score += 30;
			game->alien_count--;
			if (game->alien_moving == i) {
				game->alien_moving = -1;
				game->reached_edge = false;
				reset_timer(game->alien_level5);
			}
			if (game->alien_count == 0) {
				game->score += 500;
				reset_aliens(game);
			}
		}
	}
	return true;
}

void draw_player_bullet(Game * game) {
	draw_sprite(game->bullet);
}

