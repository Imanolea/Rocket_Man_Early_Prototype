#include "tiles.c"
#include "maps.c"

/* game boy consts */
const int WINDOWWIDTH = 160; // width of the game boy screen
const int WINDOWHEIGHT = 144; // height of the game boy screen

/* engine of Rocket Man */

typedef enum {false, true} bool;

const int CHANGEFRAME = 3; // frame of the change
const int JUMPFRAME = 4; // frame of the jump
const int ROCKETFRAME = 5; // frame of the rockets
const int FRAMESLENGTH = 12; // number of total frames

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
	int x; // x axis position of the sprite
	int y; // y axis position of the sprite
	int movspeed; // speed of movement
	int framechangef; // displacement before changing frame (simulates a decimal number)
	int tchdir; // time taken to change the direction
	int fuel; // fuel consumed by the rockets
	int framef; // actual frame line (simulates a decimal number)
};

const int SPWIDTH = 15; // sprite width
const int SPHEIGHT = 15; // sprite height

const int REALX = 8; // sprite x axis relative position
const int REALY = 16; // sprite y axis relative position
const int SPWSIZE = 4; // sprites pixels for each side that aren't part of the rigid body

static struct player rman = {0, 112, NORMALSPEED, FRAMETIME, DIRCHTIME, 
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

unsigned char map[1024];

int disx = 0; // x displacement of the sprite in space
int disy = 0; // y displacement of the sprite in space

int disjumpf = 0; // value of the vertical vector of the jump (simulates a decimal number)
const int DISJUMPDECIMALS = 100; // decimal precision for disjumpf

int jumpdir = 0; // direction of the jump

bool apressed = false; // wether the a button is pressed (true) or not (false)
bool inair = false; // wether the sprite is in the air (true) or not (false)
bool hasflied = false; // wether the sprite has flied (true) or not (false)

int scrollx = 0; // number of pixels displaced by the horizontal scroll
int scrolly = 0; // number of pixels displaced by the vertical scroll

/* process the movement while the sprite is in the air */
void jumping() {

	if (disjumpf/DISJUMPDECIMALS < MAXVERTICALSP) {
		disjumpf += (GRAVITYF * DISJUMPDECIMALS) / GRAVDECLEFT;
	} else {
		disjumpf = MAXVERTICALSP * DISJUMPDECIMALS;
	}

	disy = disjumpf/DISJUMPDECIMALS;
}

/* updates the value of the coordinates of the sprite in the map 
@param mapx1: part of map where the left end of the sprite is
@param mapx2: part of map where the right end of the sprite is
@param mapy1: part of map where the upper end of the sprite is
@param mapy2: part of map where the bottom end of the sprite is
@param mydisx: value of x axis displacement
@param mydisy: value of y axis displacement
*/
void mapcoordinates(int *mapx1, int *mapx2, int *mapy1, int *mapy2,
	int mydisx, int mydisy) {

	if (disx >= 0) {
		*mapx1 = (rman.x + mydisx + SPWSIZE)/8/MTESTSIZE + scrollx/8/MTESTSIZE;
		*mapx2 = (rman.x + mydisx + SPWIDTH - SPWSIZE)/8/MTESTSIZE + scrollx/8/MTESTSIZE;
	} else {
		*mapx1 = (rman.x - (-mydisx) + SPWSIZE)/8/MTESTSIZE + scrollx/8/MTESTSIZE;
		*mapx2 = (rman.x - (-mydisx) + SPWIDTH - SPWSIZE)/8/MTESTSIZE + scrollx/8/MTESTSIZE;
	}

	if (disy >= 0) {
		*mapy1 = (rman.y + mydisy)/8/MTESTSIZE;
		*mapy2 = (rman.y + mydisy + SPHEIGHT)/8/MTESTSIZE;
	} else {
		*mapy1 = (rman.y - (-mydisy))/8/MTESTSIZE;
		*mapy2 = (rman.y - (-mydisy) + SPHEIGHT)/8/MTESTSIZE;
	}

}

/* checks wether the sprite is colliding with a solid tile
@param mapx1: part of map where the left end of the sprite is
@param mapx2: part of map where the right end of the sprite is
@param mapy1: part of map where the upper end of the sprite is
@param mapy2: part of map where the bottom end of the sprite is
@return: wether the sprite is colliding (true) or not (false) */
bool collides(int mapx1, int mapx2, int mapy1, int mapy2) {
	int upleft, upright, downleft, downright, centerleft, centerright;


	if (mapy1 == 6) {
		upleft = mtest[257 + mapx1 - 17];
		upright = mtest[257 + mapx2 - 17];
		centerleft = mtest[(mapy1 + 1) * MTESTWIDTH + mapx1];
		centerright = mtest[(mapy1 + 1) * MTESTWIDTH + mapx2];
	} else if (mapy1 == 5) {
		upleft = mtest[mapy1 * MTESTWIDTH + mapx1];
		upright = mtest[mapy1 * MTESTWIDTH + mapx2];
		centerleft = mtest[257 + mapx1 - 17];
		centerright = mtest[257 + mapx2 - 17];
	} else if (mapy1 == 12) {
		upleft = mtest[513 + mapx1 - 33];
		upright = mtest[513 + mapx2 - 33];
		centerleft = mtest[(mapy1 + 1) * MTESTWIDTH + mapx1];
		centerright = mtest[(mapy1 + 1) * MTESTWIDTH + mapx2];
	} else if (mapy1 == 11) {
		upleft = mtest[mapy1 * MTESTWIDTH + mapx1];
		upright = mtest[mapy1 * MTESTWIDTH + mapx2];
		centerleft = mtest[513 + mapx1 - 33];
		centerright = mtest[513 + mapx2 - 33];
	} else {
		upleft = mtest[mapy1 * MTESTWIDTH + mapx1];
		upright = mtest[mapy1 * MTESTWIDTH + mapx2];
		centerleft = mtest[(mapy1 + 1) * MTESTWIDTH + mapx1];
		centerright = mtest[(mapy1 + 1) * MTESTWIDTH + mapx2];
	}

	if (mapy2 == 6) {
		downleft = mtest[257 + mapx1 - 17];
		downright = mtest[257 + mapx2 - 17];
	} else if (mapy2 == 12) {
		downleft = mtest[513 + mapx1 - 33];
		downright = mtest[513 + mapx2 - 33];
	} else {
		downleft = mtest[mapy2 * MTESTWIDTH + mapx1];
		downright = mtest[mapy2 * MTESTWIDTH + mapx2];
	}

	return (upleft >= 13 || upright >= 13 || downleft >= 13 || downright >= 13 || centerleft >= 13 || centerright >= 13);

}

/* checks if there is ground below the sprite */
void checkground() {
	int mapx1 = 0, mapx2 = 0, mapy1 = 0, mapy2 = 0;

	mapcoordinates(&mapx1, &mapx2, &mapy1, &mapy2, 0, 1);

	if (collides(mapx1, mapx2, mapy1, mapy2)) {
		inair = false;
		disjumpf = 0;
		hasflied = false;
		rman.fuel = 0;
	} else {
		inair = true;
	}
}


/* checks the collisons of the sprite 
@param mydisx: value of x axis displacement
@param mydisy: value of y axis displacement
*/
bool checkcollisions(int mydisx, int mydisy) {
	int mapx1 = 0, mapy1 = 0;
	int mapx2 = 0, mapy2 = 0;
	bool end = false;
	bool isdisx = true;
	bool isdisy = true;
	bool hascollide = false;

	if (!mydisx)
		isdisx = false;

	if (!mydisy)
		isdisy = false;

	do {
		mapcoordinates(&mapx1, &mapx2, &mapy1, &mapy2, mydisx, mydisy);
		
		if (collides(mapx1, mapx2, mapy1, mapy2)) {
			reduceabs(&mydisx, 1);
			reduceabs(&mydisy, 1);
			hascollide = true;
		} else {
			end = true;
		}

	} while (!end);

	if (isdisx)
		disx = mydisx;

	if (isdisy)
		disy = mydisy;

	return hascollide;

}

/* loads the map in the map char array
@param scrlx: number of pixels displaced by the horizontal scroll
@param scrly: number of pixels displaced by the vertical scroll
 */
void loadmap(int scrlx, int scrly) {
	
	set_bkg_tiles(0, 0, MTESTWIDTH, MTESTHEIGHT, &mtest[scrlx/8 + scrly * MTESTWIDTH]);
}

/* displays the background */
void loadbkg() {

	set_bkg_data(1, 17, bkgdata);

	loadmap(scrollx, scrolly);

	SHOW_BKG;
}

/* only draw the sprite, no frame setting */
void movesprites() {

	move_sprite(0, rman.x + orientation[0] + REALX, rman.y + REALY);
	move_sprite(1, rman.x + orientation[1] + REALX, rman.y + REALY);
}

/* displays the sprite */
void drawsprites() {

	set_sprite_tile(0, animation[rman.framef/FRAMEDECIMALS] * 2);
	set_sprite_tile(1, FRAMESLENGTH + animation[rman.framef/FRAMEDECIMALS] * 2);

	movesprites();
}

/* loads the sprite data */
void loadsprite() {

	SPRITES_8x16;

	set_sprite_data(0, FRAMESLENGTH * 2, rocketmandata);

	drawsprites();

	SHOW_SPRITES;
}

/* sets the sprite orientation 
@param left: dictates if the orientation to represent is left (true) or not (false) */
void setorientation(bool left) {

	if (left) {
		orientation[0] = 8;
		orientation[1] = 0;
		HIDE_SPRITES;
		set_sprite_prop(0, S_FLIPX | 0x01);
		set_sprite_prop(1, S_FLIPX | 0x01);
		movesprites();
		SHOW_SPRITES;
		if (disx > 0)
			rman.tchdir = 0;
	} else {
		orientation[0] = 0;
		orientation[1] = 8;
		HIDE_SPRITES;
		set_sprite_prop(0, 0);
		set_sprite_prop(1, 0);
		movesprites();
		SHOW_SPRITES;
		if (disx < 0)
			rman.tchdir = 0;
	}
	jumpdir = 0;
	disx = 0;
	rman.framef = FRAMETIME * FRAMEDECIMALS - 1;
}

/* does a large scroll without leting the player to move 
@param orientation: direction of the scroll, true: right, false: left */
void dramascroll(bool orientation) {
	int i;
	int desp;

	if (orientation)
		desp = 1;
	else
		desp = -1;
		

	for (i = 0; i < WINDOWWIDTH; i++) {

		// moves the background
		if (scrollx % 96 == 0) {
			scroll_bkg(-scrollx, -scrolly);
			loadmap(scrollx, scrolly);
		}

		scroll_bkg(desp, 0);
		delay(10);
		if (desp >= 0)
			scrollx += desp;
		else
			scrollx = scrollx - desp;

		// moves the sprite
		rman.x = rman.x - desp;
		movesprites();
		
	}
	
}

/* checks if the sprite is crossing a door
@param scrlx:  number of pixels displaced by the horizontal scroll
*/
void checkdoor(int scrlx) {

	if (rman.x > (WINDOWWIDTH + scrlx))
		dramascroll(true);
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


	checkcollisions(disx, 0);

	if (disx >= 0) {
		rman.x += disx;
	} else {
		rman.x -= (-disx);
	}
	
	if (checkcollisions(0, disy))
		disjumpf = 0;

	if (disy >= 0)
		rman.y += disy;
	else
		rman.y -= (-disy);

	checkground();
	checkdoor(scrollx);

	disx = 0;
	disy = 0;
	
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

/* process the input 
@param keys: array with a list of boolean values that dictates the keys pressed*/
void processinput(bool* keys) {

	if (keys[0]) { // left
		if (!orientation[0]) {
			setorientation(1);
		}

		if (rman.tchdir < DIRCHTIME) {
			rman.tchdir++;
		} else {
			disx = - rman.movspeed;
		}
	}

	if (keys[1]) { // right
		if (orientation[0]) {
			setorientation(0);
		}	

		if (rman.tchdir < DIRCHTIME) {
			rman.tchdir++;
		} else {
			disx = rman.movspeed;
		}
	}

	if (keys[4]) { // a
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
				inair = true;
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
		if (keys[5]) { // b
			rman.movspeed = POWEREDSPEED;  
			rman.framechangef = (FRAMETIME * FRAMECHDECIMALS * NORMALSPEED) / POWEREDSPEED;
		} else {
			rman.movspeed = NORMALSPEED;
			rman.framechangef = FRAMETIME * FRAMECHDECIMALS;
		}


	if (!(keys[0] || keys[1])) { // no left and no right
		if (!inair) {
			rman.framef = FRAMETIME * FRAMEDECIMALS - 1;
			abs((&disx), 1);
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
