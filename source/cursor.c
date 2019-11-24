
#include "random.h"
#include "pattern.h"
#include "cursor.h"
#include "stdbool.h"
#include "stdint.h"
#define NUM_CURSORS NUM_TUBES
uint8_t direction[NUM_CURSORS] = {0};
uint32_t stepsLeft[NUM_CURSORS] = {0};
uint32_t r[NUM_CURSORS]; // tube
HsvColor linecolors[NUM_CURSORS];

void cursor(uint32_t count){
	for(int i=0;i<NUM_CURSORS;i++){
		if(stepsLeft[i] == 0){ // reset if we are out of steps
			if(direction[i] == 0){
				direction[i] = 1;
			} else {
				direction[i] = 0;
			}
			linecolors[i].h = (linecolors[i].h + 10) % 255;
			linecolors[i].s = 255;
			linecolors[i].v = 100;
			stepsLeft[i] = get_rand_uint32_range(1, 200);
		}
		if(direction[i] == 0){
			if(r[i] == LEDS_PER_TUBE -1){
				r[i] = 0;
			} else{
				r[i] += 1;
			}
		} else if(direction[i] == 1){
			if(r[i] == 0){
				r[i] = LEDS_PER_TUBE -1;
			} else{
				r[i] -=1;
			}
		}
		stepsLeft[i] -= 1;
		setLedHsv(i, r[i], linecolors[i]);
	}
	fade_all_and_set(5);
}

