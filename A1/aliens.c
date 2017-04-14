#include "cab202_sprites.h"
#include "cab202_timers.h"
#include "cab202_graphics.h"
#include "aliens.h"
#include "player.h"
#include "game.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


//intialize aliens at the start of the game
void setup_aliens(Game * game) {
	for (int i = 0; i < ALIEN_NUM; i++) {
		game->aliens[i] = create_sprite( 0, 0, 1, 1, "@" );
	}
	game->alien_timer = create_timer(ALIEN_UPDATE_TIME);
	game->alien_bullet_release = create_timer(3000);
	game->alien_level5 = create_timer(((rand()%3)+2)*1000);
	//define base y values for the aliens 
	int center = 5;
	game->base_alien_y[0] = center;
	game->base_alien_y[1] = center-2;
	game->base_alien_y[2] = center+2;
	game->base_alien_y[3] = center;
	game->base_alien_y[4] = center-2;
	game->base_alien_y[5] = center+2;
	game->base_alien_y[6] = center;
	game->base_alien_y[7] = center-2;
	game->base_alien_y[8] = center+2;
	game->base_alien_y[9] = center;
	//place aliens
	reset_aliens(game);
}


//Set Aliens to thier starting positons
void reset_aliens(Game * game) {
	int width = screen_width()/2;
	//int height = screen_height();
	
	//reset count
	game->alien_count = ALIEN_NUM;
	//make all aliens visable
	for (int i = 0; i < ALIEN_NUM; i++) {
		game->aliens[i]->is_visible = true;
	}	
	
	//Set positions
	
	//Left most single alien
	game->aliens[0]->x = width-12;
	game->aliens[0]->y = game->base_alien_y[0];
	
	//Left most double aliens
	
	//Top
	game->aliens[1]->x = width-8;
	game->aliens[1]->y = game->base_alien_y[1];
	//Bottom
	game->aliens[2]->x = width-8;
	game->aliens[2]->y = game->base_alien_y[2];
	
	//Center left single alien
	game->aliens[3]->x = width-4;
	game->aliens[3]->y = game->base_alien_y[3];
	
	//Center double aliens
	
	//Top
	game->aliens[4]->x = width;
	game->aliens[4]->y = game->base_alien_y[4];
	//Bottom
	game->aliens[5]->x = width;
	game->aliens[5]->y = game->base_alien_y[5];

	//Center right single alien
	game->aliens[6]->x = width+4;
	game->aliens[6]->y = game->base_alien_y[6];
	
	//Right most double aliens
	
	//Top
	game->aliens[7]->x = width+8;
	game->aliens[7]->y = game->base_alien_y[7];
	//Bottom
	game->aliens[8]->x = width+8;
	game->aliens[8]->y = game->base_alien_y[8];
	
	//Right most single aliens
	game->aliens[9]->x = width+12;
	game->aliens[9]->y = game->base_alien_y[9];
	
	//reset last to shoot
	game->last_to_shoot = -1;
	
	//reset lvl 5 stat
	game->alien_moving = -1;
	
	//reset timers
	reset_timer(game->alien_timer);
	reset_timer(game->alien_bullet_release);
	reset_timer(game->alien_level5);
}

void draw_aliens(Game * game) {
	for (int i = 0; i < ALIEN_NUM; i++) {
		draw_sprite(game->aliens[i]);
	}
}

bool update_aliens(Game * game) {
	if (!timer_expired(game->alien_timer) ) {
		return false;
	}
	get_group_width(game);
	int width = screen_width();
	int height = screen_height()-3;
	for ( int i = 0; i < ALIEN_NUM; i++ ) {
		if (game->aliens[i]->is_visible == true) {
			//LEVEL 1
			if (game->level == 1) {
				int new_x =  game->aliens[i]->x + 1;
				if (new_x >= width) {
					 game->aliens[i]->x = 0;
				} else {
					 game->aliens[i]->x = new_x;
				}
				
			//LEVEL 2
			} else if (game->level == 2) {
				int new_x =  game->aliens[i]->x + 1;
				if (new_x >= width) {
					 game->aliens[i]->x = 0;
				} else {
					 game->aliens[i]->x = new_x;
				}
				game->aliens[i]->y = round((2 * sin(0.5*(game->aliens[i]->x)))+game->base_alien_y[i]);
				
			//LEVEl 3
			} else if (game->level == 3) {
				int new_x =  game->aliens[i]->x + 1;
				if (new_x >= width ) {
					 game->aliens[i]->x = 0;
				} else {
					 game->aliens[i]->x = new_x;
				}
				int new_y =  game->aliens[i]->y + 1;
				if (new_y >= height ) {
					 game->aliens[i]->y = 0;
				} else {
					 game->aliens[i]->y = new_y;
				}
				if (game->aliens[i]->x == game->player->x && game->aliens[i]->y == game->player->y) {
					game->aliens[i]->is_visible = false;
					reset_player(game);
					game->lives--;
					if (game->lives == 0) {
						game->game_over = true;
					} else {
						game->alien_count--;
						if (game->alien_count == 0) {
						reset_aliens(game);
						}
					}
				}
				
			//LEVEL 4
			} else if (game->level == 4) {
				int new_x = game->aliens[i]->x + set_drunken_movement(game, i);
				if (new_x >= width ) {
					 game->aliens[i]->x = 0;
				} else {
					 game->aliens[i]->x = new_x;
				}
				int new_y =  game->aliens[i]->y + 1;
				if (new_y >= height ) {
					 game->aliens[i]->y = 0;
				} else {
					 game->aliens[i]->y = new_y;
				}
				if (game->aliens[i]->x == game->player->x && game->aliens[i]->y == game->player->y) {
					game->aliens[i]->is_visible = false;
					reset_player(game);
					game->lives--;
					if (game->lives == 0) {
						game->game_over = true;
					} else {
						game->alien_count--;
						if (game->alien_count == 0) {
						reset_aliens(game);
						}
					}
				}
				
			//LEVEL 5
			} else if (game->level == 5) {
				if (timer_expired(game->alien_level5)) {
					if (game->alien_moving == -1) {
						bool check = false;
						while (!check) {
							int alien = (rand() % ALIEN_NUM);
							if (game->aliens[alien]->is_visible == true) {
								game->alien_moving = alien;
								game->x_cont = game->aliens[alien]->x;
								game->y_cont = game->aliens[alien]->y;
								game->aliens[alien]->y--;
								check = true;
							}
						}
					}
				}
				if (game->alien_moving == i) {
					//tracking movement
					game->x_cont += 1;
					if (game->x_cont >= screen_width() ) { 
						game->x_cont = 0;
					} 
					game->y_cont += 1;
					if (game->y_cont >= screen_height()-3 ) {
						game->y_cont = 0;
					} 
					
					//actual movement
					move_alien_lvlFive(game);
				} else {
					int new_x =  game->aliens[i]->x + 1;
					if (new_x >= width ) {
						 game->aliens[i]->x = 0;
					} else {
						 game->aliens[i]->x = new_x;
					}
					int new_y =  game->aliens[i]->y + 1;
					if (new_y >= height ) {
						 game->aliens[i]->y = 0;
					} else {
						 game->aliens[i]->y = new_y;
					}
					if (game->aliens[i]->x == game->player->x && game->aliens[i]->y == game->player->y) {
						game->aliens[i]->is_visible = false;
						reset_player(game);
						game->lives--;
						if (game->lives == 0) {
							game->game_over = true;
						} else {
							game->alien_count--;
							if (game->alien_count == 0) {
							reset_aliens(game);
							}
						}
					}
				}
			}
		}
			
	}
	return true;
} 

//Level 4 functions
int set_drunken_movement(Game * game, int index) {
	int dir = 0;
	if (index <= 4) {
		if (game->group_width >= MAX_SPREAD) {
			game->lower_dir = 1;
		} else if (game->group_width <= MIN_SPREAD) {
			game->lower_dir = -1;
		}
		dir = game->lower_dir;
	} else {
		if (game->group_width >= MAX_SPREAD) {
			game->upper_dir = -1;
		} else if (game->group_width <= MIN_SPREAD) {
			game->upper_dir = 1;
		}
		dir = game->upper_dir;
	}
	int dx = 0;
	bool check = false;
	while (!check) {
		dx = (rand() % 3)*dir;
		if (game->aliens[index]->x+dx > screen_width()*0.05 && game->aliens[index]->x+dx < screen_width()*0.95) {
			int count = 0;
			for (int j = 0; j < ALIEN_NUM; j++) {
				if (index != j && game->base_alien_y[index] == game->base_alien_y[j]) {
					if ((game->aliens[index]->x+dx+1 != game->aliens[j]->x && game->aliens[index]->x +dx != game->aliens[j]->x && game->aliens[index]->x +dx-1 != game->aliens[j]->x) || (game->aliens[j]->is_visible == false)) {
						count++;
					}
				} 
			} 
			if ((game->base_alien_y[index] == 5 && count == 3) || (game->base_alien_y[index] != 5 && count == 2)) {
				check = true;
			}
		}
	}
	return dx;
}

void get_group_width(Game * game) {
	int left_x = 0;
	int right_x = 0;
	for (int i = 0; i < ALIEN_NUM; i++) {
		if (game->aliens[i]->is_visible == true) {
			left_x = game->aliens[i]->x;
			break;
		}
	} 
	for (int i = ALIEN_NUM-1; i > -1; i--) {
		if (game->aliens[i]->is_visible == true) {
			right_x = game->aliens[i]->x;
			break;
		}
	} 
	int width = right_x - left_x;
	game->group_width = width;
}

//lvl 5 alien movement
void move_alien_lvlFive(Game * game) {
	int i = game->alien_moving;
	if (game->reached_edge == false) {
		if (game->split == false) {
			for (int j = 0; j < ALIEN_NUM; j++) {
				if (game->aliens[j]->is_visible == true /*&& ((i != j || j == 0) || game->alien_count == 1)*/) {
					game->ref_x = j;
					break;
				}
			}
			game->split = true;
		}
		//Movement to reach the edge
		int new_y = game->aliens[i]->y + 1;
		if (new_y >= screen_height()-3) {
			game->aliens[i]->y = 0;
		} else {
			 game->aliens[i]->y = new_y;
		}
		game->aliens[i]->x--;
		if (game->aliens[i]->x <= game->aliens[game->ref_x]->x) {
			game->reached_edge = true;
			game->offset = game->aliens[i]->y;
			game->alien_a = cal_a(game->player->y,game->aliens[i]->y);
			game->alien_y2 = cal_y2(game->player->x, game->player->y, game->alien_a);
		} else if (game->aliens[i]->x == -1) {
			game->aliens[i]->x = screen_width();
		}
	} else { 
		game->aliens[i]->y++;
		game->aliens[i]->x = round(cal_para_alien(game->aliens[i]->y,game->alien_a,game->alien_y2));
		if (game->aliens[i]->x == game->player->x && game->aliens[i]->y == game->player->y) {
			game->aliens[i]->is_visible = false;
			game->alien_moving = -1;
			game->reached_edge = false;
			reset_timer(game->alien_level5);
			
			reset_player(game);
			game->lives--;
			if (game->lives == 0) {
				game->game_over = true;
			} else {
				game->alien_count--;
				if (game->alien_count == 0) {
				reset_aliens(game);
				}
			}
		} else if (game->aliens[i]->y >= screen_height()-3) {
			game->alien_moving = -1;
			game->reached_edge = false;
			game->aliens[i]->x = game->x_cont;
			game->aliens[i]->y = game->y_cont;
			reset_timer(game->alien_level5);
		}
	}
}

double cal_para_alien(int y, double a, double y2) {
	//x = Ay(y-y2)
	double x = a*y*(y-y2);
	return x;
}

double cal_a(int player_y,int alien_y, int player_x, int alien_x) {
	int width = screen_width();
	double a = 0.2;
	double rnd = alien_x;
	rnd = rand() % width;
	if (rnd > alien_x) {
		rnd = rnd - alien_x;
	}
	int mid = (player_y - alien_y)/2;
	if (rnd > mid) {
		a += rnd /1000;
	} else {
		a -= rnd /1000;
	}
	bool check = false;
	while (!check) {
		for (int y = alien_y; y <= player_y;y++) {
			int x = round(cal_para_alien(y,a,cal_y2(player_x,player_y,a)));
			if (x < 0) {
				a -= 0.1;
				break;
			} 
		}
		check = true;
	}
	return a;
	
}

double cal_y2(int player_x, int player_y, double a) {
	double y2 = player_y - (player_x/(a*player_y));
	return y2;
	
}
/*---------------------------------------------------------------------
Bullet functions */
bool alien_bullets_turn(Game * game) {
	bool check_new = release_bullets(game);
	bool check_move = move_alien_bullet(game);
	if (check_new == true || check_move == true) {
		return true;
	} else {
		return false;
	}
}

//creates an alien bullets
void setup_alien_bullets(Game * game) {
	for (int i = 0; i < ALIEN_BULLET_NUM;i++) {
		game->alien_bullets[i] = create_sprite(-1,0,1,1,"*");
		game->alien_bullets[i]->is_visible = false;
		game->alien_bullets[i]->dy = 1;
	}
	game->alien_bullet_move = create_timer(ALIEN_BULLET_TIME);
}

//Determines if a new alien bullet sould be released
bool release_bullets(Game * game) {
	if (!timer_expired(game->alien_bullet_release)) {
		return false;
	} 
	for (int i = 0; i < ALIEN_BULLET_NUM;i++) {
		if (game->alien_bullets[i]->is_visible == false) {
			int num = 0;
			if (game->alien_count > 1) {
				bool check = false;
				while (!check) {
					num = (rand() % ALIEN_NUM);
					if (num != game->last_to_shoot) {
						if (game->aliens[num]->is_visible == true) {
							check = true;
						}
					}
				}	
			} else {
				for ( int i = 0; i < ALIEN_NUM; i++ ) {
					if (game->aliens[i]->is_visible == true) {
						num = i;
						break;
					}	
				}
			}
			game->alien_bullets[i]->x = game->aliens[num]->x;
			game->alien_bullets[i]->y = game->aliens[num]->y;
			game->alien_bullets[i]->is_visible = true;
			game->last_to_shoot = num;
			break;
		}
	}
	return true;
}

//Move the alien bullets. Removes them if they hit the bottom of the screen
bool move_alien_bullet(Game * game) {
	if (!timer_expired(game->alien_bullet_move)) {
			return false;
	}

	for (int i = 0; i < ALIEN_BULLET_NUM;i++) {
		if (game->alien_bullets[i]->is_visible == true) {
			game->alien_bullets[i]->y += game->alien_bullets[i]->dy;
			if (game->alien_bullets[i]->y == screen_height()-3) {
				game->alien_bullets[i]->is_visible = false;
			} else if (game->alien_bullets[i]->x == game->player->x && game->alien_bullets[i]->y == game->player->y) {
				game->alien_bullets[i]->is_visible = false;
				reset_player(game);
				game->lives--;
				if (game->lives == 0) {
					game->game_over = true;
				}
			}
		}
	}
	return true;
}

//draws all the bullets
void draw_alien_bullet(Game * game) {
	for (int i = 0; i < ALIEN_BULLET_NUM;i++) {
		draw_sprite(game->alien_bullets[i]);
	}
}
