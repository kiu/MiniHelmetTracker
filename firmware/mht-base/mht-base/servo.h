/*
* servo.h
*
* Created: 17.07.2022 13:56:10
*  Author: kiu
*/

#ifndef SERVO_H_
#define SERVO_H_

typedef enum {SERVO_DOWN, SERVO_MIDDLE_OPEN, SERVO_MIDDLE_CLOSED, SERVO_UP} servo_configuration;

void servo_init();
void servo_change(servo_configuration sc);
void servo_shutdown();

#endif /* SERVO_H_ */