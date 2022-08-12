/*
* wifi.c
*
* Created: 25.07.2022 22:04:36
*  Author: kiu
*/

#include <atmel_start.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "usart_basic.h"
#include "config.h"
#include "main.h"
#include "helmet.h"
#include "led.h"
#include "panic.h"
#include "wifi.h"

/*
--- DO ONCE TO SETUP DEFAULT CONFIG ---

AT+UART_DEF=9600,8,1,0,0
AT+CWMODE_DEF=1
AT+CWDHCP_DEF=1,1
AT+CWJAP_DEF="ssid","pwd"

--- DO ONCE TO SETUP DEFAULT CONFIG ---
*/

char buffer_rx [256];
uint8_t buffer_rx_idx = 0;
volatile uint8_t wifi_data_count = 0;

ISR(USART1_RXC_vect) {
	while ((USART1.STATUS & USART_RXCIF_bm)) {
		buffer_rx[buffer_rx_idx] = USART1.RXDATAL;
		buffer_rx_idx++;

		if (buffer_rx[buffer_rx_idx - 1] == '\n' || buffer_rx_idx == 255) {
			memset(buffer_rx + buffer_rx_idx, 0, 256-buffer_rx_idx);
			buffer_rx_idx = 0;
			
			if (WIFI_DEBUG) {
				printf("WIFI RX: %s\n", buffer_rx);
			}
			
			if (strncmp(buffer_rx, "+IPD,72:", 8) == 0) {
				char buffer_data[72];
				memset(buffer_data, 0, 72);
				memcpy(buffer_data, buffer_rx + 8, 72);
				buffer_data[71] = 0x00;
				helmet_convert_store_data(buffer_data);
				wifi_data_count++;
				printf("WIFI Data: %s\n", buffer_data);
			}
			
			if (strncmp(buffer_rx, "+IPD,6:", 7) == 0) {
				char buffer_data[6];
				memset(buffer_data, 0, 6);
				memcpy(buffer_data, buffer_rx + 7, 6);
				buffer_data[5] = 0x00;
				if (buffer_data[0] != 'A') {
					continue;
				}
				
				main_change_led_auto(buffer_data[1] - 'A');
				
				if (buffer_data[2] - 'A' != 42) {
					main_mode_next_set(buffer_data[2] - 'A');
				}
				
				if (buffer_data[3] - 'A' == 1) {
					main_set_unclean();
					main_mode_next_set(POWEROFF);
				}
				
				led_indicate_error_server(buffer_data[4] - 'A');
				
				wifi_data_count++;
				printf("WIFI Admin: %s\n", buffer_data);
			}
		}
	}
}

void esp8266_tx(char buf[]) {
	if(WIFI_DEBUG) {
		printf("WIFI TX: ");
		printf(buf);
	}

	if (!WIFI_ENABLE) {
		printf("WIFI TX: Skipped as WIFI is off.");
		return;
	}
	
	for (uint8_t i = 0; i < strlen(buf); i++) {
		while (!(USART1.STATUS & USART_DREIF_bm)) {}
		USART1.TXDATAL = buf[i];
	}
	
	while (!(USART1.STATUS & USART_DREIF_bm)) {}
	_delay_ms(100);
}

void wifi_reset() {
	printf("WIFI Resetting (30s)...\n");
	esp8266_tx("\r\n");
	_delay_ms(1000);
	esp8266_tx("AT+RST\r\n");
	_delay_ms(6000);
	esp8266_tx("ATE0\r\n");
	esp8266_tx("AT+CWHOSTNAME=\"mht\"\r\n");
	esp8266_tx("AT+CWAUTOCONN=1\r\n");
	_delay_ms(20000);
	esp8266_tx("AT+CIPMUX=0\r\n");
	esp8266_tx("AT+CIPMODE=0\r\n");
	printf("WIFI Resetting done.\n");
}

void wifi_update() {
	wifi_data_count = 0;

	if(!WIFI_ENABLE) {
		printf("WIFI Ingoring update request, WIFI is off.\n");
	}
	
	printf("WIFI Updating data (10s)...\n");
	esp8266_tx("\r\n");
	_delay_ms(1000);
	esp8266_tx("AT+CIPSTART=\"TCP\",\"mht.nakamura-labs.com\",2207\r\n");
	_delay_ms(1000);
	esp8266_tx("AT+CIPSEND=4\r\n");
	esp8266_tx("V01\n");
	_delay_ms(8000);
	esp8266_tx("AT+CIPCLOSE\r\n");
	_delay_ms(2000);
	printf("WIFI Updating data done.\n");
	
	led_indicate_error_download(wifi_data_count != 5);
	led_indicate_error();
}