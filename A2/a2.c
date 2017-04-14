#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sprite.h"
#include "lcd.h"
#include "graphics.h"
#include "cpu_speed.h"
#include "usb_serial.h"

#define FREQUENCY 8000000.0

//Game struct
typedef struct Game {   
	//general
	int lives;                            
	int score;							  
	int lvl;
	int seed;
	int speed;
	int face_thresh;
	bool playing;

	//player
	Sprite player;
	
	//faces
	Sprite faces[3];
	int face_ids[3];
	
} Game;	

Game game;

//GLOBLES
bool info = true;
bool menu = false;
int width = 78;
int debouce_left = 0;
int debouce_right = 0;
int face_delay = 0;
int noPlace[] = {-1,-1};
bool hold = false;
bool holdL = false;
char* endgame = "";

//bitmaps
unsigned char bm_player[8] = {
		0b00011000,
        0b00011000,
        0b00111100,
		0b00111100,
		0b01111110,
		0b01111110,
		0b11111111,
		0b11111111

   };

unsigned char bm_faces[3][20] = {
   {	//Smilely
        0b01111111, 0b11100000,
        0b11000000, 0b00110000,
        0b10010000, 0b10010000,
        0b10010000, 0b10010000,
        0b10000000, 0b00010000,
        0b10100000, 0b01010000,
        0b10100000, 0b01010000,
		0b10011111, 0b10010000,
		0b11000000, 0b00110000,
        0b01111111, 0b11100000,
   },
   {	//Angry
        0b01111111, 0b11100000,
        0b11000000, 0b00110000,
        0b10010000, 0b10010000,
        0b10001001, 0b00010000,
        0b10000000, 0b00010000,
        0b10000110, 0b00010000,
        0b10001001, 0b00010000,
		0b10010000, 0b10010000,
		0b11000000, 0b00110000,
        0b01111111, 0b11100000,
   },
   {	//Mad
        0b01111111, 0b11100000,
        0b11000000, 0b00110000,
        0b10010000, 0b10010000,
        0b10010001, 0b00010000,
        0b10000000, 0b00010000,
        0b10111111, 0b11010000,
		0b10000011, 0b11010000,
		0b10000001, 0b10010000,
		0b11000000, 0b00110000,
        0b01111111, 0b11100000,
   },
};
//------------------------------------------------------

//-------HEADERS---------
//Hardware and interupts
void init_hardware(void);
void set_interupts(void);
void set_timers(void);
void setup_usb(void);

//Game functions
void init_game(void);
int gen_face_location_x(int face_num, int previous);
int gen_face_location_y(int face_num, int previous);
bool check_x(int face_num, int x);
bool check_hit(int x1, int y1, int x2, int y2, bool face);
void determine_bounce(int face1, int face2);
int gen_face_id(int face_num);
void collision_detection(void);
void update_speed(void);
void set_gameover(int result);

//draw functions
void draw_info(void);
void draw_lvl_select(void);
void draw_game(void);
void draw_endgame(void);
void draw_map(void);
void draw_player(void);
void draw_faces(void);

//------------------------------------------------------


//FUNCTIONS
int main() {
    set_clock_speed(CPU_8MHz);

    //Setup the hardware
    init_hardware();
	//interupts
	set_interupts();
	
	//name screen
	while (info) {
		clear_screen(); 
		draw_info();
	}
	//Main loop
	while (1) {
		//turn on level menu
		menu = true;
		//intial level setup for menu purposes
		game.lvl = 1;
		//intial seed randomizing for extra randomness
		game.seed = rand();

		//level select
		while (menu) {
			draw_lvl_select();
			_delay_ms(25);
		}
		//seed random generator
		srand(game.seed);
		
		//setup game
		init_game();
		//USB init
		if (game.lvl == 3) {
			setup_usb();
		}
		clear_screen();
		draw_game();
		game.playing = true;
		//Game loop
		while (game.playing) {
			clear_screen();
			draw_game();
			_delay_ms(25);
		} 
		//end game loop
		while (1) {
			clear_screen();
			draw_endgame();
			_delay_ms(25);
		}
	}
    // end program
    return 0;
}

//------------HARDWARE & INTERUPTS------------
void init_hardware(void) {
    //Initialising the LCD screen
    LCDInitialise(LCD_DEFAULT_CONTRAST);
	
	//set buttons as inputs
	DDRB &= ~((1 << PB0) | (1 << PB1));
	
	//intialize ADC
	//Code based off topic 11 sprite_adc.c
	//prescaler set to 128
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	//timers
	set_timers();
}

void set_timers(void) {
	// Timer 4 for button debouncing
	// Configure in normal operation mode
    TCCR4B &= ~((1<<WGM02));

    // Set prescaler
	// Prescaler:256   Overflow: 0.032736
    TCCR4B |= (1<<CS02) /*| (1<<CS00)*/;
    TCCR4B &= ~((1<<CS01) &(1<<CS00));
	
	// Timer 0 for face movement 
	// Configure in normal operation mode
    TCCR0B &= ~((1<<WGM02));

    // Set prescaler
	// Prescaler:1024  Overflow: 0.03264
    TCCR0B |= (1<<CS02) | (1<<CS00);
    TCCR0B &= ~((1<<CS00) /*&(1<<CS01)*/);
	
	// Timer 1 for adc 
	// Configure in normal operation mode
    TCCR1B &= ~((1<<WGM02));

    // Set prescaler
	// Prescaler:1  Overflow: 0.008169375
    TCCR1B |= (1<<CS00); //| (1<<CS00);
    TCCR1B &= ~((1<<CS01)&(1<<CS02));
} 

void set_interupts(void) {
	//Enable buttons in the PCMSK0 register
	//PCMSK0 |= (1<<PCINT0);
	PCMSK0 |= (1<<PCINT1);

	//Enable Pin change interrupts
	PCICR |= (1<<PCIE0);
	
	//External interrups
	EICRA |= (/*(1<<ISC00)&&(1<<ISC01)&&(1<<ISC10)&&*/(1<<ISC11)); 
	
	//Enable timer interrups
	TIMSK0 |= (1<<TOIE0);
	TIMSK1 |= (1<<TOIE1);
	TIMSK4 |= (1<<TOIE4);
		
	//Enable global interrupts
	sei();
}

void debouce() {
	if (bit_is_clear(PINB, 1)) {
		debouce_left++;
	} 
	if (bit_is_clear(PINB, 0)) {
		debouce_right++;
	}
}

// read adc value
//Code based off topic 11 sprite_adc.c
uint16_t adc_read_position(uint8_t ch)
{
	//select channel
    ch &= 0b00000111;
    ADMUX = (ADMUX & 0xF8)|ch;     
 
    // start conversion
    ADCSRA |= (1<<ADSC);
 
    // wait for conversion to complete
    while(ADCSRA & (1<<ADSC));
 
    uint16_t result = ADC;
	float max_adc = 1023.0;
 	long max_lcd_adc = (result*(long)(LCD_X-8)) / max_adc;
	return (76-max_lcd_adc);
}

void setup_usb(void) {
	usb_init();
	draw_string(10, 17, "Please Connect");
	draw_string(26, 24, "Via USB");
	show_screen();
	while(!usb_configured() || !usb_serial_get_control());
	_delay_ms(100);
}

//------------GAME FUNCTIONS------------
//setup game parameters
void init_game(void) {
	//general
	game.lives = 3;
	game.score = 0;
	game.seed = 0;
	game.speed = 1;
	update_speed();
	game.playing = false;
	
	//player
	init_sprite(&game.player, width/2, LCD_Y - 8, 8, 7, bm_player);
	if (game.lvl == 2) {
		game.player.x = adc_read_position(0);
	} else if (game.lvl == 3) {
		game.player.y = LCD_Y/2;
	}
	
	//faces
	for (int i = 0; i < 3; i++) {
		int face = gen_face_id(i);
		game.face_ids[i] = face;
		if (game.lvl == 3) {
			init_sprite(&game.faces[i], gen_face_location_x(i,-1), gen_face_location_y(i,-1), 12, 10, bm_faces[face]);
			int dir = rand() % 360;
			game.faces[i].dx = cos(dir); 
			game.faces[i].dy = sin(dir);
		} else {
			init_sprite(&game.faces[i], gen_face_location_x(i,-1), 11, 12, 10, bm_faces[face]);
			game.faces[i].dy = 1;
		}
	}
}

int gen_face_location_x(int face_num, int previous) {
	bool check = false;
	int new_x = 40;
	while (!check) {
		new_x = rand() % width-6;
		if (new_x != previous || new_x != previous-1 || new_x != previous+1) {
			if (new_x <= 0) {
				new_x = 1;
			}
			int count = 0;
			for (int i = 0; i < 3; i++) {
				if (i != face_num) {
					if (check_x(i,new_x)) {
						count++;
					}
				}
				
			}
			if (count >= 2) {
				return new_x;
			}
		}
	}
	return new_x;
}

int gen_face_location_y(int face_num, int previous) {
	bool check = false;
	int new_y = 11;
	while (!check) {
		new_y = rand() % LCD_Y - 10;
		new_y += 11;
		if (new_y != previous) {
			if (new_y >= LCD_Y - 10) {
				new_y = LCD_Y - 11;
			} else if (new_y <= 10) {
				new_y = 11;
			}
			if ((!check_x(face_num,game.player.x) && ((new_y < game.player.y-5) || (new_y > game.player.y+13))) || check_x(face_num,game.player.x)) {
				return new_y;
			}
		}
	}
	return new_y;
}

bool check_x(int i, int x) {
	if ((((x - game.faces[i].x) > 17) || ((x - game.faces[i].x) < -17)) && (((game.faces[i].x - x) > 17) || ((game.faces[i].x - x) < -17))) {
		return true;
	} else {
		return false;
	}
}

//1 == player, 2 == face. bool face is to check for face on face collisions
bool check_hit(int x1, int y1, int x2, int y2, bool face) {
	int i = 0;
	if (face) {
		i = 4;
	}
	if (((x1 <= x2+11)&&(x1+(7+i) >= x2)) && ((y1 <= y2+10)&&(y1+(7+(i-2)) >= y2))) {
		return true;
	} else {
		return false;
	}
}

void determine_bounce(int face1, int face2) {
	if ((game.faces[face1].x+8 <= game.faces[face2].x && game.faces[face2].x <= game.faces[face1].x+11) || (game.faces[face2].x+8 <= game.faces[face1].x && game.faces[face1].x <= game.faces[face2].x+11)) {
		game.faces[face1].dx = game.faces[face1].dx*-1;
		game.faces[face2].dx = game.faces[face2].dx*-1;
	} else if ((game.faces[face1].y+6 <= game.faces[face2].y && game.faces[face2].y <= game.faces[face1].y+9) || (game.faces[face2].y+6 <= game.faces[face1].y && game.faces[face1].y <= game.faces[face2].y+9)) {
		game.faces[face1].dy = game.faces[face1].dy*-1;
		game.faces[face2].dy = game.faces[face2].dy*-1;
	}
}

int gen_face_id(int face_num) {
	int count = 0;
	int face = 0;
	bool check = false;
	while (!check) {
		face = rand() % 3;
		for (int f = 0;f < 3;f++) {
			if (f != face_num) {
				if (game.face_ids[f] != face) {
					count++;
				}
			}
		}
		if (count == 2) {
			check = true;
		} else {
			count = 0;
		}
	}
	return face;
}

void collision_detection(void) {
	for (int i = 0; i < 3;i++) {
		if (check_hit(game.player.x, game.player.y, game.faces[i].x, game.faces[i].y, false)) {
			game.faces[i].x = gen_face_location_x(i,game.faces[i].x);
			if (game.lvl == 3) {
				game.faces[i].y = gen_face_location_y(i,game.faces[i].y);
			} else {
				game.faces[i].y = 11;
			}
			//hit happy
			if (game.face_ids[i] == 0) {
				game.score += 2;
				if (game.score == 20) {
					game.playing = false;
					set_gameover(1);
				}
			//hit angry
			} else if (game.face_ids[i] == 1) {
				game.lives--;
				if (game.lives == 0) {
					game.playing = false;
					set_gameover(0);
				}
			//hit mad
			} else if (game.face_ids[i] == 2) {
				if (game.speed < 3) {
					game.speed++;
					update_speed();
				}
			}
		}
	}
	if (check_hit(game.faces[0].x, game.faces[0].y, game.faces[1].x, game.faces[1].y, true)) {
		determine_bounce(0, 1);
	} 
	if (check_hit(game.faces[0].x, game.faces[0].y, game.faces[2].x, game.faces[2].y, true)) {
		determine_bounce(0, 2);
	}
	if (check_hit(game.faces[1].x, game.faces[1].y, game.faces[2].x, game.faces[2].y, true)) {
		determine_bounce(1, 2);
	}
	
}

void update_speed(void) {
	switch (game.speed) {
		case 1:
		game.face_thresh = 9;
		break;
		
		case 2:
		game.face_thresh = 7;
		break;
		
		case 3:
		game.face_thresh = 5;
		break;
	}
}
//------------DRAW FUNCTIONS------------
//Display  name and student number
void draw_info(void) {
	draw_string(1, 4, "Sebastian Sherry");
    draw_string(22, 13, "N9161350");
	draw_string(1, 22, "Press right hand");
	draw_string(28,31, "button");
	show_screen();
}
//Draw level select screen
void draw_lvl_select(void) {
	clear_screen();
	char buff[80];
	sprintf(buff, "%01d", game.lvl);
    draw_string(22, 18, "Level:");
	draw_string(54, 18, buff);
	show_screen();
}
//Draw full game
void draw_game(void) {
	draw_map();
	draw_player();
	draw_faces();
	show_screen();
}

void draw_endgame(void) {
	draw_string(20, 12, endgame);
	draw_string(8, 21, "Please restart");
	draw_string(10, 30, "to play again");
	show_screen();
}
//Draw game map
void draw_map(void) {
	//display level and lives 
	char buffLives[80];
	char buffScore[80];
	//display lives
	sprintf(buffLives, "%01d", game.lives);
    draw_string(4, 0, "L:");
	draw_string(16, 0, buffLives);
	//display score
	sprintf(buffScore, "%01d", game.score);
    draw_string(26, 0, "S:");
	draw_string(38, 0, buffScore);
	draw_line(0, 8, 84, 8);
}

void draw_player(void) {
	draw_sprite(&game.player);
}

void draw_faces(void) {
	for (int i = 0; i < 3; i++) {
		draw_sprite(&game.faces[i]);
	}
}

void set_gameover(int result) {
	if (result == 1) {
		endgame = "You Win!";
	} else if (result == 0) {
		endgame = "You Died!";
	}
	
}

//------------------------------------------------------

//INTERUPTS

//pin change buttons
//to stop menu skipping bugs
ISR(PCINT0_vect)
{	
	//pin change on sw1 (right)
	if ((PINB>>PINB1)&1){ 
		if (menu) {
			hold = false;
			game.seed += 5;
		} 
	}
}

//timer 4 
//debounce and player movement
ISR(TIMER4_OVF_vect) { 
	if (menu == true) {
		game.seed +=30;
	}
	if (game.lvl == 3 && game.playing == true) {
		if(usb_serial_available() != 0) {
			int16_t curr_char = usb_serial_getchar();
			if (curr_char == -1) { 
			} else if (curr_char == 'w') {
				game.player.y -= 2;
			} else if (curr_char == 's') {
				game.player.y += 2;
			} else if (curr_char == 'a') {
				game.player.x -= 2;
			} else if (curr_char == 'd') {
				game.player.x += 2;
			}
			if (game.player.x <= 0) {
				game.player.x = 0;
			} else if (game.player.x >= width) {
				game.player.x = width;
			} else if (game.player.y <= 11) {
				game.player.y = 11;
			} else if (game.player.y >= LCD_Y-10) { 
				game.player.y = LCD_Y-10;
			}
		}
	} else {
		int threshold = 300;
		debouce();
		if (debouce_left >= threshold) {
			//pin change on sw0 (Left)
			if ((PINB>>PINB0)&1){
				if (menu) {
					menu = false;
				} else if (game.lvl != 2 && game.playing == true) {
					game.player.x = game.player.x - 2;
					if (game.player.x <= 0) {
						game.player.x = 0;
					} 
				}
			} 
			debouce_left = 0;
		} else if (debouce_right >= threshold) {
			//pin change on sw1 (right)
			if ((PINB>>PINB1)&1){ 
				if (info) {
					hold = true;
					info = false;
				} else if (menu && !hold) {
					hold = true;
					game.lvl++;
					if (game.lvl > 3) {
						game.lvl = 1;
					}
				} else if (game.lvl != 2 && game.playing == true) {
					game.player.x = game.player.x + 2;
					if (game.player.x >= width) {
						game.player.x = width;
					} 
				}
			}
			debouce_right = 0;
		}
	}
}

//timer 0
//face movement
ISR(TIMER0_OVF_vect) { 
	if (game.lvl != 3 && game.playing == true) {
		face_delay++;
		if (face_delay == game.face_thresh) {
			for (int i = 0; i < 3; i++) {
				game.faces[i].y += game.faces[i].dy;
				if (game.faces[i].y >= LCD_Y-10) {
					game.faces[i].y = 11;
				}
			}
			face_delay = 0;
			collision_detection();
		}
	} else if (game.lvl == 3 && game.playing == true) {
		face_delay++;
		if (face_delay == game.face_thresh) {
			for (int i = 0; i < 3; i++) {
				game.faces[i].x += game.faces[i].dx;
				game.faces[i].y += game.faces[i].dy;
				if (game.faces[i].x >= width - 6) {
					game.faces[i].dx = game.faces[i].dx * -1;
					game.faces[i].x = width - 6;
				} else if (game.faces[i].x <= 0) {
					game.faces[i].dx = game.faces[i].dx * -1;
					game.faces[i].x = 0;
				}
				if (game.faces[i].y >= LCD_Y-10) {
					game.faces[i].dy = game.faces[i].dy * -1;
					game.faces[i].y = LCD_Y-10;
				} else if (game.faces[i].y < 11) {
					game.faces[i].dy = game.faces[i].dy * -1;
					game.faces[i].y = 11;
				}
			}
			face_delay = 0;
			collision_detection();
		}
	}
}

//timer 1
//Player adc
ISR(TIMER1_OVF_vect) { 
	if (game.lvl == 2 && game.playing == true) {
		game.player.x = adc_read_position(0);
	}
}