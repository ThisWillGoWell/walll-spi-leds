

#include "pattern.h"
#include "random.h"
#include "colors.h"
#include "stdbool.h"

uint8_t tops[NUM_TUBES] = {0};
uint8_t positions[NUM_TUBES] = {0};
uint8_t size[NUM_TUBES] = {0};
HsvColor colors[NUM_TUBES];

bool flashing[NUM_TUBES];

HsvColor off = {0, 0, 0};
#define MAX_SIZE 10
#define MIN_SIZE 4

uint32_t lastCount;

void LineTettris(uint32_t count ){
	if(count - 1 != lastCount){
		for(int i=0;i<NUM_TUBES;i++){
			tops[i] = 0;
			positions[i] = 0;
			flashing[i] = false;
		}
	}
	lastCount = count;
	if(count % 10 != 0){ // slow down by a factor of 10
		return;
	}
	for(int i=0;i<NUM_TUBES;i++){

		if(flashing[i]){ // are we flashing
			if(colors[i].v >= 10){ // any room left to flash
				colors[i].v -= 10;
				for(int j =0;j<LEDS_PER_TUBE;j++){ // set them all the lower flash
					setLedHsv(i, j, colors[i]);
				}

			}else{ // turn off flash and then reset top to bottom
				for(int j =0;j<LEDS_PER_TUBE;j++){
					setLedHsv(i, j, off);
				}
				tops[i] = 0;
				flashing[i] = false;
			}
		}
		else if(positions[i] - 1 <= tops[i]) { //is the next position filled?
			tops[i] += size[i]; // set new top

			// did we fill the top
			if(tops[i] >= LEDS_PER_TUBE -1 ){
				flashing[i] = true;
				colors[i].s = 0;
				colors[i].v = 100;
				for(int j =0;j<LEDS_PER_TUBE;j++){ // set them all the  flash
					setLedHsv(i, j, colors[i]);
				}
			} else {
				size[i] = get_rand_uint8_range(MIN_SIZE, MAX_SIZE); // set new size
				positions[i] = LEDS_PER_TUBE; // reset position
				// set new color
				colors[i].v = 100;
				colors[i].h = get_rand_uint8_range(0, 255);
				colors[i].s = 255;
			}

		} else{
			positions[i] -= 1;
			//turn off the led
			if(positions[i] + size[i] < LEDS_PER_TUBE ){
				setLedHsv(i, positions[i] + size[i], off);
			}
			setLedHsv(i, positions[i], colors[i]);
		}


	}
}


