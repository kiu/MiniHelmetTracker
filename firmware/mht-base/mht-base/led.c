/*
* led.c
*
* Created: 17.07.2022 14:53:55
*  Author: kiu
*/

#include <atmel_start.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "helmet.h"
#include "led.h"
#include "main.h"

uint8_t led_stream[424];
uint8_t led_zeros[4] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t led_zerooos[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
bool led_on = true;

bool led_error_download = false;
bool led_error_server = false;

const uint8_t COLOR_32 [99] = {
	0x00, 0x00, 0x00,
	0x00, 0xff, 0x00,
	0x00, 0xff, 0x00,
	0x01, 0xff, 0x00,
	0x03, 0xff, 0x00,
	0x06, 0xff, 0x00,
	0x0b, 0xff, 0x00,
	0x12, 0xff, 0x00,
	0x1b, 0xff, 0x00,
	0x27, 0xff, 0x00,
	0x38, 0xff, 0x00,
	0x4a, 0xff, 0x00,
	0x60, 0xff, 0x00,
	0x7c, 0xff, 0x00,
	0x9a, 0xff, 0x00,
	0xbf, 0xff, 0x00,
	0xe7, 0xff, 0x00,
	0xff, 0xe7, 0x00,
	0xff, 0xbf, 0x00,
	0xff, 0x9a, 0x00,
	0xff, 0x7c, 0x00,
	0xff, 0x60, 0x00,
	0xff, 0x4a, 0x00,
	0xff, 0x38, 0x00,
	0xff, 0x27, 0x00,
	0xff, 0x1b, 0x00,
	0xff, 0x12, 0x00,
	0xff, 0x0b, 0x00,
	0xff, 0x06, 0x00,
	0xff, 0x03, 0x00,
	0xff, 0x01, 0x00,
	0xff, 0x00, 0x00,
	0xff, 0x00, 0x00,
};

void led_set_auto(bool on) {
	if (led_on == on) {
		return;
	}
	
	if (on)	 {
		printf("LED Turning on.\n");
		led_on = true;
		led_update();
		} else {
		printf("LED Turning off.\n");
		led_blank();
		led_on = false;
	}
}

void led_send() {
	uint8_t led_tmp[212];

	SPI_0_write_block(led_zeros, 4);
	while (SPI_0_status_busy());

	memcpy(led_tmp, led_stream, 212);
	SPI_0_write_block(led_tmp, 212);
	while (SPI_0_status_busy());

	memcpy(led_tmp, led_stream + 212, 212);
	SPI_0_write_block(led_tmp, 212);
	while (SPI_0_status_busy());

	SPI_0_write_block(led_zerooos, 7);
	while (SPI_0_status_busy());
}

void led_blank() {
	led_all(0x00, 0x00, 0x00);
}

void led_all(uint8_t r, uint8_t g, uint8_t b) {
	memset(led_stream, 0, 424);
	for (uint16_t i = 0; i < 424; i+= 4) {
		led_stream[i + 0] = 0xE0 | LED_BRIGHTNESS;
		led_stream[i + 1] = b;
		led_stream[i + 2] = g;
		led_stream[i + 3] = r;
	}
	led_send();
}

void led_indicate_error() {
	uint8_t r = 0x00;
	uint8_t g = 0x00;
	uint8_t b = 0x00;
	
	if (led_error_server) {
		r = 0x00;
		g = 0xFF;
		b = 0xFF;
	}

	if (led_error_download) {
		r = 0x00;
		g = 0x00;
		b = 0xFF;
	}
	
	led_stream[0 + 0] = 0xE0 | LED_BRIGHTNESS;
	led_stream[0 + 1] = b;
	led_stream[0 + 2] = g;
	led_stream[0 + 3] = r;

	led_stream[420 + 0] = 0xE0 | LED_BRIGHTNESS;
	led_stream[420 + 1] = b;
	led_stream[420 + 2] = g;
	led_stream[420 + 3] = r;
	
	led_send();
}

void led_indicate_error_download(bool error) {
	if (led_error_download == error) {
		return;
	}
	led_error_download = error;
}

void led_indicate_error_server(bool error) {
	if (led_error_server == error) {
		return;
	}
	led_error_server = error;
}

void led_update() {
	if (!LED_ENABLE || !led_on) {
		return;
	}
	
	uint8_t current[35];
	memcpy(current, helmet_get_current(), 35);
	
	uint8_t target[35];
	memcpy(target, helmet_get_target(), 35);
	
	uint8_t color[35];
	memcpy(color, helmet_get_color(), 35);
	
	opmode mode = main_mode_get();

	uint8_t r1 = 0x00;
	uint8_t g1 = 0x00;
	uint8_t b1 = 0x00;

	uint8_t r3 = 0x00;
	uint8_t g3 = 0x00;
	uint8_t b3 = 0x00;
	
	for (uint8_t i = 0; i < 35; i++) {
		if (current[i] != target[i]) {
			r1 = 0xFF;
			g1 = 0x00;
			b1 = 0xFF;

			r3 = 0x00;
			g3 = 0x00;
			b3 = 0x00;
			} else {
			r1 = COLOR_32[color[i] * 3 + 0];
			g1 = COLOR_32[color[i] * 3 + 1];
			b1 = COLOR_32[color[i] * 3 + 2];
			
			if (mode != PLAYOFFS && i < 34 && color[i] == color[i+1] && current[i+1] == target[i+1]) {
				r3 = r1;
				g3 = g1;
				b3 = b1;
				} else {
				r3 = 0x00;
				g3 = 0x00;
				b3 = 0x00;
			}
		}
		
		led_stream[4 + i * 12 + 0] = 0xE0 | LED_BRIGHTNESS;
		led_stream[4 + i * 12 + 1] = b1;
		led_stream[4 + i * 12 + 2] = g1;
		led_stream[4 + i * 12 + 3] = r1;

		led_stream[4 + i * 12 + 4] = 0xE0 | LED_BRIGHTNESS;
		led_stream[4 + i * 12 + 5] = b1;
		led_stream[4 + i * 12 + 6] = g1;
		led_stream[4 + i * 12 + 7] = r1;

		if (i != 34) {
			led_stream[4 + i * 12 + 8] = 0xE0 | LED_BRIGHTNESS;
			led_stream[4 + i * 12 + 9] = b3;
			led_stream[4 + i * 12 + 10] = g3;
			led_stream[4 + i * 12 + 11] = r3;
		}
	}
	
	led_send();
}

void led_load(uint8_t slot) {
	uint8_t r = 0x00;
	uint8_t g = 0x00;
	uint8_t b = 0x00;
	
	for (uint8_t i = 0; i < 35; i++) {
		
		if (i < slot) {
			r = 0x00;
			g = 0xFF;
			b = 0x00;
		}

		if (i == slot) {
			r = 0xFF;
			g = 0x00;
			b = 0xFF;
		}

		if (i > slot) {
			r = 0x00;
			g = 0x00;
			b = 0x00;
		}

		led_stream[4 + i * 12 + 0] = 0xE0 | LED_BRIGHTNESS;
		led_stream[4 + i * 12 + 1] = r;
		led_stream[4 + i * 12 + 2] = g;
		led_stream[4 + i * 12 + 3] = b;

		led_stream[4 + i * 12 + 4] = 0xE0 | LED_BRIGHTNESS;
		led_stream[4 + i * 12 + 5] = r;
		led_stream[4 + i * 12 + 6] = g;
		led_stream[4 + i * 12 + 7] = b;

		led_stream[4 + i * 12 + 8] = 0xE0 | LED_BRIGHTNESS;
		led_stream[4 + i * 12 + 9] = 0x00;
		led_stream[4 + i * 12 + 10] = 0x00;
		led_stream[4 + i * 12 + 11] = 0x00;
	}
	
	led_send();
}


/*

# https://learn.adafruit.com/led-tricks-gamma-correction/the-quick-fix
gamma8 = [
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
]

# https://stackoverflow.com/questions/24852345/hsv-to-rgb-color-conversion
def hsv_to_rgb(h, s, v):
if s == 0.0: v*=255; return (v, v, v)
i = int(h*6.) # XXX assume int() truncates!
f = (h*6.)-i; p,q,t = int(255*(v*(1.-s))), int(255*(v*(1.-s*f))), int(255*(v*(1.-s*(1.-f)))); v*=255; i%=6
if i == 0: return (v, t, p)
if i == 1: return (q, v, p)
if i == 2: return (p, v, t)
if i == 3: return (p, q, v)
if i == 4: return (t, p, v)
if i == 5: return (v, p, q)

def gen(steps):
print("const uint8_t COLOR_%d [%d] = {" % (steps, (steps + 1) * 3))
print("0x00, 0x00, 0x00, ")
distance = 120.0 / (steps - 1)
angle = 120.0
for x in range(0, steps):
rgb = hsv_to_rgb(float(angle) / 360.0, 1, 1)
print("0x%02x, 0x%02x, 0x%02x, " % (gamma8[rgb[0]], gamma8[rgb[1]], gamma8[rgb[2]]))
angle = angle - distance
print("};")
print();

gen(32)

*/
