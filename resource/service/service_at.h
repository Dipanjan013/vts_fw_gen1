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

/**
 * @enum Commands that are added in command  table gAtCmdTable[] in service_at.c
 * To add your command, add here and respectively in gAtCmdTable[]
 * @note: The command table doesn't care about the order of index as command look up table feature is present
 */
typedef enum service_at_tagcmdType{
	AT_EXE_TEST,        /**< AT */
	AT_READ_MFG_INFO,   /**< ATI. Reads IMEI, vendor ID, mfg ID, HW rev, build date, etc */
	AT_READ_IMSI,       /**< AT_READ_IMSI */
	AT_READ_ICCID,      /**< AT_READ_ICCID */
	AT_READ_SIM_SLOT,   /**< AT_READ_SIM_SLOT */
	AT_SET_ESIM,        /**< AT_SET_ESIM */
	AT_SET_EXTSIM,      /**< AT_SET_EXTSIM */
	AT_READ_NW_REG_STAT,/**< AT_READ_NW_REG_STAT */
	AT_READ_CSQ,        /**< AT_READ_CSQ */
	AT_EXE_RESET,       /**< AT_EXE_RESET */
	AT_EXE_ECHO_OFF,    /**< AT_EXE_ECHO_OFF */
	AT_EXE_GPS_ON,
	AT_EXE_GPS_OFF,
	AT_READ_GPS_STREAM,
	AT_EXE_GPS_STREAM_STOP,
	AT_MAX  //end
}service_at_cmd_t;

/**
 * @enum Unsolicited response commands that are  present in unsolicited response command table
 * To add more unsolicited response codes, add here and respectively in
 * @note
 */
typedef enum service_at_tagUnsolResp{
	AT_UNSOL_RESP_SMS, /**< AT_UNSOL_RESP_SMS */
	AT_UNSOL_RESP_CALL,/**< AT_UNSOL_RESP_CALL */
	AT_UNSOL_RESP_GPS,
	AT_UNSOL_RESP_MAX  /**< AT_UNSOL_RESP_MAX */
}service_at_unsolResp_t;

__WEAK void service_at_UnsolRespCallback(service_at_unsolResp_t type, uint8_t *buff, uint16_t len);

uint8_t service_at_Init(void);

uint8_t service_at_Set(service_at_uartInst_t instance, service_at_cmd_t type, uint8_t *txBuff, uint16_t size, uint16_t timeoutMs);

//uint8_t service_at_Test(port_uart_handle_t *handle, service_at_cmd_t type, uint16_t timeoutMs);

uint8_t service_at_Read(service_at_uartInst_t instance, service_at_cmd_t type, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);

uint8_t service_at_Execute(service_at_uartInst_t instance, service_at_cmd_t type, uint16_t timeoutMs);

void service_at_UnsolRespCheckerTask(service_at_uartInst_t instance);

#endif /* SERVICE_SERVICE_AT_H_ */
