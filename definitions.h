/* 	definitions.h
	Declares all functions used in the different files
	This file written 2018 by Jacob Klasmark
	Based on file "mipslab.h" written 2015 by F Lundevall
		some parts are original code written by Axel Isaksson
	
	For copyright and licensing, see file COPYING
	
*/


#include <stdint.h>
#include <pic32mx.h>

// Creates boc, shot and arrow of "data type" "Object_type"
typedef enum {
	BOX,
	SHOT,
	ARROW
}Object_Type;

// Define a pixel, and give it x and y values 
typedef struct {
	unsigned char x;
	unsigned char y;
} Pixel;

// Define an Object, define what type of object it is and that it is 10 pixels big
typedef struct {
	Pixel piece[10];
	Object_Type object_type;
}Object;


// Screen functions
void quicksleep(int cyc);
uint8_t spi_send_recv(uint8_t data);
void display_init(void);
void display_menu(void);
void game_over(void);
void display_highscore(void);
void display_update(void);
void render(void);
void show_pixel(const Pixel *pixel);
void draw_object(const Object *object);
void display_menu(void);
void display_clear(void);
void show_highscore(int highscore);
void show_number(int number, int position);

// Helper
unsigned int pow(unsigned const int bas, volatile unsigned int exponent);
void move_vertical(Object *object, int way);
void move_horizont(Object *object, int way);
void make_box(Object *object);
void make_shot(Object *object);
void make_arrow(Object *object);
void create_object(Object *object, int box_x);
void save_score(void);
void init(void);
int getbtns(void);
void release_press (void);

// Dodge the bullet
void work (void);
void main_menu(void);
void highscore(void);
void game(void);
void user_isr( void );

//Data
char textbuffer[4][16];
extern const uint8_t const menu_font[512];
extern const uint8_t const gameover_font [512];
extern const uint8_t const font[128*8];
extern const uint8_t const highscore_font[256];
extern const uint8_t const number_font[80];