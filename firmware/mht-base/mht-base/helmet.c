/*
* helmet.c
*
* Created: 21.07.2022 21:36:18
*  Author: kiu
*/

#include <atmel_start.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "config.h"
#include "helmet.h"
#include "led.h"
#include "main.h"
#include "servo.h"
#include "stepper.h"

const char TEAMS[] = "---ARIATLBALBUFCARCHICINCLEDALDENDETGBPHOUINDJAXKCCMIAMINNEPNOSNYGNYJLVRPHIPITLACSF4SEALARTBBTENWAS";

uint8_t helmet_target[35] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 0, 0 };
uint8_t helmet_current[35] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 0, 0 };
uint8_t helmet_color[35];

char tmp[4] = { '\0', '\0', '\0', '\0' };

uint8_t helmet_league_order[35] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 0, 0 };
uint8_t helmet_league_color[35] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t helmet_conference_order[35] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 0, 0 };
uint8_t helmet_conference_color[35] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t helmet_division_order[35] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 0, 0 };
uint8_t helmet_division_color[35] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t helmet_playoffs_order[35] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 0, 0 };
uint8_t helmet_playoffs_color[35] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/*
ID,Name,Abbreviation,Conference,Division
1,Arizona Cardinals,ARI,NFC,West
2,Atlanta Falcons,ATL,NFC,South
3,Baltimore Ravens,BAL,AFC,North
4,Buffalo Bills,BUF,AFC,East
5,Carolina Panthers,CAR,NFC,South
6,Chicago Bears,CHI,NFC,North
7,Cincinnati Bengals,CIN,AFC,North
8,Cleveland Browns,CLE,AFC,North
9,Dallas Cowboys,DAL,NFC,East
10,Denver Broncos,DEN,AFC,West
11,Detroit Lions,DET,NFC,North
12,Green Bay Packers,GB,NFC,North
13,Houston Texans,HOU,AFC,South
14,Indianapolis Colts,IND,AFC,South
15,Jacksonville Jaguars,JAX,AFC,South
16,Kansas City Chiefs,KC,AFC,West
17,Miami Dolphins,MIA,AFC,East
18,Minnesota Vikings,MIN,NFC,North
19,New England Patriots,NE,AFC,East
20,New Orleans Saints,NO,NFC,South
21,NY Giants,NYG,NFC,East
22,NY Jets,NYJ,AFC,East
23,Las Vegas Raiders,LV,AFC,West
24,Philadelphia Eagles,PHI,NFC,East
25,Pittsburgh Steelers,PIT,AFC,North
26,Los Angeles Chargers,LAC,AFC,West
27,San Francisco 49ers,SF,NFC,West
28,Seattle Seahawks,SEA,NFC,West
29,Los Angeles Rams,LAR,NFC,West
30,Tampa Bay Buccaneers,TB,NFC,South
31,Tennessee Titans,TEN,AFC,South
32,Washington Football Team,WAS,NFC,East
*/


void helmet_set_current(uint8_t hc[]) {
	memcpy(helmet_current, hc, 35);
	memcpy(helmet_target, hc, 35);
	memset(helmet_color, 0, 35);
	led_update();
}

uint8_t* helmet_get_current() {
	return helmet_current;
}
uint8_t* helmet_get_target() {
	return helmet_target;
}
uint8_t* helmet_get_color() {
	return helmet_color;
}

void helmet_set_mode(opmode mode) {
	if (mode == LEAGUE) {
		memcpy(helmet_target, helmet_league_order, 35);
		memcpy(helmet_color, helmet_league_color, 35);
		led_update();
		return;
	}

	if (mode == CONFERENCE) {
		memcpy(helmet_target, helmet_conference_order, 35);
		memcpy(helmet_color, helmet_conference_color, 35);
		led_update();
		return;
	}

	if (mode == DIVISION) {
		memcpy(helmet_target, helmet_division_order, 35);
		memcpy(helmet_color, helmet_division_color, 35);
		led_update();
		return;
	}

	if (mode == PLAYOFFS) {
		memcpy(helmet_target, helmet_playoffs_order, 35);
		memcpy(helmet_color, helmet_playoffs_color, 35);
		led_update();
		return;
	}
}

void helmet_convert_store_data(char data[]) {
	uint8_t converted[72];
	for (uint8_t i = 1; i < 71; i++) {
		converted[i] = data[i] - 'A';
	}
	
	if (data[0] == 'L')	{
		memcpy(helmet_league_order, converted + 1, 35);
		memcpy(helmet_league_color, converted + 1 + 35, 35);
		return;
	}

	if (data[0] == 'C')	{
		memcpy(helmet_conference_order, converted + 1, 35);
		memcpy(helmet_conference_color, converted + 1 + 35, 35);
		return;
	}

	if (data[0] == 'D')	{
		memcpy(helmet_division_order, converted + 1, 35);
		memcpy(helmet_division_color, converted + 1 + 35, 35);
		return;
	}

	if (data[0] == 'P')	{
		memcpy(helmet_playoffs_order, converted + 1, 35);
		memcpy(helmet_playoffs_color, converted + 1 + 35, 35);
		return;
	}
}

uint8_t helmet_find_value_in_array(uint8_t n, uint8_t *a) {
	for (uint8_t i = 0; i < 35; i++) {
		if (a[i] == n) {
			return i;
		}
	}
	return 0xFF;
}

uint8_t helmet_find_unmatched_slot_not_zero() {
	for (uint8_t i = 0; i < 35; i++) {
		if (helmet_current[i] == 0) continue;
		if (helmet_current[i] == helmet_target[i]) continue;
		return i;
	}
	return 0xFF;
}

void helmet_print() {
	if (HELMET_DEBUG) {
		for (uint8_t i = 0; i < 35; i++) {
			printf("%03d ", helmet_color[i]);
		}
		printf("\n");

		for (uint8_t i = 0; i < 35; i++) {
			printf("%03d ", helmet_target[i]);
		}
		printf("\n");
	}
	
	for (uint8_t i = 0; i < 35; i++) {
		memcpy(tmp, TEAMS + (helmet_target[i] * 3), 3);
		printf("%s ", tmp);
	}
	printf("\n");
	for (uint8_t i = 0; i < 35; i++) {
		memcpy(tmp, TEAMS + (helmet_current[i] * 3), 3);
		printf("%s ", tmp);
	}
	printf("\n");
	
	if (HELMET_DEBUG) {
		for (uint8_t i = 0; i < 35; i++) {
			printf("%03d ", helmet_current[i]);
		}
		printf("\n");
	}
}

void helmet_swap(uint8_t slot1, uint8_t slot2) {
	memcpy(tmp, TEAMS + (helmet_current[slot1] * 3), 3);
	printf("HELMET Swap slot: %s (%d) vs --- (%d)\n", tmp, slot1, slot2);
	
	stepper_move(slot1);
	servo_change(SERVO_MIDDLE_OPEN);
	servo_change(SERVO_MIDDLE_CLOSED);
	servo_change(SERVO_UP);
	stepper_move(slot2);
	servo_change(SERVO_MIDDLE_CLOSED);
	servo_change(SERVO_MIDDLE_OPEN);
	servo_change(SERVO_DOWN);
	
	helmet_current[slot2] = helmet_current[slot1];
	helmet_current[slot1] = 0;

	helmet_print();
	led_update();
}

uint8_t helmet_issort() {
	return memcmp(helmet_current, helmet_target, 35) == 0;
}

uint8_t helmet_dosort() {
	if (helmet_issort()) {
		stepper_move(34);
		return true;
	}

	// Find empty slot which shouldn't be empty
	for (uint8_t i = 0; i < 35; i++) {
		if (helmet_current[i] != 0) continue;
		if (helmet_target[i] == 0) continue;
		uint8_t swap = helmet_find_value_in_array(helmet_target[i], helmet_current);
		helmet_swap(swap, i);
		return false;
	}
	
	// Didn't find one, alternative, pick a random slot which isn't correct
	uint8_t s1 = helmet_find_value_in_array(0, helmet_current);
	uint8_t s2 = helmet_find_unmatched_slot_not_zero();
	helmet_swap(s2, s1);
	return false;
}

void helmet_load_manual() {
	led_all(0x00, 0x00, 0xFF);
	
	printf("HELMET Confirm to move servo to open position.\n");
	main_mode_next_set(NONE);
	
	main_button_update(DIVISION, NONE);
	while(main_mode_next_get() != DIVISION) {}
	main_button_update(NONE, NONE);
	servo_init();

	printf("HELMET Confirm to move stepper to home.\n");
	main_mode_next_set(NONE);
	main_button_update(CONFERENCE, NONE);
	while(main_mode_next_get() != CONFERENCE) {}
	main_button_update(NONE, NONE);
	stepper_home();
	
	led_blank();
	
	for (uint8_t i = 0 ; i < 34; i++) {
		led_load(i);
		servo_change(SERVO_DOWN);
		stepper_move(i);
		servo_change(SERVO_MIDDLE_OPEN);
		memcpy(tmp, TEAMS + (i * 3), 3);
		printf("HELMET Please load slot %02d with %s\n", i, tmp);
		main_mode_next_set(NONE);

		main_button_update(LEAGUE, NONE);
		while(main_mode_next_get() != LEAGUE) {}
		main_button_update(NONE, NONE);
		servo_change(SERVO_MIDDLE_CLOSED);
		_delay_ms(500);
	}
	servo_change(SERVO_DOWN);

	led_blank();

	stepper_move(0);
	
	printf("HELMET Initial setup completed.\n");
	
}
