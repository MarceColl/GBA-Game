#include <stdio.h>
#include <tonc.h>
#include <string.h>
#include "map.h"
#include "units.h"
#include "tileset.h"
#include "square-sprites.h"
#include "color.h"


// VIDEO DEFINITIONS
#define WIDTH 240
#define HEIGHT 160


// CURSOR DEFINITIONS
#define CURSOR_MOVEMENT 16
#define CURSOR_DELAY 4 
#define CURSOR_EDGE_SCREEN_X WIDTH/CURSOR_MOVEMENT 
#define CURSOR_EDGE_SCREEN_Y HEIGHT/CURSOR_MOVEMENT 


// OBJECT INDEXES
#define POBJ_INDEX(id) &obj_buffer[id]
#define OBJ_CURSOR POBJ_INDEX(0)

#define TILE_AT_POS(_x, _y) bg0[_y][_x]


#define EVERY_X_FRAMES(frames) if(frame%frames == 0)
#define EVERY_20_FRAMES EVERY_X_FRAMES(20)


OBJ_ATTR obj_buffer[128];


typedef enum SCROLL_DIR {
	RIGHT,
	LEFT,
	DOWN,
	UP,
	NONE
} scrolldir;


typedef struct MAP_POS {
	int x;
	int y;
} mappos;


void init() {
	// Init interrupts and VBlank irq.
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	REG_BG0CNT = BG_CBB(0) | BG_SBB(25) | BG_8BPP | BG_REG_32x32 | BG_PRIO(2);
	REG_BG1CNT = BG_CBB(0) | BG_SBB(30) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1);
	REG_BG2CNT = BG_CBB(0) | BG_SBB(20) | BG_8BPP | BG_REG_32x32 | BG_PRIO(0);

	// Video mode 0, enable bg 0, 1, 2. Enable object rendering, 2D layout in memory.
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_2D;

	// Load palette
	memcpy(pal_bg_mem, tilesetPal, tilesetPalLen);
	memcpy(pal_obj_mem, unitsPal, unitsPalLen);
	memcpy(pal_obj_mem, square_spritesPal, square_spritesPalLen);

	// Load tiles into CBB 0
	memcpy(&tile8_mem[0][0], tilesetTiles, tilesetTilesLen);
	// Load units in memory CBB 4
	memcpy(&tile8_mem[5][0], unitsTiles, unitsTilesLen);
	// Load square sprites in CBB 5
	memcpy(&tile8_mem[4][0], square_spritesTiles, square_spritesTilesLen);

	// Load map into SBB 30
	memcpy16(&se_mem[25][0], bg0, bg0Len);
	memcpy16(&se_mem[30][0], bg1, bg1Len);
	memcpy16(&se_mem[20][0], bg2, bg2Len);

	oam_init(obj_buffer, 128);
}


void move_map_to(int VSCR, int HSCR) {
	REG_BG0VOFS = VSCR;
	REG_BG1VOFS = VSCR;
	REG_BG2VOFS = VSCR;

	REG_BG0HOFS = HSCR;
	REG_BG1HOFS = HSCR;
	REG_BG2HOFS = HSCR;
}


mappos move_map(scrolldir dir) {
	static int VSCR = 0;
	static int HSCR = 0;

	switch(dir) {
		case RIGHT:
			HSCR += CURSOR_MOVEMENT;
			break;
		case LEFT:
			HSCR -= CURSOR_MOVEMENT;
			break;
		case DOWN:
			VSCR += CURSOR_MOVEMENT;
			break;
		case UP:
			VSCR -= CURSOR_MOVEMENT;
			break;
	}


	move_map_to(VSCR, HSCR);
}


// Move zee cursor according to the laws of nature
void cursor_movement() {
	static volatile int ccursor_x = 2;
	static volatile int ccursor_y = 2;
	static int cursor_timeout = 16;

	static int mvx = 0;
	static int mvy = 0;

	if(cursor_timeout > CURSOR_DELAY) {
		obj_set_pos(OBJ_CURSOR, ccursor_x*CURSOR_MOVEMENT, ccursor_y*CURSOR_MOVEMENT);

		int vert_move = key_tri_vert();
		int horz_move = key_tri_horz();
		mvx = horz_move;
		mvy = vert_move;

		if(vert_move != 0 || horz_move != 0) {
			cursor_timeout = 0;
		}
		ccursor_y += vert_move; 
		ccursor_x += horz_move;
	} else {
		// Animation of the transition between positions
		int dx = -mvx*CURSOR_MOVEMENT/CURSOR_DELAY*(CURSOR_DELAY - cursor_timeout);
		int dy = -mvy*CURSOR_MOVEMENT/CURSOR_DELAY*(CURSOR_DELAY - cursor_timeout);
		obj_set_pos(OBJ_CURSOR, 
				ccursor_x*CURSOR_MOVEMENT + dx, 
				ccursor_y*CURSOR_MOVEMENT + dy);
	}

	if(ccursor_x == 0) {
		ccursor_x++;
		move_map(LEFT);
	}

	if(ccursor_x == CURSOR_EDGE_SCREEN_X - 1) {
		ccursor_x--;
		move_map(RIGHT);
	}

	if(ccursor_y == CURSOR_EDGE_SCREEN_Y - 1) {
		ccursor_y--;
		move_map(DOWN);
	}

	if(ccursor_y == 0) {
		ccursor_y++;
		move_map(UP);
	}

	mappos map_pos = move_map(NONE);

	if(TILE_AT_POS(map_pos.x + ccursor_x*16, map_pos.y + ccursor_y*16) == 21) {
		pal_obj_mem[1] = 123;
	}

	cursor_timeout++;
}


void animate_water() {
	u16 col1 = pal_bg_mem[3];
	u16 col2 = pal_bg_mem[11];
	u16 col3 = pal_bg_mem[7];
	pal_bg_mem[3] = col3;
	pal_bg_mem[11] = col1;
	pal_bg_mem[7] = col2;
}


void init_objects() {
	obj_set_attr(OBJ_CURSOR, ATTR0_SQUARE | ATTR0_8BPP , ATTR1_SIZE_16, 0);
}


int main() {
	init();
	init_objects();

	move_map_to(0, 0);

	int frame = 0;
	u16 color;

	while(1) {
		vid_vsync();
		VBlankIntrWait();
		key_poll();

		EVERY_20_FRAMES {
			animate_water();
		}

		// Cursor movement
		cursor_movement();

		// Copy temporal oam memory to oam_memory
		oam_copy(oam_mem, obj_buffer, 1);

		frame++;
	}

	return 0;
}
