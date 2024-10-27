/*
 * port_uart.c
 *
 *  Created on: Sep 1, 2024
 *      Author: Dipan
 */
#include "port_uart.h"

/***********************************************************************************
 *															Private Functions
 **********************************************************************************/

__WEAK void port_uart_Callback(port_uart_callback_t cbType, UART_HandleTypeDef *huart)
{
	;
}

#if PORT_UART_MODE_INTERRUPT
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	port_uart_Callback(PORT_UART_TX_CMPLT, huart);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	port_uart_Callback(PORT_UART_RX_CMPLT, huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	port_uart_Callback(PORT_UART_XFER_ERR, huart);
}
#endif

port_uart_fnStatus_t port_uart_Init(port_uart_handle_t *uartHndl)
{
	uartHndl->handle.Init.WordLength = UART_WORDLENGTH_8B;
	uartHndl->handle.Init.StopBits = UART_STOPBITS_1;
	uartHndl->handle.Init.Parity = UART_PARITY_NONE;
	uartHndl->handle.Init.Mode = UART_MODE_TX_RX;
	uartHndl->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uartHndl->handle.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_StatusTypeDef ret = HAL_UART_Init(&uartHndl->handle);
	switch(ret){
		case HAL_OK:
			return PORT_UART_FN_STATUS_OK;
		case HAL_BUSY:
			return PORT_UART_FN_STATUS_BUSY;
		case HAL_TIMEOUT:
			return PORT_UART_FN_STATUS_TIMEOUT;
		case HAL_ERROR:
			return PORT_UART_FN_STATUS_ERR;
		default:
			return PORT_UART_FN_STATUS_ERR;
	}
}

#if PORT_UART_MODE_INTERRUPT
port_uart_fnStatus_t port_uart_Transmit(port_uart_handle_t *uartHndl, uint8_t *txBuff, uint16_t txLen)
{
	uartHndl->cbType = PORT_UART_CB_NONE;
	HAL_StatusTypeDef ret = HAL_UART_Transmit_IT(&uartHndl->handle, txBuff, txLen);
	switch(ret){
		case HAL_OK:
			return PORT_UART_FN_STATUS_OK;
		case HAL_BUSY:
			return PORT_UART_FN_STATUS_BUSY;
		case HAL_TIMEOUT:
			return PORT_UART_FN_STATUS_TIMEOUT;
		case HAL_ERROR:
			return PORT_UART_FN_STATUS_ERR;
		default:
			return PORT_UART_FN_STATUS_ERR;
	}
}

port_uart_fnStatus_t port_uart_Receive(port_uart_handle_t *uartHndl, uint8_t *rxBuff, uint16_t buffSize)
{
	__HAL_UART_CLEAR_OREFLAG(&uartHndl->handle);
	HAL_UART_AbortReceive_IT(&uartHndl->handle);		//in case this command is already sent  previously
	uartHndl->cbType = PORT_UART_CB_NONE;
	HAL_StatusTypeDef ret = HAL_UARTEx_ReceiveToIdle_IT(&uartHndl->handle, rxBuff, buffSize);
	switch(ret){
		case HAL_OK:
			return PORT_UART_FN_STATUS_OK;
		case HAL_BUSY:
			return PORT_UART_FN_STATUS_BUSY;
		case HAL_TIMEOUT:
			return PORT_UART_FN_STATUS_TIMEOUT;
		case HAL_ERROR:
			return PORT_UART_FN_STATUS_ERR;
		default:
			return PORT_UART_FN_STATUS_ERR;
	}
}
#else
port_uart_fnStatus_t port_uart_Transmit(port_uart_handle_t *uartHndl, uint8_t *txBuff, uint16_t txLen)
{
	HAL_StatusTypeDef ret = HAL_UART_Transmit(uartHndl, txBuff, txLen, HAL_MAX_DELAY);
	switch(ret){
		case HAL_OK:
			return PORT_UART_FN_STATUS_OK;
		case HAL_BUSY:
			return PORT_UART_FN_STATUS_BUSY;
		case HAL_TIMEOUT:
			return PORT_UART_FN_STATUS_TIMEOUT;
		case HAL_ERROR:
			return PORT_UART_FN_STATUS_ERR;
		default:
			return PORT_UART_FN_STATUS_ERR;
	}
}

port_uart_fnStatus_t port_uart_Receive(port_uart_handle_t *uartHndl, uint8_t *rxBuff, uint16_t buffSize, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret = HAL_UART_Receive(uartHndl, rxBuff, buffSize, timeoutMs);
	switch(ret){
		case HAL_OK:
			return PORT_UART_FN_STATUS_OK;
		case HAL_BUSY:
			return PORT_UART_FN_STATUS_BUSY;
		case HAL_TIMEOUT:
			return PORT_UART_FN_STATUS_TIMEOUT;
		case HAL_ERROR:
			return PORT_UART_FN_STATUS_ERR;
		default:
			return PORT_UART_FN_STATUS_ERR;
	}
}
#endif

port_uart_fnStatus_t port_uart_DeInit(port_uart_handle_t *uartHndl)
{
	HAL_StatusTypeDef ret = HAL_UART_DeInit(&uartHndl->handle);
	switch(ret){
		case HAL_OK:
			return PORT_UART_FN_STATUS_OK;
		case HAL_BUSY:
			return PORT_UART_FN_STATUS_BUSY;
		case HAL_TIMEOUT:
			return PORT_UART_FN_STATUS_TIMEOUT;
		case HAL_ERROR:
			return PORT_UART_FN_STATUS_ERR;
		default:
			return PORT_UART_FN_STATUS_ERR;
	}
}
