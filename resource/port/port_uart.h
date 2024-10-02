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

#define PORT_UART_MODE_INTERRUPT 1

typedef enum port_uart_tagFnStatus{
	PORT_UART_FN_STATUS_INVALID_ARG = -3,
	PORT_UART_FN_STATUS_ERR = -2,
	PORT_UART_FN_STATUS_TIMEOUT = -1,
	PORT_UART_FN_STATUS_OK = 0,
	PORT_UART_FN_STATUS_BUSY = 1,
}port_uart_fnStatus_t;

#if PORT_UART_MODE_INTERRUPT
typedef enum port_uart_tagCbStatus{
	PORT_UART_CB_NONE = 0,
	PORT_UART_TX_CMPLT,
	PORT_UART_RX_CMPLT,
	PORT_UART_XFER_ERR,
}port_uart_callback_t;

typedef UART_HandleTypeDef port_uart_handle_t;
//void port_uart_Callback(port_uart_callback_t cbType, port_uart_handle_t *handle);
#endif
port_uart_fnStatus_t port_uart_Init(port_uart_handle_t *uartHndl);

#if PORT_UART_MODE_INTERRUPT
port_uart_fnStatus_t port_uart_Transmit(port_uart_handle_t *uartHndl, uint8_t *txBuff, uint16_t txLen);
port_uart_fnStatus_t port_uart_Receive(port_uart_handle_t *uartHndl, uint8_t *rxBuff, uint16_t buffSize);

#else
port_uart_fnStatus_t port_uart_Transmit(port_uart_handle_t *uartHndl, uint8_t *txBuff, uint16_t txLen);
port_uart_fnStatus_t port_uart_Receive(port_uart_handle_t *uartHndl, uint8_t *rxBuff, uint16_t buffSize, uint32_t timeoutMs);
#endif

port_uart_fnStatus_t port_uart_DeInit(port_uart_handle_t *uartHndl);

#endif /* PORT_PORT_UART_H_ */
