#include <stdio.h>
#include <tonc.h>
#include <string.h>
#include "map.h"
#include "tileset.h"
#include "square-sprites.h"
#include "color.h"


#define BOOL u8
#define TRUE 1
#define FALSE 0

#define forever while(TRUE)

// VIDEO DEFINITIONS
#define WIDTH 240
#define HEIGHT 160


// CURSOR DEFINITIONS
#define CURSOR_MOVEMENT 16
#define CURSOR_DELAY 4 
#define CURSOR_EDGE_SCREEN_X WIDTH/CURSOR_MOVEMENT 
#define CURSOR_EDGE_SCREEN_Y HEIGHT/CURSOR_MOVEMENT 


#define SPRITES_BEFORE_UNITS 8
#define NUM_UNITS 6
#define CURSOR_UNIT_OFFSET_Y 12
#define SHADOW_OFFSET 8


// OBJECT INDEXES
#define POBJ_INDEX(id) (&obj_buffer[id])
#define OBJ_CURSOR 	POBJ_INDEX(0)
#define OBJ_HP_BAR1 	POBJ_INDEX(1)
#define OBJ_HP_BAR2 	POBJ_INDEX(2)
#define OBJ_HP_BAR3 	POBJ_INDEX(3)
#define OBJ_HP_BAR4 	POBJ_INDEX(4)
#define OBJ_HP_BAR5 	POBJ_INDEX(5)
#define OBJ_HP_BAR6 	POBJ_INDEX(6)
#define OBJ_TOOLTIP 	POBJ_INDEX(7)


// SPRITE TILE INDEXES
#define SPRITE_TILE(index) 8*index

#define TILE_CURSOR 		SPRITE_TILE(0)
#define TILE_SHADOW 		SPRITE_TILE(1)
#define TILE_BLUE_ARCHER 	SPRITE_TILE(2)
#define TILE_BLUE_SWORDSMAN 	SPRITE_TILE(4)
#define TILE_ORANGE_ARCHER 	SPRITE_TILE(3)
#define TILE_ORANGE_SWORDSMAN 	SPRITE_TILE(5) 
#define TILE_TOOLTIP 		SPRITE_TILE(6)
#define TILE_HP_BAR 		SPRITE_TILE(7)
#define TILE_HP_BAR_L 		SPRITE_TILE(8)


// TIMING MACROS
#define EVERY_X_FRAMES(frames) if(frame%frames == 0)
#define EVERY_20_FRAMES EVERY_X_FRAMES(20)
#define EVERY_10_FRAMES EVERY_X_FRAMES(10)


// PALETTE
#define CURSOR_COLOR_INDEX 11


typedef enum SCROLL_DIR {
	RIGHT,
	LEFT,
	DOWN,
	UP,
	NONE
} scrolldir;

typedef enum TEAM {
	BLUE,
	ORANGE
} Team;

typedef struct MAP_POS {
	int x;
	int y;
} mappos;

typedef struct UNIT {
	int index;
	Team team;
	int movements_left;
	int x;
	int y;
	int hp;

	BOOL placed;
} Unit;


OBJ_ATTR obj_buffer[128];
Unit units[NUM_UNITS];

int mapx;
int mapy;

u16 cursor_color;

void init() {
	// Init interrupts and VBlank irq.
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	REG_BG0CNT = BG_CBB(0) | BG_SBB(20) | BG_8BPP | BG_REG_32x32 | BG_PRIO(2);
	REG_BG1CNT = BG_CBB(0) | BG_SBB(25) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1);
	REG_BG2CNT = BG_CBB(0) | BG_SBB(30) | BG_8BPP | BG_REG_32x32 | BG_PRIO(0);

	// Video mode 0, enable bg 0, 1, 2. Enable object rendering, 1D layout in memory.
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

	// Load palette
	memcpy(pal_bg_mem, tilesetPal, tilesetPalLen);
	memcpy(pal_obj_mem, square_spritesPal, square_spritesPalLen);

	// Load tiles into CBB 0
	memcpy(&tile8_mem[0][0], tilesetTiles, tilesetTilesLen);
	// Load square sprites in CBB 4
	memcpy(&tile8_mem[4][0], square_spritesTiles, square_spritesTilesLen);

	// Load map into SBB 30
	memcpy16(&se_mem[20][0], bg0, bg0Len);
	memcpy16(&se_mem[25][0], bg1, bg1Len);
	memcpy16(&se_mem[30][0], bg2, bg2Len);

	oam_init(obj_buffer, 128);

	cursor_color = pal_obj_mem[CURSOR_COLOR_INDEX];
}


void shift_units(int dx, int dy) {
	for(int i = 0; i < NUM_UNITS; ++i) {
		if(units[i].placed == FALSE)
			continue;

		OBJ_ATTR *obj = &obj_buffer[units[i].index];
		OBJ_ATTR *shdw_obj = &obj_buffer[units[i].index + 1];

		int x = units[i].x;
		int y = units[i].y; 

		x = x+dx;
		y = y+dy;

		units[i].x = x;
		units[i].y = y;

		int shdw_y = y + SHADOW_OFFSET;

		BFN_SET(obj->attr1, x, ATTR1_X);
		BFN_SET(obj->attr0, y, ATTR0_Y);

		BFN_SET(shdw_obj->attr1, x, ATTR1_X);
		BFN_SET(shdw_obj->attr0, shdw_y, ATTR0_Y);
	}
}

void move_map_to(int VSCR, int HSCR) {
	static int prev_VSCR = 0;
	static int prev_HSCR = 0;

	int dVSCR = prev_VSCR - VSCR;
	int dHSCR = prev_HSCR - HSCR;

	shift_units(dHSCR, dVSCR);

	REG_BG0VOFS = VSCR;
	REG_BG1VOFS = VSCR;
	REG_BG2VOFS = VSCR;

	REG_BG0HOFS = HSCR;
	REG_BG1HOFS = HSCR;
	REG_BG2HOFS = HSCR;

	prev_VSCR = VSCR;
	prev_HSCR = HSCR;

	mapx = HSCR;
	mapy = VSCR;
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


BOOL valid_tile(int x, int y) {
	int realx = (mapx + x) >> 3;	
	int realy = (mapy + y) >> 3;

	if(bg0[realy][realx] == 21)
		return TRUE;

	return FALSE;
}


// Move zee cursor according to the laws of nature
void cursor_movement() {
	static volatile int ccursor_x = 2;
	static volatile int ccursor_y = 2;
	static int cursor_timeout = CURSOR_DELAY + 1;

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

	if(valid_tile(ccursor_x*16, ccursor_y*16) == FALSE) {
		pal_obj_mem[CURSOR_COLOR_INDEX] = 123;
	}
	else {
		pal_obj_mem[CURSOR_COLOR_INDEX] = cursor_color;
	}

	cursor_timeout++;
}


void animate_water() {
	u16 col1 = pal_bg_mem[3];
	u16 col2 = pal_bg_mem[7];
	u16 col3 = pal_bg_mem[11];
	pal_bg_mem[3] = col3;
	pal_bg_mem[7] = col1;
	pal_bg_mem[11] = col2;
}


void animate_units() {
	static int animation_status = 0;
	static int dir = 1;
	
	if(animation_status == 0) {
		dir = -1;
	} else if(animation_status == -2) {
		dir = 1;
	}

	animation_status += dir;

	for(int i = 0; i < NUM_UNITS; ++i) {
		if(units[i].placed == FALSE)
			continue;

		int x = units[i].x;
		int y = units[i].y; 

		obj_set_pos(&obj_buffer[units[i].index], x, y + animation_status);
	}
}


void move_unit(int u_idx, int x, int y) {
	units[u_idx].x = 16;	
	units[u_idx].y = 16;	
}

void get_cursor_position(int *a, int *b) {
	*a = BFN_GET(OBJ_CURSOR->attr1, ATTR1_X);
	*b = BFN_GET(OBJ_CURSOR->attr0, ATTR0_Y);
}

void add_unit(int type, int x, int y) {
	static int added_units = 0;

	units[added_units].index = added_units*2 + SPRITES_BEFORE_UNITS;	
	units[added_units].x = x;
	units[added_units].y = y;

	if(type == TILE_BLUE_ARCHER || type == TILE_BLUE_SWORDSMAN) {
		units[added_units].team = BLUE;
	}
	else {
		units[added_units].team = ORANGE;
	}

	if(type == TILE_BLUE_ARCHER || type == TILE_ORANGE_ARCHER) {
		units[added_units].hp = 3;
	}
	else {
		units[added_units].hp = 6;
	}

	units[added_units].placed = TRUE;

	obj_set_attr(&obj_buffer[units[added_units].index], ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, type | ATTR2_PRIO(0));
	obj_set_pos(&obj_buffer[units[added_units].index], x, y);
	
	obj_set_attr(&obj_buffer[units[added_units].index + 1], ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, TILE_SHADOW | ATTR2_PRIO(1));
	obj_set_pos(&obj_buffer[units[added_units].index + 1], x, y + SHADOW_OFFSET);

	added_units++;
}

BOOL placing() {
	const Team team_order[NUM_UNITS] = { ORANGE, BLUE, BLUE, ORANGE, ORANGE, BLUE };
	const int unit_order[NUM_UNITS] = { TILE_ORANGE_ARCHER, TILE_BLUE_ARCHER, TILE_BLUE_SWORDSMAN, TILE_ORANGE_SWORDSMAN, TILE_ORANGE_SWORDSMAN, TILE_BLUE_SWORDSMAN };
	static int stage = 0;

	if(key_hit(KEY_A)) {
		int a, b;
		get_cursor_position(&a, &b);

		if(!valid_tile(a, b))
			return TRUE;

		b -= CURSOR_UNIT_OFFSET_Y;

		add_unit(unit_order[stage], a, b);
		stage++;
	}

	if(stage == NUM_UNITS)
		return FALSE;

	return TRUE;
}

int get_unit_at(int x, int y) {
	for(int i = 0; i < NUM_UNITS; i++) {
		if(units[i].x == x && units[i].y + CURSOR_UNIT_OFFSET_Y == y)
			return i;
	}

	return -1;
}

void show_tooltip(u_idx) {
	OBJ_ATTR* health_bar[6] = { OBJ_HP_BAR1, OBJ_HP_BAR2, OBJ_HP_BAR3, OBJ_HP_BAR4, OBJ_HP_BAR5, OBJ_HP_BAR6 };

	int hp = units[u_idx].hp;
	int curx, cury;

	get_cursor_position(&curx, &cury);

	int tooltipx = curx + 16;
	int tooltipy = cury - 16 - CURSOR_UNIT_OFFSET_Y;

	
	BFN_SET(OBJ_TOOLTIP->attr1, tooltipx, ATTR1_X);	
	BFN_SET(OBJ_TOOLTIP->attr0, tooltipy, ATTR0_Y);	

	obj_unhide(OBJ_TOOLTIP, 0);

	for(int i = 0; i < 6 && i < hp; ++i) {
		BFN_SET(health_bar[i]->attr1, tooltipx + 2 + 2*i, ATTR1_X);
		BFN_SET(health_bar[i]->attr0, tooltipy + 2, ATTR0_Y);
		obj_unhide(health_bar[i], 0);
	}
}

void hide_tooltip() {
	obj_hide(OBJ_TOOLTIP);
	obj_hide(OBJ_HP_BAR1);
	obj_hide(OBJ_HP_BAR2);
	obj_hide(OBJ_HP_BAR3);
	obj_hide(OBJ_HP_BAR4);
	obj_hide(OBJ_HP_BAR5);
	obj_hide(OBJ_HP_BAR6);
}

void show_movements(u_idx) {
		
}

void fight() {
	int curx, cury;

	get_cursor_position(&curx, &cury);
	int u_idx = get_unit_at(curx, cury);	

	if(u_idx == -1) {
		hide_tooltip();
		return;
	}

	show_tooltip(u_idx);

	if(key_hit(KEY_A)) {
		show_movements(u_idx);
	}
}

void init_objects() {
	oam_init(obj_buffer, 128);
	
	// Initialize cursor
	obj_set_attr(OBJ_CURSOR, ATTR0_SQUARE | ATTR0_8BPP , ATTR1_SIZE_16, TILE_CURSOR | ATTR2_PRIO(1));
	obj_set_attr(OBJ_TOOLTIP, ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, TILE_TOOLTIP | ATTR2_PRIO(0));
	obj_set_attr(OBJ_HP_BAR1, ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, TILE_HP_BAR_L | ATTR2_PRIO(0));
	obj_set_attr(OBJ_HP_BAR2, ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, TILE_HP_BAR | ATTR2_PRIO(0));
	obj_set_attr(OBJ_HP_BAR3, ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, TILE_HP_BAR | ATTR2_PRIO(0));
	obj_set_attr(OBJ_HP_BAR4, ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, TILE_HP_BAR | ATTR2_PRIO(0));
	obj_set_attr(OBJ_HP_BAR5, ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, TILE_HP_BAR | ATTR2_PRIO(0));
	obj_set_attr(OBJ_HP_BAR6, ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16, TILE_HP_BAR | ATTR2_PRIO(0));

	hide_tooltip();
}


int main() {
	init();
	init_objects();

	move_map_to(0, 0);

	BOOL isPlacement = TRUE;

	int frame = 0;
	forever {
		vid_vsync();
		VBlankIntrWait();
		key_poll();

		// At the beginning of the game it's placement mode.
		// In placement mode the players take turns to place their units
		if(isPlacement == TRUE) {
			isPlacement = placing();
		}
		else {
			fight();
		}

		EVERY_10_FRAMES {
			animate_units();
		}

		EVERY_20_FRAMES {
			animate_water();
		}

		// Cursor movement
		cursor_movement();

		// Copy temporal oam memory to oam_memory
		oam_copy(oam_mem, obj_buffer, NUM_UNITS*2 + SPRITES_BEFORE_UNITS);

		frame++;
	}

	return 0;
}
