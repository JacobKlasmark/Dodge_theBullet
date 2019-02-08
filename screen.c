/* 	screen.c
	This file was written 2018 by Jacob Klasmark
	Based on the "mipsfunc.c" file written 2015 by F Lundvall
		Some parts are original code written by Axel Isaksson
	
	For copyright and icensing, see file COPYING*/
	

#include <stdint.h>
#include <pic32mx.h>
#include "definitions.h"


/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

static uint8_t buffer[512] = {0};

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}
	
// Disaplys the text GAME OVER on the screen
void display_gameover(void){
	int i;
    for(i = 0; i < 512; i++)
		buffer[i] |= gameover_font[i];
}

// Displays HIGHSCORE on the screen
void display_highscore(void){
	int i;
    for(i = 0; i < 256; i++)
		buffer[i] |= highscore_font[i];
}


// A modified version of the display_update function
void render(void) {
    int page, j;
    for(page = 0; page < 4; page++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);   
        spi_send_recv(page);  

        spi_send_recv(0x0);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        // j is the x axis of the current page
        for(j = 0; j < 128; j++) {
            // Each byte sent to this function is a 8 pixel high column on the display
            // the lsb is the top most pixel and the msb is the most bottom pixel
            spi_send_recv(buffer[page*128 + j]);
            // Clear the buffer after drawing it
            buffer[page*128 + j] = 0x0;
        }
    }
}

// Used to lighting up the pixel on the specified x,y coordinate
void show_pixel(const Pixel *pixel) {
    if (pixel->x > 31 || pixel->x < 0 || pixel->y > 127 || pixel->y < 0)
        return;

    int position;
	position = (((pixel->x)/8)*128 + pixel->y);
	buffer[position] 	|= pow(2, (pixel->x - ((pixel->x)/8) * 8));
}

// Takes the x,y coordiantes of the object we want to present on the screen
// uses the show_pixel function to do this.
void draw_object(const Object *object) {
    int i;
    for(i = 0; i < 10; i++)
        show_pixel(&object->piece[i]);
}

// Displays the menu
void display_menu(void){
	int i;
    for(i = 0; i < 512; i++)
		buffer[i] |= menu_font[i];
}

// Clears the display of any pixels
void display_clear(void){
	int i;
	for(i = 0; i < 4; i++)
		buffer[i] = 0;
		
}

// Display the highest score by using show_number function
void show_highscore(int highscore){
	int i;
	for(i = 5; i > -1; i--){
		int number = (highscore / pow(10,i));
		highscore = highscore - number * pow(10,i);
		int position = 81 - (i * 8) + 256;
		show_number(number, position);	
	}	
}

// Disaply a number with the number_font and the buffer for the screen
void show_number(int number, int position){
	int i;
	for(i = 0; i < 8; i++){
		buffer[position + i] = number_font[number * 8 + i];
	}
}
