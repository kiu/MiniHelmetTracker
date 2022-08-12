/*
* stepper.c
*
* Created: 17.07.2022 14:48:04
*  Author: kiu
*/

#include <atmel_start.h>
#include <stdio.h>
#include <util/delay.h>
#include "include/atmel_start_pins.h"

#include "config.h"
#include "panic.h"
#include "stepper.h"

uint32_t stepper_pos = 0;
bool stepper_homed = false;

void stepper_single_step(stepper_direction sc, uint8_t speed) {
	if (STEPPER_ENABLE) {
		STEP_DIR_set_level(sc);
		STEP_STEP_set_level(true);
		_delay_us(STEPPER_SPEED_MIN * 10);
		for (uint8_t i = 0; i < speed; i++) {
			_delay_us(10);
		}
		
		STEP_STEP_set_level(false);
		_delay_us(STEPPER_SPEED_MIN * 10);
		for (uint8_t i = 0; i < speed; i++) {
			_delay_us(10);
		}
	}
	
	if (sc == STEPPER_RIGHT) {
		stepper_pos++;
		} else {
		stepper_pos--;
	}
}

void stepper_move(uint8_t slot) {
	printf("STEPPER Move to slot: %d\n", slot);
	if (!stepper_homed) {
		panic("STEPPER Can not move before being homed!\n");
	}

	if (slot > 34) {
		panic("STEPPER Asked to move stepper out of bounds!\n");
	}
	
	uint32_t target = (uint32_t)slot * (uint32_t)4000;
	
	if (target == stepper_pos) {
		return;
	}
	
	uint32_t distance = 0;
	stepper_direction sc = STEPPER_RIGHT;
	
	if (target > stepper_pos) {
		distance = target - stepper_pos;
		sc = STEPPER_RIGHT;
	}

	if (target < stepper_pos) {
		distance = stepper_pos - target;
		sc = STEPPER_LEFT;
	}
	
	if (STEPPER_ENABLE) {
		STEP_EN_set_level(false);
	}
	if (distance < STEPPER_RAMP * 2) {
		while (distance > 0) {
			stepper_single_step(sc, STEPPER_SPEED_SLOW);
			distance--;
		}
		
		} else {

		for (uint16_t i = STEPPER_RAMP; i > 0; i--) {
			stepper_single_step(sc, i / (uint16_t)STEPPER_SPEED_CHANGE);
			distance--;
		}
		uint32_t mid = distance - STEPPER_RAMP;
		for (uint32_t i = 0; i < mid; i++) {
			stepper_single_step(sc, STEPPER_SPEED_FAST);
			distance--;
		}
		for (uint16_t i = 0; i < STEPPER_RAMP; i++) {
			stepper_single_step(sc, i / (uint16_t)STEPPER_SPEED_CHANGE);
			distance--;
		}
	}
	STEP_EN_set_level(true);
}

void stepper_home() {
	printf("STEPPER Homing...\n");
	if (STEPPER_ENABLE) {
		STEP_EN_set_level(false);
	}
	
	printf("STEPPER 1/4 Finding home\n");
	while (STEP_STOP_get_level() && STEPPER_ENABLE) {
		stepper_single_step(STEPPER_LEFT, STEPPER_HOME_SPEED_FAST);
	}
	
	printf("STEPPER 2/4 Wiggle Room\n");
	for (uint16_t i = 0; i < STEPPER_WIGGLE; i++) {
		stepper_single_step(STEPPER_RIGHT, STEPPER_HOME_SPEED_SLOW);
	}

	printf("STEPPER 3/4 Slowly back to home\n");
	while (STEP_STOP_get_level() && STEPPER_ENABLE) {
		stepper_single_step(STEPPER_LEFT, STEPPER_HOME_SPEED_SLOW);
	}

	printf("STEPPER 4/4 Move initial offset\n");
	for (uint16_t i = 0; i < STEPPER_OFFSET; i++) {
		stepper_single_step(STEPPER_RIGHT, STEPPER_HOME_SPEED_FAST);
	}

	STEP_EN_set_level(true);
	stepper_pos = 0;
	stepper_homed = true;
	printf("STEPPER Homing completed.\n");
}

void stepper_shutdown() {
	stepper_move(0);
}