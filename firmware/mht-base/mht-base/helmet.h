/*
* helmet.h
*
* Created: 21.07.2022 21:36:08
*  Author: kiu
*/

#ifndef HELMET_H_
#define HELMET_H_

#include "main.h"

uint8_t helmet_issort();
uint8_t helmet_dosort();

void helmet_load_manual();

uint8_t* helmet_get_current();
uint8_t* helmet_get_target();
uint8_t* helmet_get_color();

void helmet_set_current(uint8_t hc[]);

void helmet_set_mode(opmode mode);
void helmet_convert_store_data(char data[]);

void helmet_print();

#endif /* HELMET_H_ */