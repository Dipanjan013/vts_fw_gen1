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

typedef enum{
	APP_FLAG_BIT_ADXL_TAP = (1 << 0),
}appFlagBits_e;

typedef struct{

}app_cellularData_s;	//separate file for this?

typedef struct{

}app_gpsData_s;				//separate file for this?

typedef struct{
	uint16_t event;
	union{
		struct{
			app_cellularData_s data;
		}cellular;
		struct{
			app_gpsData_s data;
		}gps;
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
