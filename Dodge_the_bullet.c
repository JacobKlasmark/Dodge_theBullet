/* 	Dodge_the_bullet.cabs
	This file written 2018 by Jacob Klasmark
	
	For copying and licensing, see file COPYING
	*/

#include <stdint.h>
#include <pic32mx.h>
#include "definitions.h"

// Declare the variables used 
static Object box;
static Object shot[20];
static Object arrow;
int timeoutcount = 0;
int shot_counter = 0;
int score = 0;
int bestscore = 0;


void work (void){
	main_menu();
}

// Displays the main menu with 2 options to select. "start" or "Highscore"
void main_menu(void){
	display_clear();				// clear screen from gameover screen
	render();								
	display_menu();					// Display the menu on the screen
	make_arrow(&arrow);											
	create_object(&arrow, 0);		// create the arrow used to navigate menu
	draw_object(&arrow);			// display it to the screen
	render();
	release_press();				// fixes so you dont autostart a game if you 
									// pressed BTN 4 to get away from game over screen
	while(1){
		int button = getbtns();
		if ((button & 0x1) && (arrow.piece[7].x != 11)){		// navigate down with arrow
			move_vertical(&arrow, -8);
			display_menu();
			draw_object(&arrow);
			render();
		}
		if ((button & 0x2) && (arrow.piece[7].x != 19)){		// navigate up with arrow
			move_vertical(&arrow, 8);
			display_menu();
			draw_object(&arrow);
			render();
		}
		if (button & 0x8){											
			switch(arrow.piece[7].x){
				case 11:								// Goes to the game if arrow is at start
					game();
					break;
				case 19:
					highscore();						// Goes to highscore if arrow is at highscore
					break;
			}
		}
	}
}

void game_over(void){
	display_gameover();
	render();
	release_press();
	main_menu();
	
}


void highscore(void){
	display_highscore();
	show_highscore(bestscore);
	render();
	release_press();
	main_menu();
	
}

// Running the game
void game(void){
	asm volatile("ei");				// Enable interrupt on chipkit
	make_box(&box);	
	PORTE = 0;
	create_object(&box, 0);				// Display the box
	int i, m;
	for (i = 0; i < 20; i++)		// make "shot objects" in the shot array
		make_shot(&shot[i]);
	for (m = 0; m < 20; m++)		// So the coordinates of the shots resets between games
		create_object(&shot[m], 16);
	score = 0;
	T2CONSET = 0x8000;				//Starts the timer
	
	while (1){
		draw_object(&box);
		int j;
		for (j = 0; j < 20; j++)
			draw_object(&shot[j]);
		int k, l;
		// checks if the box is in contact with a shot
		for(k = 0; k < 20; k++){
			if ((shot[k].piece[0].x <= box.piece[9].x) &&  (shot[k].piece[5].x >= box.piece[3].x)){
				if ((shot[k].piece[0].y <= box.piece[9].y) && (shot[k].piece[4].y >= box.piece[0].y)){
					T2CONCLR = 0x8000;
					if (score > bestscore)
						bestscore = score;
					game_over();
				}
			}
		}
	render();
	}
}

// The game is basically run by interrupts.
/* Interrupt Service Routine */
void user_isr( void )
{
  if ((IFS(0) & 0x00000100) >> 8 == 1){		// timer interrupt flag
  IFS(0) = (IFS(0) & 0xfffffeff);		// resett timer flag
  timeoutcount++;
	int button = getbtns();
	if ((button & 0x1) && (box.piece[0].x > 0))			// button 1
		move_vertical(&box, -1);							// go upp		
	if ((button & 0x2) && (box.piece[9].x < 31))		// Button 2
		move_vertical(&box, 1);								// go down
	if ((button & 0x4) && (box.piece[9].y < 127))		// Button 3
		move_horizont(&box, 1);								// go right
	if ((button & 0x8) && (box.piece[0].y > 0))			// Button 4
		move_horizont(&box, -1);							// go Left
	// Moves the shots
	int i;
	for (i = 0; i < 20; i++)							// move shots Left
		move_horizont(&shot[i], -1);
  }
  if ((timeoutcount % 50) == 0){							
	  score++;								// +1 score every second alive
	  PORTE++;								// +1 for the led for every score we get				
  }
  if (timeoutcount == 100){						// position of 1 shot updates every 2 seconds
	timeoutcount = 0;						// y-coordinate is unsigned short. So it will go "full cricle"
	create_object(&shot[shot_counter], (box.piece[4].x));		// and appear on the screen again before getting a new x-coordinate
	shot_counter++;
	if (shot_counter == 20)								// Every shot is updated every 40 seconds
		shot_counter = 0;
  }
  return;
}
