/*
 * port_led.h
 *
 *  Created on: May 14, 2025
 *      Author: Dipanjan
 */

#ifndef PORT_PORT_LED_H_
#define PORT_PORT_LED_H_

#include <stdint.h>
#include "port_gpio.h"

#define PORT_LED_BLUE_PIN 	PORT_GPIO_PIN_5
#define PORT_LED_BLUE_PORT 	PORT_GPIO_PORT_A

typedef enum{
	PORT_LED_COLOUR_BLUE,
}port_led_colour_e;

/*!
 * @note RGB LED
 */
#define PORT_LED_COMMON_ANODE 	0
#define PORT_LED_COMMON_CATHODE 1

#define PORT_LED_PULL_TYPE PORT_GPIO_PULL_DOWN

uint8_t port_led_Init(void);
uint8_t port_led_DeInit(void);
void port_led_On(port_led_colour_e colour);
void port_led_Off(port_led_colour_e colour);
void port_led_Toggle(port_led_colour_e colour);

#endif /* PORT_PORT_LED_H_ */
