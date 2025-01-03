/*
 * atc.c
 *
 *  Created on: Dec 25, 2024
 *      Author: Dipan
 */
#include "atc.h"
#include <stdio.h>
#include <string.h>

extern port_uart_handle_t gHuart1;
extern port_uart_handle_t gHuart2;

static atc_respQ_t atcRespQueue[5] = {0};

struct recvDataStruct{
	uint8_t byte;
	uint16_t len;
	uint8_t buffer[512];
}gRecvData;

static void ClearRecv(port_uart_handle_t *handle)
{
	if(handle == (&gHuart1)){
		memset(&gRecvData, 0, sizeof(gRecvData));
	}
}

void port_uart_Callback(port_uart_handle_t *huart, port_uart_cb_id_t id)
{
	switch(id){
		case PORT_UART_CB_ID_TX_CMPLT:
			break;
		case PORT_UART_CB_ID_RX_CMPLT:{
			if(huart == (&gHuart1)){
				gRecvData.buffer[len] = gRecvData.byte;
				gRecvData.len = (gRecvData.len + 1) % ATC_MAX_BUFF_SIZE;
				port_uart_Receive(huart, &gRecvData.byte, 1);
			}
		}break;
		case PORT_UART_CB_ID_XFER_ERR:
			break;
		default:
			break;
	}
}

atc_fnStatus_t atc_Commander(port_uart_handle_t *handle, atc_data_t *pData, uint16_t timeoutMs)
{
	port_uart_fnStatus_t ret;
	atc_fnStatus_t rc;
	timeoutMs = 500;
	char sendCmd[ATC_TX_DATA_MAX_LEN + 1] = {0};
	if(NULL == atc_cmd_LookUpCmdStr(pData->cmd)){
		printf("[%s] Command not found\r\n", __func__);
		rc = ATC_FN_STATUS_FAIL;
		goto __exit_point;
	}
	strncpy(sendCmd, atc_cmd_LookUpCmdStr(pData->cmd), ATC_TX_DATA_MAX_LEN);
	ret = port_uart_Transmit(handle, (uint8_t*)sendCmd, strlen(sendCmd), timeoutMs);
	if(ret != PORT_UART_FN_STATUS_OK){
		printf("[%s] Transmit failed (%d)\r\n", __func__, ret);
		rc = ATC_FN_STATUS_FAIL;
		goto __exit_point;
	}

	//Wait for receive data flags
	while(timeoutMs--){
		if(gValidDataFlag){
			rc = ATC_FN_STATUS_OK;
			break;
		}else if(gErrorDataFlag){
			rc = ATC_FN_STATUS_FAIL;
			goto __exit_point;
		}else if(timeoutMs == 0){
			rc = ATC_FN_STATUS_TIMEOUT;
			goto __exit_point;
		}
//		osDelay(1);
		HAL_Delay(1);
	}
	memcpy(pData->data.rxData, (char*)gRecvData1.gBuff, ATC_MAX_BUFF_SIZE);
	ClearRecv(handle);

__exit_point:
	return rc;
}

void atc_CheckForValidData(void)
{
	static atc_unsolRespCodes_t respCode = ATC_RESP_MAX;
	printf("test :  in %s\r\n", __func__);
	while(1){
		if((gRecvData.len > 2) && (gRecvData.byte == '\n')){
			respCode = atc_LookUpResp((char*)gRecvData.buffer);
			if(ATC_RESP_MAX != respCode){	//valid response
				atcRespQueue[atcRespQueue.validRespCnt].respCode = respCode;
				memcpy(atcRespQueue[atcRespQueue.validRespCnt].respData, gRecvData.buffer, 100);
				ClearRecv(&gHuart1);
				atcRespQueue.validRespCn = (atcRespQueue.validRespCn + 1) % 5;
			}
		}
		HAL_Delay(1);
	}
}

uint8_t atc_Init(port_uart_handle_t *handle)
{
	port_uart_fnStatus_t ret = 0;
	if(handle == (&gHuart1)){
		handle->Instance = USART1;
		handle->Init.BaudRate = 115200;
		ClearRecv(&gHuart1);
	}
	ret = port_uart_Init(handle);
	if(PORT_UART_FN_STATUS_OK != ret){
		return 0;
	}
	if(handle == (&gHuart1)){
		port_uart_Receive(&gHuart1, &gByte1, 1);
	}
	return 1;
}
