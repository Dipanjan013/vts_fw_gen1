/*
 * port_uart.h
 *
 *  Created on: Sep 1, 2024
 *      Author: Dipan
 */

#ifndef PORT_PORT_UART_H_
#define PORT_PORT_UART_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef enum port_uart_tagFnStatus{
	PORT_UART_FN_STATUS_INVALID_ARG = -3,
	PORT_UART_FN_STATUS_ERR = -2,
	PORT_UART_FN_STATUS_TIMEOUT = -1,
	PORT_UART_FN_STATUS_OK = 0,
	PORT_UART_FN_STATUS_BUSY = 1,
}port_uart_fnStatus_t;

typedef enum port_uart_tagCbID{
	PORT_UART_CB_ID_UNDEF,
	PORT_UART_CB_ID_TX_CMPLT,
	PORT_UART_CB_ID_RX_CMPLT,
	PORT_UART_CB_ID_XFER_ERR,
}port_uart_cb_id_t;

typedef UART_HandleTypeDef port_uart_handle_t;

port_uart_fnStatus_t port_uart_Init(port_uart_handle_t *uartHndl);
port_uart_fnStatus_t port_uart_Transmit(port_uart_handle_t *uartHndl, uint8_t *txBuff, uint16_t txLen, uint16_t timeoutMs);
port_uart_fnStatus_t port_uart_Receive(port_uart_handle_t *uartHndl, uint8_t *rxBuff, uint16_t buffSize);
void port_uart_AbortXfer(port_uart_handle_t *uartHndl);
uint8_t port_uart_IsRxDataPending(port_uart_handle_t *uartHndl);
void port_uart_Callback(port_uart_handle_t *huart, port_uart_cb_id_t id);
port_uart_fnStatus_t port_uart_DeInit(port_uart_handle_t *uartHndl);

#endif /* PORT_PORT_UART_H_ */
