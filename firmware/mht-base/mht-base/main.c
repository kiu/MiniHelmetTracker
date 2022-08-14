#include <atmel_start.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "config.h"
#include "helmet.h"
#include "led.h"
#include "main.h"
#include "panic.h"
#include "servo.h"
#include "stepper.h"
#include "wifi.h"

opstate state = INIT;

volatile opmode mode = NONE;
volatile opmode modenext = NONE;
volatile opmode modelast = NONE;

volatile uint32_t seconds = 0;
volatile uint8_t ticked = true;

volatile uint8_t b_on = 0x00;
volatile uint8_t b_blink = 0x00;

uint8_t int_next = NONE;
uint8_t int_flag = 0x00;

uint8_t clean = 0;

const uint8_t CLEAN_FLAG = 0x42;

bool change_led_auto_tick = false;
bool led_state = true;

// -------------------------------------------------------------------------------

void main_change_led_auto(bool value) {
	led_state = value;
	change_led_auto_tick = true;
}

void main_button_update(opmode on, opmode blink) {
	b_on = (1 << on) | (1 << POWEROFF);
	b_on |= (1 << blink);
	
	b_blink = (1 << blink);
}

void main_set_unclean() {
	clean = 0x00;
}

opmode main_mode_get() {
	return mode;
}

opmode main_mode_next_get() {
	return modenext;
}

void main_mode_next_set(opmode om) {
	modenext = om;
}

// -------------------------------------------------------------------------------

char* main_mode_to_string(opmode mode) {
	if (mode == DIVISION) {
		return "DIVISION";
	}
	if (mode == CONFERENCE) {
		return "CONFERENCE";
	}
	if (mode == LEAGUE) {
		return "LEAGUE";
	}
	if (mode == PLAYOFFS) {
		return "PLAYOFFS";
	}
	if (mode == POWEROFF) {
		return "POWEROFF";
	}
	if (mode == NONE) {
		return "NONE";
	}
	return "UNKNOWN";
}

char* main_clean_to_string() {
	if (clean == CLEAN_FLAG) {
		return "CLEAN";
		} else {
		return "UNCLEAN";
	}
}

// -------------------------------------------------------------------------------

void main_store_load() {
	uint8_t storage[37];
	FLASH_0_read_eeprom_block(0, storage, 37);
	clean = storage[35];
	if (clean == CLEAN_FLAG) {
		uint8_t hc[35];
		memcpy(hc, storage, 35);
		helmet_set_current(hc);
	}
	modenext = storage[36];
	if (modenext == POWEROFF) {
		modenext = NONE;
	}
}

void main_store_save(opmode om) {
	printf("MAIN Storing configuration: %s / %s.\n", main_mode_to_string(om), main_clean_to_string());
	uint8_t storage[37];
	if (clean == CLEAN_FLAG) {
		memcpy(storage, helmet_get_current(), 35);
	}
	storage[35] = clean;
	storage[36] = om;
	FLASH_0_write_eeprom_block(0, storage, 37);
}

// -------------------------------------------------------------------------------


void main_tick() {
	
	if (change_led_auto_tick) {
		led_set_auto(led_state);
		change_led_auto_tick = false;
	}

	if (state != SHUTDOWN && mode != modenext) {
		if (modenext != POWEROFF) {
			modelast = modenext;
		}
		
		mode = modenext;
		main_button_update(mode, NONE);
		printf("MAIN Switching to mode: %s\n", main_mode_to_string(mode));
		
		if (mode == DIVISION) {
			helmet_set_mode(DIVISION);
		}
		if (mode == CONFERENCE) {
			helmet_set_mode(CONFERENCE);
		}
		if (mode == LEAGUE) {
			helmet_set_mode(LEAGUE);
		}
		if (mode == PLAYOFFS) {
			helmet_set_mode(PLAYOFFS);
		}

		if (mode == NONE) {
			led_blank();
		}

		if (mode == POWEROFF) {
			main_button_update(POWEROFF, POWEROFF);
			state = SHUTDOWN;
		}
	}

	if (state == INIT) {
		state = IDLE;
		return;
	}

	if (state == IDLE) {
		if (seconds % WIFI_RESET == 0) {
			wifi_reset();
			return;
		}
		if (seconds % WIFI_UPDATE == 0) {
			wifi_update();
			helmet_set_mode(mode);
			return;
		}
		if (!helmet_issort()) {
			printf("MAIN Sorting is needed, lets start.\n");
			state = SORTING;
			clean = 0x00;
			main_store_save(mode);
			helmet_set_mode(mode);
			return;
		}
	}

	if (state == SORTING) {
		if (helmet_dosort()) {
			printf("MAIN Sorting is completed.\n");
			state = IDLE;
			clean = CLEAN_FLAG;
			main_store_save(mode);
			return;
		}
	}
	
	if (state == SHUTDOWN) {
		return;
	}
}

// -------------------------------------------------------------------------------

ISR(PORTC_PORT_vect) {
	int_next = NONE;
	int_flag = PORTC.INTFLAGS;
	PORTC.INTFLAGS = 0xFF;
	
	if (int_flag & 1 << 3) {
		int_next = PLAYOFFS;
	}

	modenext = int_next;
	if (state != INIT && mode != POWEROFF) {
		main_button_update(mode, modenext);
	}
}

ISR(PORTD_PORT_vect) {
	int_next = NONE;
	int_flag = PORTD.INTFLAGS;
	PORTD.INTFLAGS = 0xFF;
	
	if (int_flag & 1 << 1) {
		int_next = POWEROFF;
	}

	modenext = int_next;
	if (state != INIT && mode != POWEROFF) {
		main_button_update(mode, modenext);
	}
}

ISR(PORTF_PORT_vect) {
	int_next = NONE;
	int_flag = PORTF.INTFLAGS;
	PORTF.INTFLAGS = 0xFF;
	
	if (int_flag & 1 << 1) {
		int_next = LEAGUE;
	}
	if (int_flag & 1 << 3) {
		int_next = CONFERENCE;
	}
	if (int_flag & 1 << 5) {
		int_next = DIVISION;
	}
	
	modenext = int_next;
	if (state != INIT && mode != POWEROFF) {
		main_button_update(mode, modenext);
	}
}

ISR(RTC_PIT_vect) {
	RTC.PITINTFLAGS = RTC_PI_bm;
	
	uint8_t out = b_on;
	
	if ((seconds & 0x01) == 1) {
		out = b_on & ~(b_blink);
	}

	if (!led_state) {
		out = 0x00;
	}
	
	BTN_DIVISION_LED_set_level((out >> 0) & 0x01);
	BTN_CONFERENCE_LED_set_level((out >> 1) & 0x01);
	BTN_LEAGUE_LED_set_level((out >> 2) & 0x01);
	BTN_PLAYOFFS_LED_set_level((out >> 3) & 0x01);
	BTN_POWER_LED_set_level((out >> 4) & 0x01);
	
	seconds++;
	ticked = true;
}

// -------------------------------------------------------------------------------

int main(void) {
	atmel_start_init();
	
	_delay_ms(2000);

	printf("\n");
	printf("mht-base v0.1 by kiu\n");
	printf("https://github.com/kiu/MiniHelmetTracker\n");
	printf("\n");

	_delay_ms(2000);

	sei();
	
	main_store_load();
	led_blank();
	
	if (ALWAYS_CLEAN) {
		clean = CLEAN_FLAG;
	}
	
	if (clean == CLEAN_FLAG) {
		printf("MAIN Previous shutdown was CLEAN, ready to start.\n");
		printf("MAIN Loaded layout:\n");
		helmet_print();
		printf("MAIN Last known mode: %s\n", main_mode_to_string(modenext));
		} else {
		printf("MAIN Previous shutdown was UNCLEAN, manual setup required.\n");
		helmet_load_manual();
		modenext = NONE;
		clean = CLEAN_FLAG;
		main_store_save(mode);
	}
	
	main_button_update(mode, modenext);

	servo_init();
	stepper_home();
	stepper_move(34);
	
	wifi_reset();
	wifi_update();
	
	while(state != SHUTDOWN) {
		if (ticked) {
			ticked = false;
			main_tick();
		}
		_delay_ms(10);
	}
	
	printf("MAIN Shutdown sequence initiated...\n");
	
	stepper_move(0);
	
	servo_shutdown();
	stepper_shutdown();
	
	clean = CLEAN_FLAG;
	main_store_save(modelast);

	led_blank();
	
	b_on = 0x00;
	
	printf("MAIN Shutdown completed. Go Seahawks.\n");
	
	_delay_ms(2000);
	
	cli();
}
