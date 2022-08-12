/*
* config.h
*
* Created: 22.07.2022 21:18:47
*  Author: kiu
*/

#ifndef CONFIG_H_
#define CONFIG_H_

// --------------------------------------------------

#define ALWAYS_CLEAN 0

// --------------------------------------------------

#define HELMET_DEBUG 1

// --------------------------------------------------

#define LED_ENABLE 1
#define LED_BRIGHTNESS 0x01

// --------------------------------------------------

#define SERVO_ENABLE 1

#define SERVO_DELAY 300
#define SERVO_PLATE_DOWN 32
#define SERVO_PLATE_MIDDLE 25
#define SERVO_PLATE_UP 18

#define SERVO_HORN_DOWN 17
#define SERVO_HORN_UP 28

// --------------------------------------------------

#define STEPPER_ENABLE 1

#define STEPPER_WIGGLE 400UL
#define STEPPER_OFFSET 2560UL
#define STEPPER_RAMP 1800UL

// SPEEDs are multiples of 10us

#define STEPPER_HOME_SPEED_FAST 20
#define STEPPER_HOME_SPEED_SLOW 160

#define STEPPER_SPEED_MIN 5 // 4
#define STEPPER_SPEED_FAST 0
#define STEPPER_SPEED_SLOW 20

#define STEPPER_SPEED_CHANGE (STEPPER_RAMP / (STEPPER_SPEED_SLOW - STEPPER_SPEED_FAST))

// --------------------------------------------------

#define WIFI_ENABLE 1
#define WIFI_DEBUG 0
#define WIFI_UPDATE 300UL
#define WIFI_RESET 21600UL

// --------------------------------------------------


#endif /* CONFIG_H_ */
