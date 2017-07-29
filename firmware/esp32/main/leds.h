/*
 * leds.h
 *
 *  Created on: Jul 8, 2017
 *      Author: supersat
 */

#ifndef MAIN_LEDS_H_
#define MAIN_LEDS_H_

#include <stdint.h>

typedef struct {
	uint8_t ones:3;
	uint8_t brightness:5;
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} LEDFrame;

extern uint8_t ledBuf[32];
extern LEDFrame *ledFrames;

extern void led_init_udp();

#endif /* MAIN_LEDS_H_ */
