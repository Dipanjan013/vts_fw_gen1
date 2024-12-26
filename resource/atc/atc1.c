/*
 * atc..c
 *
 *  Created on: Sep 10, 2024
 *      Author: Dipan
 */

/*! Includes */
#include <atc1.h>
#include <stdio.h>
#include <string.h>
#include <cmsis_os2.h>

#include "atc_utils.h"
#include "port_uart.h"
#include "services.h"

/*! Macros */
#define EOS "\r\n\0"	//end of string

#define AT_TEST "AT"EOS
#define AT_IMSI "AT+CIMI"EOS
#define AT_ICCID "AT+ICCID"EOS
#define AT_MODEM_INFO "AT+CGMM"EOS
#define AT_NW_REG "AT+CREG?"EOS
#define AT_RSSI "AT+CSQ"EOS
#define AT_NW_OP_NAME "AT+COPS?"EOS
#define AT_NW_DEREG "AT+CGATT=0"EOS
#define AT_NW_REREG "AT+CGATT=1"EOS
#define AT_MPLMN "AT+COPS=?"EOS
#define AT_SIM_CHANNEL_GET "AT^SIMSWAP?"EOS
#define AT_SIMSWAP_ESIM "AT+SIMSWAP=0"EOS
#define AT_SIMSWAP_USIM "AT+SIMSWAP=1"EOS
#define AT_REBOOT "AT"EOS

//Modem specific commands
#define AT_HUBBLE_REG_STATUS "AT+HUBBLEREG?"EOS
#define DEF_TIMEOUT_MS (uint16_t)500

#define VERIFY_AND_RETURN(x, y, s, d) \
do{ \
		if(x != 0)  \
		{						\
			printf("[%s %d]err return %d\r\n", s, d, x); \
			return y;   \
		}						\
}while(0);

#define DBG_LOG(fmt, ...) printf("[%s:%d] " fmt "\r\n", __func__, __LINE__, ##__VA_ARGS__)

/*! User-defined  data types */
typedef struct tagCmdRegTable{
	atc_cmd_t cmd;
	uint8_t (*handler)(uint8_t *buff, uint16_t len);
}cmdRegisterTable_t;

typedef struct tagAtcInfo{
	port_uart_handle_t *handle;
	uint8_t byte;
	uint16_t len;
	uint32_t errCode;
	uint8_t buff[ATC_MAX_BUFF_SIZE + 1];
}atc_info_t;

typedef struct tagCmdRespInfo{
	uint8_t *cmd;
	uint16_t cmdLen;
	uint8_t *
}atc_cmdResp_info_t;

/*! Private variables */

/*! Function prototypes */
static uint8_t SendAtCmd(atc_info_t *pInfo,
												 const uint8_t* cmd,
												 uint16_t cmdLen,
												 uint8_t *rxBuff,
												 uint16_t size,
												 const char *expResp,
												 uint8_t saveResp,
												 uint16_t timeoutMs);

static uint8_t HandleTest(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleGetImsi(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleGetIccid(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleGetModemInfo(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleGetNwRegStatus(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleCheckNwRssi(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleGetNwOperatorName(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleNetworkDeregister(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleNetworkReRegister(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleMqttCreate(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleMqttConnect(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleMqttDisconnect(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleMqttPublish(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleMqttSubscribe(uint8_t *buff, uint16_t buffSize);

static uint8_t HandleScanAvailableNetworks(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleSimChannelCheck(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleSimSwapEsim(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleSimSwapExtSim(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleHubbleRegStatus(uint8_t *buff, uint16_t buffSize);
static uint8_t HandleModemReboot(uint8_t *buff, uint16_t buffSize);

/** COMMAND RESPONSE TABLE****/

/**!
 * @note
 * "=?" means whether command is supported or not
 * "=" means command is given to set some values
 * ex : "AT+CGMI" without any operator or '?' operator means to return some information
 */

/*! Variable definition */
static atc_info_t gGsmAtcInfo = {0};
__attribute__((unused)) static atc_info_t gGpsAtcInfo = {0};

static cmdRegisterTable_t cmdRegisterTable[] = {
	{ATC_TEST, HandleTest},                           // Basic Commands
	{ATC_IMSI, HandleGetImsi},
	{ATC_ICCID, HandleGetIccid},
	{ATC_MODEM_INFO, HandleGetModemInfo},

	{ATC_NW_REG_STATUS, HandleGetNwRegStatus},     // Network Commands
	{ATC_NW_RSSI_CHECK, HandleCheckNwRssi},
	{ATC_NW_OP_NAME, HandleGetNwOperatorName},
	{ATC_NW_DEREG, HandleNetworkDeregister},
	{ATC_NW_REREG, HandleNetworkReRegister},
	{ATC_NW_SCAN_AVAIL, HandleScanAvailableNetworks},

	{ATC_MQTT_CREATE, NULL},
	{ATC_MQTT_CONNECT, NULL},
	{ATC_MQTT_PUBLISH, NULL},
	{ATC_MQTT_SUBSCRIBE, NULL},
	{ATC_MQTT_DISCONNECT, NULL},

	{ATC_SIM_CHANNEL_CHECK, HandleSimChannelCheck}, // SIM Card Commands
	{ATC_SIM_SWAP_ESIM, HandleSimSwapEsim},
	{ATC_SIM_SWAP_EXT_SIM, HandleSimSwapExtSim},

	{ATC_CAVLI_HUBBLE_REG_STATUS, HandleHubbleRegStatus}, // Modem Specific Commands
	{ATC_CAVLI_REBOOT, HandleModemReboot},

	// Add additional commands here
};

/*!************************************************************************************************************************
 *
 *************************************************************************************************************************/
static void PrintMe(uint8_t *buff, uint16_t len)
{
	printf("[%s] len = %d\r\n", __func__, len);
	for(uint16_t i=0; i<len; i++){
//		printf("0x%X ", buff[i]);
		if(buff[i] == '\r' || buff[i] == '\n')
			continue;
		printf("%c", buff[i]);
	}
	printf("\r\n");
}

void port_uart_Callback(port_uart_handle_t *huart, port_uart_cb_id_t id)
{
	switch(id){
		case PORT_UART_CB_ID_TX_CMPLT:
			break;
		case PORT_UART_CB_ID_RX_CMPLT:{
			if(gGsmAtcInfo.handle == huart){
				if(gGsmAtcInfo.len <= ATC_MAX_BUFF_SIZE){
					gGsmAtcInfo.buff[gGsmAtcInfo.len++] = gGsmAtcInfo.byte;
					port_uart_Receive(huart, &gGsmAtcInfo.byte, 1);
				}
			}
		}break;
		case PORT_UART_CB_ID_XFER_ERR:
			gGsmAtcInfo.errCode = huart->ErrorCode;
			break;
		default:
			break;
	}
}

static atc_fnStatus_t SendAtCmd(atc_info_t *pInfo,
																const uint8_t *cmd,
																uint16_t cmdLen,
																uint16_t waitMs)
{
	if(NULL == pInfo->handle){
		return ATC_FN_STATUS_ERR_PARAM;
	}
	atc_fnStatus_t ret = ATC_FN_STATUS_FAIL;
	memset(pInfo->buff, 0, ATC_MAX_BUFF_SIZE);	//reset the structure
	pInfo->byte = 0, pInfo->len = 0, pInfo->errCode = 0;
	port_uart_Receive(pInfo->handle, &pInfo->byte, 1);	//in-case communication lost & restored, we must call recv again otherwise it won't be called
	port_uart_fnStatus_t rc = port_uart_Transmit(pInfo->handle, cmd, cmdLen, waitMs);
	if(rc != PORT_UART_FN_STATUS_OK){
		ret = ATC_FN_STATUS_FAIL;
		goto __exit_point;
	}
	while(timeoutMs > 0){
		if(NULL != strstr((char*)pInfo->buff, "OK")){
			ret = ATC_FN_STATUS_OK;
			break;
		}
		if((pInfo->len > 7) && (NULL != strstr((char*)pInfo->buff, "ERROR"))){
			break;
		}
		if(pInfo.errCode > 0){
			goto __exit_point;
		}
		if(0 == (timeoutMs-1)){
			printf("timeout waiting for data\r\n");
			ret = ATC_FN_STATUS_TIMEOUT;
			goto __exit_point;
		}else{
			osDelay(1);
			--timeoutMs;
		}
	}
__exit_point:
	PrintMe(pInfo->buff, pInfo->len);
	return ret;

}

atc_fnStatus_t atc_Commander(port_uart_handle_t *huart, atc_data_t *pData)
{
	atc_fnStatus_t ret = ATC_FN_STATUS_FAIL;
	switch (pData->cmd) {
		// Basic commands
		case ATC_TEST: {
			if(gGsmAtcInfo.handle == huart){
				ret = SendAtCmd(&gGsmAtcInfo, AT_TEST, strlen(AT_TEST), DEF_TIMEOUT_MS);
			}
		} break;
		case ATC_IMSI: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_IMSI, strlen(AT_IMSI), DEF_TIMEOUT_MS);
			if(ATC_FN_STATUS_OK == ret){
				atc_utils_CopyDigitsFromBuff(gGsmAtcInfo.buff, gGsmAtcInfo.len, pData->rxData, ATC_MAX_BUFF_SIZE);
			}
		} break;
		case ATC_ICCID: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_ICCID, strlen(AT_ICCID), DEF_TIMEOUT_MS);
			if(ATC_FN_STATUS_OK == ret){
				atc_utils_CopyDigitsFromBuff(gGsmAtcInfo.buff, gGsmAtcInfo.len, pData->rxData, ATC_MAX_BUFF_SIZE);
			}
		} break;
		case ATC_MODEM_INFO: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_MODEM_INFO, strlen(AT_MODEM_INFO), DEF_TIMEOUT_MS);
			if(ATC_FN_STATUS_OK == ret){
				sscanf((char*)rxBuff+2, "+CGMM: %s\r\n", pData->rxData);
			}
		} break;

		// Network commands
		case ATC_NW_REG_STATUS: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_NW_REG, strlen(AT_NW_REG), DEF_TIMEOUT_MS);
			if(ATC_FN_STATUS_OK == ret){
				if((NULL == strstr((char*)gGsmAtcInfo.buff, "0,1")) || (NULL == strstr((char*)gGsmAtcInfo.buff, "0,2"))){
					//means network not yet registered
					ret = ATC_FN_STATUS_FAIL;
				}
			}
		} break;
		case ATC_NW_RSSI_CHECK: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_RSSI, strlen(AT_RSSI), DEF_TIMEOUT_MS);
			if(ATC_FN_STATUS_OK == ret){
				int val = 999, err = 999;
				sscanf((char*)gGsmAtcInfo.buff + 2, "+CSQ: %d,%d\r\n", &val, &err);
				if(err == 0){	//no error
					int16_t rssi = atc_utils_MapSQ(val);
					if(rssi != 0){
						snprintf(pData->rxData, ATC_MAX_BUFF_SIZE, "%d", rssi);
					}
				}
			}
		} break;
		case ATC_NW_OP_NAME: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_RSSI, strlen(AT_RSSI), DEF_TIMEOUT_MS);
			if(ATC_FN_STATUS_OK == ret){
				int mode = 999;
				int opNameFormat = 999;
				int act = 999;	//radio access technology
				sscanf((char*)gGsmAtcInfo.buff + 2, "+COPS:%d,%d,\"%[^\"]\",%d", &mode, &opNameFormat, pData->rxData, &act);
				if(mode == 0 || mode == 1){	//0 : auto , 1 = manual
				//TODO
				/* Need to map the NW OP name based on operator name format.
				 * opNameFormat = 0 : Long alphanumeric nw op name
				 * opNameFormat = 1 : short alphanumeric nw op name
				 * opNameFormat = 2 : numeric nw op name
				 */
				}
				if(NULL == pData->rxData){
					ret = ATC_FN_STATUS_FAIL;	//parse failed. Err data
				}
			}
		} break;
		case ATC_NW_DEREG: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_NW_DEREG, strlen(AT_NW_DEREG), DEF_TIMEOUT_MS);
		} break;
		case ATC_NW_REREG: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_NW_REREG, strlen(AT_NW_REREG), DEF_TIMEOUT_MS);
		} break;
		case ATC_NW_SCAN_AVAIL: {
				// Handle ATC_NW_SCAN_AVAIL
		} break;

		// MQTT commands
		case ATC_MQTT_CREATE: {
				// Handle ATC_MQTT_CREATE
		} break;
		case ATC_MQTT_CONNECT: {
				// Handle ATC_MQTT_CONNECT
		} break;
		case ATC_MQTT_DISCONNECT: {
				// Handle ATC_MQTT_DISCONNECT
		} break;
		case ATC_MQTT_PUBLISH: {
				// Handle ATC_MQTT_PUBLISH
		} break;
		case ATC_MQTT_SUBSCRIBE: {
				// Handle ATC_MQTT_SUBSCRIBE
		} break;

		// SIM card related commands
		case ATC_SIM_CHANNEL_CHECK: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_SIM_CHANNEL_GET, strlen(AT_SIM_CHANNEL_GET), DEF_TIMEOUT_MS);
			if(ATC_FN_STATUS_OK == ret){
				char ch = 0;
				sscanf((char*)gGsmAtcInfo.buff+2, "AT^SIMSWAP:%c", &ch);
				if(ch == '0'){
					pData->rxData[0] = '0';
				}else if(ch == '1'){
					pData->rxData[0] = '1';
				}else{
					ret = ATC_FN_STATUS_FAIL;	//parse failed. Err data
				}
			}
		} break;
		case ATC_SIM_SWAP_ESIM: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_SIMSWAP_ESIM, strlen(AT_SIMSWAP_ESIM), DEF_TIMEOUT_MS);
		} break;
		case ATC_SIM_SWAP_EXT_SIM: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_SIMSWAP_USIM, strlen(AT_SIMSWAP_USIM), DEF_TIMEOUT_MS);
		} break;

		// Modem specific commands
		case ATC_CAVLI_HUBBLE_REG_STATUS: {
			ret = SendAtCmd(&gGsmAtcInfo, AT_HUBBLE_REG_STATUS, strlen(AT_HUBBLE_REG_STATUS), DEF_TIMEOUT_MS);
			if(ATC_FN_STATUS_OK == ret){
				if(NULL == strstr((char*)gGsmAtcInfo.buff), "+HUBBLEREG: REGISTERED"){
					ret = ATC_FN_STATUS_FAIL;	//parse failed. Err data
				}
			}
		} break;
		case ATC_CAVLI_REBOOT: {
			SendAtCmd(&gGsmAtcInfo, AT_REBOOT, strlen(AT_REBOOT), DEF_TIMEOUT_MS);
		} break;

		// Default case
		default: {
				// Handle unknown command
		} break;
	}
	return ret;
}

int8_t atc_Init(void)
{
	gGsmAtcInfo.handle.Instance = USART1;
	gGsmAtcInfo.handle.Init.BaudRate = 115200;
	port_uart_fnStatus_t ret = port_uart_Init(&gGsmAtcInfo.handle);
	if(ret != PORT_UART_FN_STATUS_OK){
		LOG_E("UART INIT FAILED (%d)\r\n", ret);
	}
	port_uart_Receive(&gGsmAtcInfo.handle, &gGsmAtcInfo.byte, 1);
	if(ret != PORT_UART_FN_STATUS_OK){
		LOG_E("UART START RX FAILED (%d)\r\n", ret);
	}
	return 0;
}

int8_t atc_DeInit(void)
{
	port_uart_fnStatus_t ret = port_uart_DeInit(&gGsmAtcInfo.handle);
	VERIFY_AND_RETURN(ret, -1, __func__, __LINE__);
	return 0;
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&gGsmAtcInfo.handle);
}
