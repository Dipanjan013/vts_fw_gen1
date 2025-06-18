/*
 * service_btn.h
 *
 *  Created on: Jun 8, 2025
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_BTN_H_
#define SERVICE_SERVICE_BTN_H_

#include "port_gpio.h"

#define SERVICE_BTN_PIN PORT_GPIO_PIN_6
#define SERVICE_BTN_PORT PORT_GPIO_PORT_A

#define SERVICE_BTN_LONGPRESS_MS 									800
#define SERVICE_BTN_LONGPRESS_OFFSET_MS 					500

#define SERVICE_BTN_SHORTPRESS_MS 								50
#define SERVICE_BTN_SHORTPRESS_OFFSET_MS 					(SERVICE_BTN_LONGPRESS_MS - 100)

#define SERVICE_BTN_RELEASE_TIMEOUT_MS	(SERVICE_BTN_LONGPRESS_MS + 1000)
#define SERVICE_BTN_WAIT_FOR_PRESS_TIMEOUT_MS 800

#define SERVICE_BTN_MAX_PRESS 		3
#define SERVICE_BTN_DEBOUNCE_MS 	20

typedef enum{
	SERVICE_BTN_PATTERN_ONBOARDING = 0,
	SERVICE_BTN_PATTERN_REBOOT = 1,
	SERVICE_BTN_PATTERN_CLEARALL = 2,
	SERVICE_BTN_PATTERN_TESTMODE = 3,
	SERVICE_BTN_PATTERN_MAX,
}service_btn_pattern_t;

/*!
 * @note This is a weak function
 * @param pattern - button pattern
 */
void service_btn_EventCb(service_btn_pattern_t pattern);
void service_btn_Init(void);
void service_btn_Deinit(void);

#endif /* SERVICE_SERVICE_BTN_H_ */
