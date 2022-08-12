/*
 * led.h
 *
 * Created: 17.07.2022 14:53:45
 *  Author: kiu
 */ 

#ifndef LED_H_
#define LED_H_

#include <atmel_start.h>

void led_set_auto(bool on);

void led_blank();
void led_all(uint8_t r, uint8_t g, uint8_t b);

void led_update();

void led_load(uint8_t slot);

void led_indicate_error();
void led_indicate_error_download(bool error);
void led_indicate_error_server(bool error);

#endif /* LED_H_ */
