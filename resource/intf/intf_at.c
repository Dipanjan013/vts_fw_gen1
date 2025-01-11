//!Includes
#include <string.h>
#include "intf_at.h"

//! Private variables
static uint8_t gBuff[INTF_AT_RX_DATA_MAX + 1] = {0};
static uint8_t gRxByte = 0;
static uint16_t gIndex = 0;

/*******************************************************************************************************************
 *
 ******************************************************************************************************************/

intf_ble_unsolRespTable_t unsolRespTable[] = {
	{INTF_BLE_UNSOL_RESP_NEW_MSG, "+CIMI"},
};

static void ClearRecv(void)
{
	memset(gBuff, 0, sizeof(gBuff));
	gRxByte = 0;
	gIndex = 0;
}

void port_uart_Callback(port_uart_handle_t *huart, port_uart_cb_id_t id)
{
	(void)(huart);
	switch(id){
	case PORT_UART_CB_ID_RX_CMPLT:
		gBuff[gIndex] = gRxByte;
		gIndex = (gIndex + 1) % INTF_AT_RX_DATA_MAX;
		port_uart_Receive(huart, &gRxByte, 1);
		break;
	case PORT_UART_CB_ID_TX_CMPLT:
		break;
	case PORT_UART_CB_ID_XFER_ERR:
		break;
	default:
	}
}

intf_at_fnStatus_t intf_at_Init(port_uart_handle_t *handle)
{
	port_uart_fnStatus_t ret = port_uart_Init(handle);
	if(PORT_UART_FN_STATUS_OK != ret){
		return INTF_AT_FN_STATUS_FAIL;
	}
	port_uart_Receive(handle, &gRxByte, 1);
	return INTF_AT_FN_STATUS_OK;
}
intf_at_fnStatus_t intf_at_DeInit(port_uart_handle_t *handle)
{
	port_uart_fnStatus_t ret = port_uart_DeInit(handle);
	if(PORT_UART_FN_STATUS_OK != ret){
		return INTF_AT_FN_STATUS_FAIL;
	}
	return INTF_AT_FN_STATUS_OK;
}

intf_at_fnStatus_t intf_at_Command(port_uart_handle_t *handle,
								   uint8_t *cmd,
								   uint16_t cmdLen,
								   uint8_t *rxBuff,
								   uint16_t size,
								   uint8_t *expResp,
								   uint16_t timeoutMs)
{
	port_uart_fnStatus_t ret = port_uart_Transmit(handle, cmd, cmdLen, timeoutMs);
	if(PORT_UART_FN_STATUS_OK != ret){
		return INTF_AT_FN_STATUS_FAIL;
	}
	uint8_t flag = 0;	//1= OK/ <expResp> . 0 = ERROR
	//Search "OK","ERROR", "<expResp>"
	char *pSearchStr[3] = {"OK" , "ERROR", expResp};
	uint8_t searchItems = (NULL == expResp) ? 2 : 3;

	while(timeoutMs--){
		for(uint8_t i = 0; i < searchItems; i++){
			if(NULL != strstr((char*)gRecvData, pSearchStr[i])){
				if(i == 0 || i == 2){	// OK or expResp
					flag = 1;
				}
				break;
			}
		}
		HAL_Delay(1);
	}

	if(timeoutMs == 0){
		return INTF_AT_FN_STATUS_TIMEOUT;
	}else if(flag == 0){
		return INTF_AT_FN_STATUS_FAIL;
	}
	else{
		memcpy(rxBuff, gRecvData, size);
		ClearRecv();
	}
	return INTF_AT_FN_STATUS_OK;
}

void intf_at_UnsolRespChecker(void)
{
	if((gRxByte == '\n') && (gIndex > 2)){

	}
}
