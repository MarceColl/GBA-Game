

#define CLR_DARK_WATER 1
#define CLR_DARK_WATER 


#include <tonc.h>
#include "all_gfx.h"



void test_palette() {
	int i = 0;

	for(i = 1; i <= tilesetPalLen; ++i) {
		pal_bg_mem[i] = 5*i;
	}
}

void iterate_colors(int indices[], int size) {
	COLOR prev = pal_bg_mem[indices[size - 1]]; 

	for(int i = 0; i < size; ++i) {
		prev = pal_bg_mem[indices[i]];
		pal_bg_mem[indices[i]] = prev;	
	}
}
