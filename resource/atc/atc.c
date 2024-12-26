/*
 * atc.c
 *
 *  Created on: Dec 25, 2024
 *      Author: Dipan
 */
#include "atc.h"

port_uart_handle_t gHuart1;
port_uart_handle_t gHuart2;

typedef struct{
	uint8_t gBuff[ATC_MAX_BUFF_SIZE + 1];
	uint16_t len;
}recvData_t;

static recvData_t gRecvData1 = {0};
uint8_t gByte1 = 0, gValidDataFlag = 0, gErrorDataFlag = 0;

void port_uart_Callback(port_uart_handle_t *huart, port_uart_cb_id_t id)
{
	switch(id){
		case PORT_UART_CB_ID_TX_CMPLT:
			break;
		case PORT_UART_CB_ID_RX_CMPLT:{
			if(huart == (&gHuart1)){
				gRecvData1.gBuff[len] = gByte1;
				gRecvData1.len = (gRecvData1.len + 1) % ATC_MAX_BUFF_SIZE;
				port_uart_Receive(huart, &gByte1, 1);
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
	port_uart_fnStatus_t ret = 0;
	atc_fnStatus_t rc = 0;
	timeoutMs = 500;
	char sendCmd[ATC_TX_DATA_MAX_LEN + 1] = {0};
	strncpy(sendCmd, atc_cmd_GetCmdStr(pData->cmd), ATC_TX_DATA_MAX_LEN);
	if(sendCmd[0] == '\0' || handle == NULL || timeoutMs == 0){
		rc = ATC_FN_STATUS_ERR_PARAM;
		goto __exit_point;
	}

	ret = port_uart_Transmit(handle, (uint8_t*)sendCmd, strlen(sendCmd), timeoutMs);
	if(ret != PORT_UART_FN_STATUS_OK){
		return ATC_FN_STATUS_FAIL;
	}

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
		osDelay(1);
	}



__exit_point:
	return rc;
}

void CheckForValidData(void)
{
	static atc_response_t response = 0xFF;
	if((gRecvData1.len > 2) && (gByte == 0x0A)){	//LF from CRLF
		response = atc_cmd_SearchResponse((char*)gRecvData1.gBuff);
		if(response == ATC_RESP_OK){
			gValidDataFlag = 1;
		}else if(response == ATC_RESP_ERROR){
			gErrorDataFlag = 1;
		}
	}
}
