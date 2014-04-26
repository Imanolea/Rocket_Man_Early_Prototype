#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gb/console.h>
#include "utils.c"
#include "engine.c"

const int PERFRAME = 34; // milliseconds per iteration (34)

/* keys pressed (true) or not (false)
0: LEFT
1: RIGHT
2: UP
3: DOWN
4: A
5: B
6: START
7: SELECT
 */
unsigned bool keys[] = {
	false, false, false, false, false, false, false, NULL
};

/* gets the keys input */
void recordinput(int key) {

	if (key & J_LEFT) {
		keys[0] = true;
	} else {
		keys[0] = false;
	}

	if (key & J_RIGHT) {
		keys[1] = true;
	} else {
		keys[1] = false;
	}

	if (key & J_A) {
		keys[4] = true;
	} else {
		keys[4] = false;
	}

	if (key & J_B) {
		keys[5] = true;
	} else {
		keys[5] = false;
	}

	processinput(keys);
}

/* process the movement in the sprite */
void process() {

	processsprites();
}

/* main program */
void main() {

	loadbkg();

	loadsprite();

	/* main loop */
	while (TRUE) {

		recordinput(joypad());

		drawsprites();

		process(); 

		delay(PERFRAME);

	}
}