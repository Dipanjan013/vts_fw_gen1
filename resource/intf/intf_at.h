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

intf_at_fnStatus_t intf_at_Init(port_uart_handle_t *handle);
intf_at_fnStatus_t intf_at_DeInit(port_uart_handle_t *handle);
intf_at_fnStatus_t intf_at_Command(port_uart_handle_t *handle,
								   uint8_t *cmd,
								   uint16_t cmdLen,
								   uint8_t *rxBuff,
								   uint16_t size,
								   uint8_t *expResp,
								   uint16_t timeoutMs);

#endif /* INTF_INTF_AT_H_ */
