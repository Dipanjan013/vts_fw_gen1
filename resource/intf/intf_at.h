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

#define INTF_AT_DEFAULT_TIMEOUT_MS 1000
#define INTF_AT_RX_DATA_MAX 512
#define INTF_AT_UNSOL_RESP_DATA_MAX 100

typedef enum intf_at_tagFnStatus{
	INTF_AT_FN_STATUS_OK,
	INTF_AT_FN_STATUS_FAIL,
	INTF_AT_FN_STATUS_TIMEOUT,
	INTF_AT_FN_STATUS_BUSY,
}intf_at_fnStatus_t;

typedef enum{
	UNSOL_RESP_CODE_MT_MSG = 0,
}intf_ble_unsolRespCode_t;

typedef struct{
	uint16_t respcode;
	char *respStr;
}intf_ble_unsolRespTable_t;

typedef struct{
	uint16_t respCode;
	uint8_t data[INTF_AT_UNSOL_RESP_DATA_MAX + 1];
}intf_ble_unsolRespParam_t;


intf_at_fnStatus_t intf_at_Init(port_uart_handle_t *handle);
intf_at_fnStatus_t intf_at_DeInit(port_uart_handle_t *handle);

/*!
 * @brief: Function is used to send the command and wait for the desired response until timeoutMs
 * If expResp i.e expected response string  is not null, it will search for expResp string. Otherwise only "OK" or "ERROR" is searched
 */
intf_at_fnStatus_t intf_at_Command(port_uart_handle_t *handle,
								   uint8_t *cmd,
								   uint16_t cmdLen,
								   uint8_t *rxBuff,
								   uint16_t size,
									 uint8_t saveResp,
								   char *expResp,
								   uint16_t timeoutMs);

/*!
 * @brief : This command is used to only send the command.  Unsolicited response can be checked from intf_at_UnsolRespCallback task
 */
intf_at_fnStatus_t intf_at_SendOnlyCmd(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint16_t timeoutMs);

/**
 * @brief Function to check the receive buffer have the source string
 * @param pSourceStr -  String to be checked
 * @return the buffer if success or NULL
 */
uint8_t *intf_at_pUnsolRespChecker(char *pSourceStr);

/**
 * @brief Based on this flag , unsolicited response checker will run or not run
 * @param flag - 1 to set as running state or 0 to stop it
 */
void intf_at_SetUnsolRespCheckerFlag(uint8_t flag);

uint8_t intf_at_GetUnsolRespCheckerFlag(void);

void intf_at_ClearRecvBuff(void);

#endif /* INTF_INTF_AT_H_ */
