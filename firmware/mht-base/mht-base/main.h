/*
* main.h
*
* Created: 22.07.2022 13:34:38
*  Author: kiu
*/

#ifndef MAIN_H_
#define MAIN_H_

typedef enum {INIT, IDLE, SORTING, SHUTDOWN} opstate;
typedef enum {DIVISION, CONFERENCE, LEAGUE, PLAYOFFS, POWEROFF, NONE} opmode;

void main_button_update(opmode on, opmode blink);

void main_change_led_auto(bool value);

opmode main_mode_get();
opmode main_mode_next_get();
void main_mode_next_set(opmode om);

void main_set_unclean();

#endif /* MAIN_H_ */