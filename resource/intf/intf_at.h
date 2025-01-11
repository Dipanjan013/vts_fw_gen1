/*
 * intf_at.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Dipan
 */

#ifndef INTF_INTF_AT_H_
#define INTF_INTF_AT_H_

#include <stdint.h>
#include "port_uart.h"

#define INTF_AT_RX_DATA_MAX 100

typedef enum intf_at_tagFnStatus{
	INTF_AT_FN_STATUS_OK,
	INTF_AT_FN_STATUS_FAIL,
	INTF_AT_FN_STATUS_TIMEOUT,
}intf_at_fnStatus_t;

typedef enum{
	INTF_BLE_UNSOL_RESP_NEW_MSG = 0,
}intf_ble_unsolResp_t;

typedef struct{
	uint16_t event;
	char *str;
}intf_ble_unsolRespTable_t;

typedef struct{
	uint16_t event;
	uint8_t data[100];
}intf_ble_unsolRespParam_t;


intf_at_fnStatus_t intf_at_Init(port_uart_handle_t *handle);
intf_at_fnStatus_t intf_at_DeInit(port_uart_handle_t *handle);
intf_at_fnStatus_t intf_at_Command(port_uart_handle_t *handle,
								   uint8_t *cmd,
								   uint16_t cmdLen,
								   uint8_t *rxBuff,
								   uint16_t size,
								   uint8_t *expResp,
								   uint16_t timeoutMs);

__WEAK void intf_at_UnsolRespCallbac(intf_ble_unsolRespParam_t *pParam);
void intf_at_UnsolRespChecker(void);

#endif /* INTF_INTF_AT_H_ */
