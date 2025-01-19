//!Includes
#include <string.h>
#include "intf_at.h"
#include "port_timer.h"

//! Macro

//! User-defined data type

//! Private variables
static uint8_t gBuff[INTF_AT_RX_DATA_MAX + 1] = {0};
static uint8_t gRxByte = 0;
static uint16_t gIndex = 0;

static uint8_t gUnsolRespCheckerFlag = 0;

static const intf_ble_unsolRespTable_t gUnsolRespTable[] = {
	{UNSOL_RESP_CODE_MT_MSG, "+CIMI"},
};

/*******************************************************************************************************************
 *
 ******************************************************************************************************************/

static void ClearRecv(void)
{
	memset(gBuff, 0, sizeof(gBuff));
	gRxByte = 0;
	gIndex = 0;
}

static inline uint8_t IsSubStrPresent(char *substr)
{
	return (NULL != strstr((char*)gBuff, substr));
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
									 uint8_t saveResp,
								   char *expResp,
								   uint16_t timeoutMs)
{
	intf_at_fnStatus_t  ret = INTF_AT_FN_STATUS_OK;
	if(gUnsolRespCheckerFlag){
		return INTF_AT_FN_STATUS_BUSY;
	}
	port_uart_fnStatus_t ret = port_uart_Transmit(handle, cmd, cmdLen, timeoutMs);
	if(PORT_UART_FN_STATUS_OK != ret){
		return INTF_AT_FN_STATUS_FAIL;
	}

	ret = INTF_AT_FN_STATUS_TIMEOUT;
	while(timeoutMs--){
		if((gRxByte == '\n') && (gIndex > 2)){
				if(NULL != expResp){	//when expected response present, only search for this response else general search
					if(IsSubStrPresent(expResp)){
							ret = INTF_AT_FN_STATUS_OK;
							break;
					}
				}else{
					if(IsSubStrPresent("OK")){
							ret = INTF_AT_FN_STATUS_OK;
							break;
					}
					if(IsSubStrPresent("ERROR")){
							ret = INTF_AT_FN_STATUS_FAIL;
							break;
					}
				}
		}
		port_timer_DelayMs(1);
	}
	if(saveResp){
		memcpy(rxBuff, gBuff, size);
	}
	ClearRecv();
	return ret;
}

void intf_at_UnsolRespChecker(void)
{
	static const uint16_t numOfItems = sizeof(gUnsolRespTable)/sizeof(intf_ble_unsolRespTable_t);
	static intf_ble_unsolRespParam_t param = {0};
	if(!gUnsolRespCheckerFlag){
		return;
	}
	if((gRxByte == '\n') && (gIndex > 2)){
		for(uint16_t i = 0; i < numOfItems; i++){
			if(IsSubStrPresent(gUnsolRespTable[i].respStr)){
				memset(&param, 0, sizeof(intf_ble_unsolRespParam_t));
				param.respCode = gUnsolRespTable[i].respcode;
				memcpy(param.data, gBuff, INTF_BLE_UNSOL_RESP_DATA_MAX);
				intf_at_UnsolRespCallback(&param);
			}
		}
	}
}
