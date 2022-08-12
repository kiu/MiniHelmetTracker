/*
* stepper.h
*
* Created: 17.07.2022 14:47:51
*  Author: kiu
*/

#ifndef STEPPER_H_
#define STEPPER_H_

typedef enum {STEPPER_LEFT, STEPPER_RIGHT} stepper_direction;

void stepper_home();
void stepper_move(uint8_t slot);
void stepper_shutdown();

#endif /* STEPPER_H_ */