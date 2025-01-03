/*
 * atc.h
 *
 *  Created on: Dec 25, 2024
 *      Author: Dipan
 */

#ifndef ATC_ATC_H_
#define ATC_ATC_H_

#include "atc_cmd.h"
#include "port_uart.h"

#define ATC_MAX_BUFF_SIZE 512
#define ATC_TX_DATA_MAX_LEN 100

typedef enum atc_tagFnStatus{
	ATC_FN_STATUS_ERR_PARAM = -3,
	ATC_FN_STATUS_TIMEOUT = -2,
	ATC_FN_STATUS_FAIL = -1,
	ATC_FN_STATUS_OK = 0,
}atc_fnStatus_t;

typedef struct tagAtcData{
	atc_cmd_t cmd;
	union{
		char txData[ATC_TX_DATA_MAX_LEN + 1];
		char rxData[ATC_MAX_BUFF_SIZE + 1];
	}data;
}atc_data_t;

void atc_CheckForValidData(void);	//better to run this as a task
uint8_t atc_Init(port_uart_handle_t *handle);
atc_fnStatus_t atc_Commander(port_uart_handle_t *handle, atc_data_t *pData, uint16_t timeoutMs);

#endif /* ATC_ATC_H_ */
