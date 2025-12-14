/*
 * service_at.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_AT_H_
#define SERVICE_SERVICE_AT_H_

#include "intf_at.h"

#define SERVICE_AT_DEF_TIMEOUT_MS 1000U

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
	AT_ECHO_OFF,
	/* Read commands */
	AT_READ_GPS_POS,
	AT_READ_MFG_INFO,   /**< ATI. Reads IMEI, vendor ID, mfg ID, HW rev, build date, etc */
	AT_READ_IMSI,       /**< AT_READ_IMSI */
	AT_READ_ICCID,      /**< AT_READ_ICCID */
	AT_READ_SIM_SLOT,   /**< AT_READ_SIM_SLOT */
	AT_READ_NW_REG_STAT,/**< AT_READ_NW_REG_STAT */
	AT_READ_CSQ,        /**< AT_READ_CSQ */
	AT_READ_MQTT_STATUS,
	AT_READ_PDN_STATUS,
	/* End of read commands */

	/* Write commands */
	AT_WRITE_MQTTCREATE,
	AT_WRITE_MQTTCONNECT,
	AT_WRITE_MQTTPUB,
	AT_WRITE_MQTTSUB,
	AT_WRITE_MQTTDISCONNECT,
	AT_WRITE_MQTTDELETE,
	/* End of Set commands */

	/* Execute commands */
	AT_EXE_TEST,        /**< AT */
	AT_EXE_RESET,       /**< AT_EXE_RESET */
	AT_EXE_ECHO_OFF,    /**< AT_EXE_ECHO_OFF */
	AT_EXE_GPS_ON,
	AT_EXE_GPS_OFF,
	AT_EXE_ESIM,        /**< AT_SET_ESIM */
	AT_EXE_EXTSIM,      /**< AT_SET_EXTSIM */
	/* End of Execute commands */

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
	AT_UNSOL_RESP_MAX  /**< AT_UNSOL_RESP_MAX */
}service_at_unsolResp_t;

__attribute__((weak)) void service_at_UnsolRespCallback(service_at_unsolResp_t type, uint8_t *buff, uint16_t len);

/*!
 * @fn API to initialize the AT command interface layer using UART
 * @return 1 for success else 0
 */
uint8_t service_at_Init(void);

/*!
 * @fn API to send AT command with user data
 * @param instance UART instance
 * @param type command enum
 * @param txBuff user data
 * @param size size of user data
 * @param timeoutMs wait time for response
 * @return 1 for success else 0
 */
uint8_t service_at_Write(service_at_uartInst_t instance, service_at_cmd_t type, uint8_t *txBuff, uint16_t size, uint16_t timeoutMs);

/*!
 * @fn API to test AT command (AT+<CR><LF>  & wait for OK)
 * @param instance UART instance
 * @return 1 for success else 0
 */
uint8_t service_at_Test(service_at_uartInst_t instance);

/*!
 * @fn API to send AT command and wait for it's response. The response is then saved in the rxBuff
 * @brief if command handler is registered, the handler will parse the received data
 * @param instance UART instance
 * @param type command enum
 * @param rxBuff buffer where received response is stored
 * @param size size of the rxBUff
 * @param timeoutMs wait time for response
 * @return 1 for success else 0
 */
uint8_t service_at_Read(service_at_uartInst_t instance, service_at_cmd_t type, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);

/*!
 * @fn API that will send particular AT command from the table and wait for OK/ERROR response. No user data is written in this case
 * @param instance UART instance
 * @param type command enum
 * @param timeoutMs wait time for response
 * @return 1 for success else 0
 */
uint8_t service_at_Execute(service_at_uartInst_t instance, service_at_cmd_t type, uint16_t timeoutMs);

/*!
 * @fn Call this API in a task or super-loop depending upon RTOS or bare-metal architecture
 * @brief This is for unsolicited response checker. If received data matches unsolicited response table, service_at_UnsolRespCallback() is invoked
 * @param instance UART instance
 */
void service_at_UnsolRespCheckerTask(service_at_uartInst_t instance);

#endif /* SERVICE_SERVICE_AT_H_ */
