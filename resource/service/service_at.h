/*
 * service_at.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_AT_H_
#define SERVICE_SERVICE_AT_H_

#include "intf_at.h"

typedef enum service_at_tagcmdType{
	AT_EXE_TEST,
	AT_READ_MFG_ID,
	AT_READ_MODEL_REV,
	AT_READ_MODEL_INFO,
	AT_READ_IMSI,
	AT_READ_MFG_INFO,
	AT_READ_CLK,
	AT_SET_CLK,
	AT_READ_ICCID,
	AT_READ_FREE_MEM,
	AT_READ_SIM_SLOT,
	AT_SET_ESIM,
	AT_SET_EXTSIM,
	AT_READ_NW_REG_STAT,
	AT_READ_CSQ,
	AT_EXE_RESET,
	AT_SET_ECHO,
	AT_MAX  //end
}service_at_cmd_t;

uint8_t service_at_Set(port_uart_handle_t *handle, service_at_cmd_t type, uint8_t *txBuff, uin16_t size, uint16_t timeoutMs);

//uint8_t service_at_Test(port_uart_handle_t *handle, service_at_cmd_t type, uint16_t timeoutMs);

uint8_t service_at_Read(port_uart_handle_t *handle, service_at_cmd_t type, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);

uint8_t service_at_Execute(port_uart_handle_t *handle, service_at_cmd_t type, uint16_t timeoutMs);

/*!
 * @brief : Sends "AT" and wait for OK/ERROR response
 * @return 1 if OK else 0
 */
uint8_t service_at_AT(port_uart_handle_t *handle, uint16_t timeoutMs);
/*!
 * @brief : Command to get the IMSI
 */
uint8_t service_at_GetIMSI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Command to get the IMEI number
 */
uint8_t service_at_GetIMEI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Command to get the ICCID of SIM card
 */
uint8_t service_at_GetICCID(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Get complete information from the module. Ref - check SERVICE_AT_MFG_SPEC_INFO_STR
 */
uint8_t service_at_GetCmpltInfo(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Command to get Modem information
 */
uint8_t service_at_GetModemInfo(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Command to check network registration status
 */
uint8_t service_at_IsNwReg(port_uart_handle_t *handle, uint16_t timeoutMs);

/*!
 * @brief : Command to get the IMSI
 */
uint8_t service_at_CheckSignalStrength(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);
uint8_t service_at_ManNwSearch(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);
uint8_t service_at_DeRegNw(port_uart_handle_t *handle, uint16_t timeoutMs);
uint8_t service_at_ReRegNw(port_uart_handle_t *handle, uint16_t timeoutMs);
uint8_t service_at_CheckSimChannel(port_uart_handle_t *handle, uint8_t channel, uint16_t timeoutMs);

#endif /* SERVICE_SERVICE_AT_H_ */
