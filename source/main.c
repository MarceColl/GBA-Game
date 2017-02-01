#include <stdio.h>
#include <tonc.h>
#include <string.h>
#include "map.h"
#include "all_gfx.h"
#include "color.h"


void init() {
	// Init interrupts and VBlank irq.
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	// Video mode 0, enable bg 0.
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;
	// set up BG0 for a 4bpp 64x32t map, using
	//   using charblock 0 and screenblock 31
	REG_BG0CNT= BG_CBB(0) | BG_SBB(30) | BG_8BPP | BG_REG_32x32;

	// Load palette
	memcpy(pal_bg_mem, tilesetPal, tilesetPalLen);
	// Load tiles into CBB 0
	memcpy(&tile_mem[0][0], tilesetTiles, tilesetTilesLen);
	// Load map into SBB 30
	memcpy16(&se_mem[30][0], map, mapLen);
}


int main() {
	init();

	test_palette();

	int frame = 0;
	while(1)
	{
		VBlankIntrWait();
		key_poll();



		frame++;
	}

	return 0;
}
