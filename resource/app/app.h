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

#endif /* APP_APP_H_ */
