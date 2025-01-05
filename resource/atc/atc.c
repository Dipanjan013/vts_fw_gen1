/*
 * atc.c
 *
 *  Created on: Dec 25, 2024
 *      Author: Dipan
 */
//! Includes
#include "atc.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h"

//! Macros

#define CRLF "\r\n"

/*!********************************************************
 * 										Transmit commands
 **********************************************************/

//Basic command
#define ATC_TEST_CMD "AT"CRLF
#define ATC_IMSI_CMD "AT+CIMI"CRLF
#define ATC_ICCID_CMD "AT+ICCID"CRLF
#define ATC_MODEM_INFO_CMD "AT+CGMM"CRLF

//Network command
#define ATC_NW_REG_STATUS_CMD "AT+CREG?"CRLF
#define ATC_NW_RSSI_CHECK_CMD "AT+CSQ"CRLF
#define ATC_NW_OP_NAME_CMD "AT+COPS?"CRLF
#define ATC_NW_DEREG_CMD "AT+CGATT=0"CRLF
#define ATC_NW_REREG_CMD "AT+CGATT=1"CRLF
#define ATC_NW_SCAN_AVAIL_CMD "AT+COPS=?"CRLF

//MQTT commands
#define ATC_MQTT_CREATE_CMD "AT+MQTTCREATE"CRLF
#define ATC_MQTT_CONNECT_CMD "AT+MQTTCONNECT"CRLF
#define ATC_MQTT_DISCONNECT_CMD "AT+MQTTDISCONNECT"CRLF
#define ATC_MQTT_PUBLISH_CMD "AT+MQTTPUBLISH"CRLF
#define ATC_MQTT_SUBSCRIBE_CMD "AT+MQTTSUBSCRIBE"CRLF

//SIM command
#define ATC_SIM_CHANNEL_CHECK_CMD "AT^SIMSWAP"CRLF
#define ATC_SIM_SWAP_ESIM_CMD "AT+SIMSWAP=0"CRLF
#define ATC_SIM_SWAP_EXT_SIM_CMD "AT+SIMSWAP=1"CRLF

//Modem specific command
#define ATC_CAVLI_HUBBLE_REG_STATUS_CMD "AT+HUBBLEREG?"CRLF
#define ATC_CAVLI_REBOOT_CMD ""CRLF

#define ATC_MAX_CMD ""

//! Global Variables

extern port_uart_handle_t gHuart1;
extern port_uart_handle_t gHuart2;

//! User defined data types

static atc_respQ_t gAtcRespQueue = {0};

struct recvDataStruct{
	uint8_t byte;
	uint16_t len;
	uint8_t buffer[ATC_MAX_BUFF_SIZE + 1];
}gRecvData;

static atc_unsolResp_t gUnsolResp[] = {
	{ATC_RESP_OK, "OK"},
	{ATC_RESP_ERROR, "ERROR"},
	{ATC_RESP_CREG, "+CREG"},
	{ATC_RESP_ICCID, "+ICCID"},
	{ATC_RESP_CSQ, "+CSQ"},
	{ATC_RESP_MAX, ""},
};

//!************************* Private Functions*******************/

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
				gRecvData.buffer[gRecvData.len] = gRecvData.byte;
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

atc_unsolRespCodes_t atc_LookUpResp(char *pTargetStr)
{
	if(NULL != strstr(pTargetStr, gUnsolResp[ATC_RESP_ERROR].respStr)){
		return ATC_RESP_ERROR;
	}
	for(atc_unsolRespCodes_t i = ATC_RESP_OK; i < ATC_RESP_MAX; i++){
		if(NULL != strstr(pTargetStr, gUnsolResp[i].respStr)){
			return gUnsolResp[i].code;
		}
	}
	return ATC_RESP_MAX;	//match not found
}

static void SaveResponse(atc_unsolRespCodes_t type , uint8_t *data)
{
	memset(&gAtcRespQueue.respData[gAtcRespQueue.validRespCnt], 0, sizeof());
	gAtcRespQueue.respData.type = type;
	memcpy(gAtcRespQueue.respData.data, data, ATC_RX_DATA_MAX_LEN);
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

static void ResponseCheckerTask(void)
{
	while(1){
		if((gRecvData.len > 2) && (gRecvData.byte == '\n')){
			//Check for valid response
			for(atc_unsolRespCodes_t i = ATC_RESP_OK; i < ATC_RESP_MAX; i++){
				if(NULL != strstr(gRecvData.buffer, gUnsolResp[i].respStr)){
					printf("Response matched\r\n");
					SaveResponse();
				}else{
					printf("Response didn't match\r\n");
					ClearRecv();
				}
			}
		}
		osDelay(1);
	}
}

void atc_StartReceiverTask(void)
{

}
