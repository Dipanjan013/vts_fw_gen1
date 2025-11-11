/*
 * service_led.h
 *
 *  Created on: Nov 2, 2025
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_LED_H_
#define SERVICE_SERVICE_LED_H_

#include "port_gpio.h"
#include "port_led.h"

typedef enum service_led_pattern{
	SERVICE_LED_PATTERN_BOOTUP,
	SERVICE_LED_PATTERN_REBOOT,
	SERVICE_LED_PATTERN_ONBOARD,
	SERVICE_LED_PATTERN_MFG_NULL,
	SERVICE_LED_PATTERN_TELEMETRY,
	SERVICE_LED_PATTERN_MAX
}service_led_pattern_e;

void service_led_Init(void);
void service_led_DeInit(void);
void service_led_SetPattern(service_led_pattern_e pattern);

#endif /* SERVICE_SERVICE_LED_H_ */
