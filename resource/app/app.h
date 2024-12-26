/*
 * app.h
 *
 *  Created on: Sep 1, 2024
 *      Author: Dipan
 */

#ifndef APP_APP_H_
#define APP_APP_H_

#include <atc1.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct app_tagEventparam{
	int16_t event;
	union{
		atc_data_t atc_SendData;
		atc_data_t atc_RecvData;
	}data;
}app_eventParam_t;

void app_main(void);
void app_PostEvent(app_eventParam_t *pParam, uint8_t flagFromISR);

#endif /* APP_APP_H_ */
