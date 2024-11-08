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

#ifdef PORT_UART_MODE_INTERRUPT

__WEAK void port_uart_Callback(port_uart_handle_t *huart, port_uart_cb_id_t id)
{
	(void)(huart);
	(void)(id);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	port_uart_Callback(huart, PORT_UART_CB_ID_TX_CMPLT);
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	port_uart_Callback(huart, PORT_UART_CB_ID_RX_CMPLT);
//}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	(void)(Size);
	port_uart_Callback(huart, PORT_UART_CB_ID_RX_CMPLT);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	port_uart_Callback(huart, PORT_UART_CB_ID_XFER_ERR);
}
#endif

port_uart_fnStatus_t port_uart_Init(port_uart_handle_t *uartHndl)
{
	uartHndl->Init.WordLength = UART_WORDLENGTH_8B;
	uartHndl->Init.StopBits = UART_STOPBITS_1;
	uartHndl->Init.Parity = UART_PARITY_NONE;
	uartHndl->Init.Mode = UART_MODE_TX_RX;
	uartHndl->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uartHndl->Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_StatusTypeDef ret = HAL_UART_Init(uartHndl);
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

#ifdef PORT_UART_MODE_INTERRUPT
port_uart_fnStatus_t port_uart_Transmit(port_uart_handle_t *uartHndl, uint8_t *txBuff, uint16_t txLen)
{
	HAL_StatusTypeDef ret = HAL_UART_Transmit_IT(uartHndl, txBuff, txLen);
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
	HAL_StatusTypeDef ret = HAL_UARTEx_ReceiveToIdle_IT(uartHndl, rxBuff, buffSize);
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
	HAL_StatusTypeDef ret = HAL_UART_DeInit(uartHndl);
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
