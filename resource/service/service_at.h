/*
 * service_at.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_AT_H_
#define SERVICE_SERVICE_AT_H_

#include "intf_at.h"

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
