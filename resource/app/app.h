/*
 * app.h
 *
 *  Created on: Jan 26, 2025
 *      Author: Dipanjan Banerjee
 */

#ifndef APP_APP_H_
#define APP_APP_H_

#include <stdint.h>
#include "config.h"
#include "port_timer.h"
#include "port_led.h"
#include "service_at.h"
#include "service_btn.h"
#include "drv_adxl.h"
#include "utils_gps.h"

#define APP_PUB_PACKET_SIZE 100

typedef enum{
	APP_FLAG_BIT_ADXL_TAP = (1 << 0),
	APP_FLAG_BIT_RI_FIRED = (1 << 1),
}appFlagBits_e;

typedef struct{
	uint16_t event;
	union{
		struct{
			char data[APP_PUB_PACKET_SIZE + 1];
		}telemetry;
		struct{
			nmea_s nmea;
		}gps;
		struct{
			uint16_t err_code;
			char info[10];
		}errData;
	}param;
}app_eventParam_s;

void app_main(void);

/*!
 * @fn : Post event to app layer
 * @param : pParam app_eventParam_s
 * @return nothing
 */
void AppPostEvent(app_eventParam_s *pParam);

/*!
 * @fn Post event to app layer from interrupt callback handler
 * @param pParam app_eventParam_s
 */
void AppPostEventFromIsr(app_eventParam_s *pParam);

uint8_t app_flagGet(appFlagBits_e bit);

#endif /* APP_APP_H_ */
