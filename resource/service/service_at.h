/*
 * service_at.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_AT_H_
#define SERVICE_SERVICE_AT_H_

#include "intf_at.h"

typedef enum service_at_tagUartInstance{
	SERVICE_AT_UART_INST0 = 0,
	SERVICE_AT_UART_INST1 = 1,
}service_at_uartInst_t;

typedef enum service_at_tagcmdType{
	AT_EXE_TEST,
	AT_READ_MFG_INFO,
	AT_READ_IMSI,
	AT_READ_ICCID,
	AT_READ_SIM_SLOT,
	AT_SET_ESIM,
	AT_SET_EXTSIM,
	AT_READ_NW_REG_STAT,
	AT_READ_CSQ,
	AT_EXE_RESET,
	AT_EXE_ECHO_OFF,
	AT_MAX  //end
}service_at_cmd_t;

uint8_t service_at_Init(void);

uint8_t service_at_Set(service_at_uartInst_t instance, service_at_cmd_t type, uint8_t *txBuff, uint16_t size, uint16_t timeoutMs);

//uint8_t service_at_Test(port_uart_handle_t *handle, service_at_cmd_t type, uint16_t timeoutMs);

uint8_t service_at_Read(service_at_uartInst_t instance, service_at_cmd_t type, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);

uint8_t service_at_Execute(service_at_uartInst_t instance, service_at_cmd_t type, uint16_t timeoutMs);

#endif /* SERVICE_SERVICE_AT_H_ */
