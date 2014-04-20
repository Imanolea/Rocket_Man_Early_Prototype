#include "data.c"

/* engine of Rocket Man */

typedef enum {false, true} bool;

const int CHANGEFRAME = 3; // frame of the change
const int JUMPFRAME = 4; // frame of the jump
const int ROCKETFRAME = 5; // frame of the rockets
const int FRAMESLENGTH = 12; // number of total frames

const int PERFRAME = 34; // milliseconds per iteration (34)
const int NORMALSPEED = 2; // speed of the sprite without pressing b
const int POWEREDSPEED = 3; // speed of the sprite pressing b
const int NORMALJUMPHEIGHT = 8; // jump height with normal speed
const int POWEREDJUMPHEIGHT = 9; // jump height with powered speed
const int MAXVERTICALSP = 8; // max speed in freefall
const int FRAMETIME = 3; // number of iterations per frame
const int ANIMATIONFRAMES = 4; // frames per animation
const int DIRCHTIME = 3; // time that takes to change of direction
const int INITIALFUEL = 20; // initial fuel available for the rockets

const int GRAVITYF = 8; // gravity of the game (simulates a decimal number)
const int GRAVDECLEFT = 10; // GRAVITYF/GRAVDECLEFT equals real value of GRAVITY

struct player {
	int x; // coordinate x of the sprite
	int y; // coordinate y of the sprite
	int movspeed; // speed of movement
	int framechangef; // displacement before changing frame (simulates a decimal number)
	int tchdir; // time taken to change the direction
	int fuel; // fuel consumed by the rockets
	int framef; // actual frame line (simulates a decimal number)
};

static struct player rman = {15, 128, NORMALSPEED, FRAMETIME, DIRCHTIME, 
	0, FRAMETIME * FRAMEDECIMALS - 1};

const int FRAMECHDECIMALS = 100; // decimal presision for framechangef
const int FRAMEDECIMALS = 100; // decimal precision for framef

/* relative x position for each half (16x8) of the sprite (16x16) */
unsigned char orientation[] = {
	0, 8, NULL
};

/* frame position for the animations */
unsigned char animation[] = {
	0, 2, 0, 1, 3, 4, 5, NULL
};

int disx = 0; // x displacement of the sprite in space
int disy = 0; // y displacement of the sprite in space

int disjumpf = 0; // value of the vertical vector of the jump (simulates a decimal number)
const int DISJUMPDECIMALS = 100; // decimal precision for disjumpf

int jumpdir = 0; // direction of the jump

bool apressed = false; // wether the a button is pressed (true) or not (false)
bool inair = false; // wether the sprite is in the air (true) or not (false)
bool hasflied = false; // wether the sprite has flied (true) or not (false)

/* process the movement while the sprite is in the air */
void jumping() {

	disy = 0;

	if (disjumpf/DISJUMPDECIMALS < MAXVERTICALSP) {
		disjumpf += (GRAVITYF * DISJUMPDECIMALS) / GRAVDECLEFT;
	} else {
		disjumpf = MAXVERTICALSP * DISJUMPDECIMALS;
	}


	rman.y += disjumpf/DISJUMPDECIMALS;
	if (rman.y >= 128) {
		rman.y = 128;
		inair = 0;
		disjumpf = 0;
		rman.framef = FRAMETIME * FRAMEDECIMALS - 1;
		hasflied = false;
		rman.fuel = 0;
	}
	rman.y -= disjumpf/DISJUMPDECIMALS;

	disy = disjumpf/DISJUMPDECIMALS;
}

/* do the changes in relation with the sprite */
void processsprites() {
	if (rman.fuel > 0)
		rman.fuel++;

	if (rman.fuel >= INITIALFUEL) {
		rman.fuel = 0;
		disjumpf = 0;
	}

	if (inair && !rman.fuel)
		jumping();

	// compiler's fault
	if (disx >= 0)
		rman.x = rman.x + disx;
	else
		rman.x = rman.x - (-disx);

	if (disy >= 0)
		rman.y = rman.y + disy;
	else
		rman.y = rman.y - (-disy);
}

/* displays the background */
void loadbkg() {
	int i;

	set_bkg_data(1, 2, bkgdata);  

	for(i = 0 ; i < 20 ; i++) {
		set_bkg_tiles(i, 16, 1, 1, ground0);
		set_bkg_tiles(i, 17, 1, 1, ground1);
	}

	SHOW_BKG;
}

/* displays the sprite */
void drawsprites() {

	set_sprite_tile(0, animation[rman.framef/FRAMEDECIMALS] * 2);
	set_sprite_tile(1, FRAMESLENGTH + animation[rman.framef/FRAMEDECIMALS] * 2);

	move_sprite(0, rman.x + orientation[0], rman.y);
	move_sprite(1, rman.x + orientation[1], rman.y);
}

/* loads the sprite data */
void loadsprite() {

	SPRITES_8x16;

	set_sprite_data(0, FRAMESLENGTH * 2, rocketmandata);

	drawsprites();

	SHOW_SPRITES;
}

/* draws the sprite out of the screen */
void moveoutsp() {

	move_sprite(0, 200, 200);
	move_sprite(1, 200, 200);
}

/* sets the sprite orientation */
void setorientation(bool left) {

	if (left) {
		orientation[0] = 8;
		orientation[1] = 0;
		moveoutsp();
		set_sprite_prop(0, S_FLIPX | 0x01);
		set_sprite_prop(1, S_FLIPX | 0x01);
		drawsprites();
		if (disx > 0)
			rman.tchdir = 0;
	} else {
		orientation[0] = 0;
		orientation[1] = 8;
		moveoutsp();
		set_sprite_prop(0, 0);
		set_sprite_prop(1, 0);
		drawsprites();
		if (disx < 0)
			rman.tchdir = 0;
	}
	jumpdir = 0;
	disx = 0;
	rman.framef = FRAMETIME * FRAMEDECIMALS - 1;
}

/* changes the frames of the sprite */
void animate() {

	if (rman.fuel) {
		if (rman.framef/FRAMEDECIMALS == ROCKETFRAME)
			rman.framef = (ROCKETFRAME + 1) * FRAMEDECIMALS;
		else
			rman.framef = ROCKETFRAME * FRAMEDECIMALS;
	} else if (inair)
		rman.framef = JUMPFRAME * FRAMEDECIMALS;
	else if (rman.tchdir < DIRCHTIME)
		rman.framef = CHANGEFRAME * FRAMEDECIMALS;
	else {
		rman.framef += (FRAMEDECIMALS * FRAMECHDECIMALS) / rman.framechangef;
    	if (rman.framef/FRAMEDECIMALS >= ANIMATIONFRAMES) rman.framef = 0;
	}

}

/* process the input */
void processinput(bool* keys) {

	if (keys[0]) {
		if (!orientation[0])
			setorientation(1);

		if (rman.tchdir < DIRCHTIME) {
			rman.tchdir++;
		} else {
			disx = - rman.movspeed;
		}
	}

	if (keys[1]) {
		if (orientation[0])
			setorientation(0);	

		if (rman.tchdir < DIRCHTIME) {
			rman.tchdir++;
		} else {
			disx = rman.movspeed;
		}
	}

	if (keys[4]) {
		if (!apressed) {
			if (hasflied && rman.fuel) { // condition to stop the rockets
				rman.fuel = 0;
				disjumpf = 0;
			}

			if (inair && !rman.fuel && !hasflied) { // condition to activate the rockets
				hasflied = true;
				rman.fuel++;
				rman.framef = ROCKETFRAME * FRAMEDECIMALS;
				disy = 0;
			}

			if (!inair) { // condition to jump
				rman.y--;
				inair = 1;
				jumpdir = disx;
				/* sets the power of the jump */
				if (abs(disx) <= NORMALSPEED) {
					disjumpf = (-NORMALJUMPHEIGHT);
					disjumpf = disjumpf * DISJUMPDECIMALS;
				} else {
					disjumpf = (-POWEREDJUMPHEIGHT);
					disjumpf = disjumpf * DISJUMPDECIMALS;
				}
			}
		}

		apressed = 1;
	} else {
		apressed = 0;

		/* stops the jumps ascension */
		if (disjumpf < 0) {
			disjumpf = (-(GRAVITYF * DISJUMPDECIMALS)) / GRAVDECLEFT;
		}
		if (rman.fuel >= INITIALFUEL) {
			rman.fuel = 0;
		}
	}

	// registers the b button only if the sprite it is not in the air
	if (!inair)
		if (keys[5]) {
			rman.movspeed = POWEREDSPEED;  
			rman.framechangef = (FRAMETIME * FRAMECHDECIMALS * NORMALSPEED) / POWEREDSPEED;
		} else {
			rman.movspeed = NORMALSPEED;
			rman.framechangef = FRAMETIME * FRAMECHDECIMALS;
		}


	if (!(keys[0] || keys[1])) {
		if (!inair) {
			rman.framef = FRAMETIME * FRAMEDECIMALS - 1;
			reduceabs((&disx), 1);
		} else {
			if (jumpdir > 0)
				disx = 1;
			else if (jumpdir < 0)
				disx = -1;
			else {
				reduceabs((&disx), 1);
			}
			animate();
		}
	} else
		animate();

}
