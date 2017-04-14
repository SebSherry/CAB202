#ifndef ALIENS
#define ALIENS

#include <stdbool.h>
#include "game.h"

#define ALIEN_UPDATE_TIME (150)
#define ALIEN_BULLET_TIME (600)
#define MIN_SPREAD 24
#define MAX_SPREAD 56

void setup_aliens(Game * game);
void reset_aliens(Game * game);
void draw_aliens(Game * game);
bool update_aliens(Game * game);
bool alien_bullets_turn(Game * game);
void setup_alien_bullets(Game * game);
bool release_bullets(Game * game);
bool move_alien_bullet(Game * game);
void draw_alien_bullet(Game * game);
void move_alien_lvlFive(Game * game);
int set_drunken_movement(Game * game, int index);
void get_group_width(Game * game);
double cal_para_alien(int y, double a, double y2);
double cal_y2(int player_x, int player_y, double a);
double cal_a();

#endif /* ALIENS */