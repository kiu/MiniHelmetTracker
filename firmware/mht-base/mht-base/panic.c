/*
* panic.c
*
* Created: 20.07.2022 17:54:48
*  Author: kiu
*/

#include <stdio.h>
#include "panic.h"
#include "led.h"

void panic(const char * c) {
	printf("\n");
	printf("*** PANIC ***\n");
	printf(c);
	printf("*** PANIC ***\n");
	led_all(0xFF, 0x00, 0x00);
	
	while(1) {}
}

