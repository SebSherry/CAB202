#ifndef GAME
#define GAME

#define ALIEN_NUM 10
#define ALIEN_BULLET_NUM 4
#define CYCLE 50

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#include "cab202_sprites.h"
#include "cab202_timers.h"
#include "cab202_graphics.h"

typedef struct Game { 
	//game clock
	timer_id game_clock;
	bool quit;
	
	bool game_over;                  
	int lives;                            
	int score;							  
	int level;		
	int alien_count;
	int last_to_shoot;	
	int delay;
	int base_alien_y[ALIEN_NUM];
	int group_width;
	int lower_dir; //Change in direction for drunk aliens 0-4
	int upper_dir; //Change in direction for drunk aliens 5-9
										  
	sprite_id player;						  
										  
	sprite_id bullet;					  
	timer_id bullet_timer;				  
										  
	sprite_id aliens[ALIEN_NUM];			  
	timer_id alien_timer;
											
	sprite_id alien_bullets[4];
	timer_id alien_bullet_release;
	timer_id alien_bullet_move;
	
	//Alien LVL 5 vars
	int alien_moving;
	int ref_x;
	bool split;
	bool reached_edge;
	bool reached_turn;
	int x_cont;
	int y_cont;
	int player_x;
	int player_y;
	timer_id alien_level5;
	double alien_a;
	double alien_y2;
	int offset;
	
	//Lvl 5 bullets
	timer_id bullet_curve;
	int is_held;
	bool jump_start; //To stop random first bullets
	double player_a;
	double player_b;
	double player_c;
} Game;	

void setup_game(Game * game);
void draw_screen(Game * game);
void draw_game(Game * game);
void reset_game(Game * game);
void reset_stats(Game * game);
void play_game();


#endif /* GAME */