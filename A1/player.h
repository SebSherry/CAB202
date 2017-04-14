#ifndef PLAYER
#define PLAYER

#include "game.h"
#include <stdbool.h>
#include "cab202_sprites.h"
#define BULLET_TIME 10
#define CURVE_INCREASE 0.075

void setup_player(Game * game);
void reset_player(Game * game);
void draw_player(Game * game);
bool turn_player(Game * game, int key);
void setup_bullet(Game * game);
void shoot_bullet(Game * game,int x, int y);
bool move_player_bullet(Game * game);
void draw_player_bullet(Game * game);
double cal_para(int x, double a, double b, double c);
void shoot_curved_bullet(Game * game);

#endif /* PLAYER */