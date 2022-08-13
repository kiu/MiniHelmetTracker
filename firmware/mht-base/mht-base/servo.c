/*
* servo.c
*
* Created: 17.07.2022 13:56:23
*  Author: kiu
*/

#include <atmel_start.h>
#include <util/delay.h>
#include <stdio.h>
#include "config.h"
#include "servo.h"
#include "panic.h"

void servo_set(uint8_t s1, uint8_t s2) {
	if (!SERVO_ENABLE) {
		return;
	}
	TCB0.CCMPH = s1;
	TCB1.CCMPH = s2;
	_delay_ms(SERVO_DELAY);
}

void servo_init() {
	servo_change(SERVO_DOWN);
	if (SERVO_ENABLE) {
		TCB0.CTRLA |= TCB_ENABLE_bm;
		TCB1.CTRLA |= TCB_ENABLE_bm;
	}
	servo_change(SERVO_DOWN);
}

void servo_change(servo_configuration sc) {
	if (sc == SERVO_DOWN) {
		printf("SERVO Moving to: SERVO_DOWN\n");
		servo_set(SERVO_PLATE_DOWN, SERVO_HORN_UP);
		return;
	}
	if (sc == SERVO_MIDDLE_OPEN) {
		printf("SERVO Moving to: SERVO_MIDDLE_OPEN\n");
		servo_set(SERVO_PLATE_MIDDLE, SERVO_HORN_UP);
		return;
	}
	if (sc == SERVO_MIDDLE_CLOSED) {
		printf("SERVO Moving to: SERVO_MIDDLE_CLOSED\n");
		servo_set(SERVO_PLATE_MIDDLE, SERVO_HORN_DOWN);
		return;
	}
	if (sc == SERVO_UP) {
		printf("SERVO Moving servo: to SERVO_UP\n");
		servo_set(SERVO_PLATE_UP, SERVO_HORN_DOWN);
		return;
	}
	panic("SERVO Unknown command.");
}

void servo_shutdown() {
	servo_change(SERVO_DOWN);
	_delay_ms(1000);
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
	TCB1.CTRLA &= ~TCB_ENABLE_bm;
}
