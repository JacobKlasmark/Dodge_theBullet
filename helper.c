/* 	helper.c 
	This file written 2018 by Jacob Klasmark
	
	For copyright and licensing, see file COPYING*/


#include <stdint.h>
#include <pic32mx.h>
#include "definitions.h"

// ANd array with the different objects we want on the screen
static const Object_Type objects[] = { BOX, SHOT, ARROW};

// A power function with only integers.
unsigned int pow(unsigned const int bas, volatile unsigned int exponent) {
	unsigned int resultat = 1;
	for (;exponent > 0; exponent--)
		resultat *= bas;
	return resultat;
}


// Used to move an object Up or Down.
void move_vertical(Object *object, int way) {
    int i;
    for(i = 0; i < 10; i++){
        object->piece[i].x += way;
    }
}

// Used to move an object Left or Right
void move_horizont(Object *object, int way) {
	int i;
	for (i = 0; i < 10; i++) {
		object->piece[i].y += way;
	}
}


// Decides that the object we put as argument is an object of "BOX" type
void make_box(Object *object){
	object->object_type = objects[0];
}

// Decides that the object we put as argument is an object of "SHOT" type
void make_shot(Object *object){
	object->object_type = objects[1];
}

// Decides that he object we put as argument is an object of "ARROW" type
void make_arrow(Object *object){
	object->object_type = objects[2];
}

// The "BOX" is the shape that we control, and this function draws it on the coordinates
// x: (15-17) and y: (30-33)
// The "SHOT" are the bullets we are dodging. These are drawn slightly off the screen to the right
// and on the same x coordinate as our "BOX"
// The "ARROW" is used at the menu to show what we are clicking on
void create_object(Object *object, int box_x){
	switch(object->object_type){
		case BOX:
			//Top
			object->piece[0].x = 15;		
			object->piece[0].y = 30;
			object->piece[1].x = 15;
			object->piece[1].y = 31;
			object->piece[2].x = 15;
			object->piece[2].y = 32;
			object->piece[3].x = 15;
			object->piece[3].y = 33;
			//Left
			object->piece[4].x = 16;
			object->piece[4].y = 30;
			//Right
			object->piece[5].x = 16;
			object->piece[5].y = 33;
			//Bottom
			object->piece[6].x = 17;
			object->piece[6].y = 30;
			object->piece[7].x = 17;
			object->piece[7].y = 31;
			object->piece[8].x = 17;
			object->piece[8].y = 32;
			object->piece[9].x = 17;
			object->piece[9].y = 33;
			break;
		case SHOT:
			//TOP
			object->piece[0].x = box_x;		
			object->piece[0].y = 128;
			object->piece[1].x = box_x;
			object->piece[1].y = 129;
			object->piece[2].x = box_x;
			object->piece[2].y = 130;
			object->piece[3].x = box_x;
			object->piece[3].y = 131;
			object->piece[4].x = box_x;
			object->piece[4].y = 132;
			//Bottom
			object->piece[5].x = (box_x+1);
			object->piece[5].y = 128;
			object->piece[6].x = (box_x+1);
			object->piece[6].y = 129;
			object->piece[7].x = (box_x+1);
			object->piece[7].y = 130;
			object->piece[8].x = (box_x+1);
			object->piece[8].y = 131;
			object->piece[9].x = (box_x+1);
			object->piece[9].y = 132;
			break;
		case ARROW:
			object->piece[0].x = 9;
			object->piece[0].y = 50;
			object->piece[1].x = 10;
			object->piece[1].y = 51;
			object->piece[2].x = 11;
			object->piece[2].y = 47;
			object->piece[3].x = 11;
			object->piece[3].y = 48;
			object->piece[4].x = 11;
			object->piece[4].y = 49;
			object->piece[5].x = 11;
			object->piece[5].y = 50;
			object->piece[6].x = 11;
			object->piece[6].y = 51;
			object->piece[7].x = 11;
			object->piece[7].y = 52;
			object->piece[8].x = 12;
			object->piece[8].y = 51;
			object->piece[9].x = 13;
			object->piece[9].y = 50;
			break;
	}
}

// Sets all the bits needed to use the buttons and the timer.
// Enable interrupts
void init(void){
	TRISDSET = 0x00e0;					// Set TRISED (PORTD) bit 7-5 as input (button 2-4)
	TRISFSET = 0x0002;					// Set TRISEF (PORTF) bit 5 as input (button 1)
	T2CON = 0x0;						// Resetts timern
	T2CON = 0x70;						// set 1:256 prescale
	PR2 = (80000000 / 256) / 50;		// gives us 50 timeouts / sec
	IPC(2) = 7;							// Prio 7
	IFS(0) = 0;
	IEC(0) = 0x100;						// Enable interrupt for timer 2
	TRISECLR = 0x0ff;					// Sets TRISE (PORTE) bit 7-0 as output (LED 7-0)
	asm volatile("ei");					// enable interuppt on chipkit
}

// Returns the value of buttons pressed. BTN 1 = bit 0, BTN 2 = bit 1, BTN 3 = bit 2, BTN 4 = bit 3
int getbtns(void)
{
  int status = PORTD & 0x00000e0;	// Masks bit 7-5 connected to button 2-4
  status = (status >> 4);			// Shifts them to bit 3-1
  int buff = PORTF & 0x00000002;	// Masks bit 2 connected to button 1
  buff = (buff >> 1);				// shifts to bit 1
  status = status | buff;			// combine them
  return status;					// returns which buttons are pressed
}

// Forces you to let go of the buttons and pressing one again to move on
void release_press (void){
	volatile int button = getbtns();
	while (button != 0){
		button = getbtns();
	}
	while (button == 0){
		button = getbtns();
	}
}
