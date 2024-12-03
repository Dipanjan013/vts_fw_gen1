/*
 * atc..c
 *
 *  Created on: Sep 10, 2024
 *      Author: Dipan
 */

/*! Includes */
#include <stdio.h>
#include <string.h>
#include <cmsis_os2.h>

#include "atc.h"
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
#define DEF_TIMEOUT_MS (uint16_t)5000

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
	port_uart_handle_t handle;
	uint8_t byte;
	uint16_t len;
	uint32_t errCode;
	uint8_t buff[ATC_MAX_BUFF_SIZE+1];
}atc_info_t;

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
			if(&gGsmAtcInfo.handle == huart){
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

static uint8_t SendAtCmd(atc_info_t *pInfo,
												 const uint8_t* cmd,
												 uint16_t cmdLen,
												 uint8_t *rxBuff,
												 uint16_t size,
												 const char *expResp,
												 uint8_t saveResp,
												 uint16_t timeoutMs)
{
	if(pInfo == NULL){
		return 0;
	}

	uint8_t ret = 0;
	memset(pInfo->buff, 0, ATC_MAX_BUFF_SIZE);
	pInfo->byte = 0, pInfo->len = 0, pInfo->errCode = 0;

	port_uart_Receive(&pInfo->handle, &pInfo->byte, 1);	//in-case communication lost & restored, we must call recv again otherwise it won't be called

	port_uart_fnStatus_t rc = port_uart_Transmit(&pInfo->handle, (uint8_t*)cmd, cmdLen, timeoutMs);
	if(rc != PORT_UART_FN_STATUS_OK){
		goto __exit_point;
	}
	while(timeoutMs > 0){
		if(NULL != strstr((char*)pInfo->buff, expResp)){
			ret = 1;
			break;
		}
		if((pInfo->len > 7) && (NULL != strstr((char*)pInfo->buff, "ERROR"))){
			break;
		}
		if(gGsmAtcInfo.errCode > 0){
			printf("Err code  : %lu\r\n", gGsmAtcInfo.errCode);
			goto __exit_point;
		}
		if(0 == (timeoutMs-1)){
			printf("timeout waiting for data\r\n");
			goto __exit_point;
		}else{
			osDelay(1);
			--timeoutMs;
		}
	}
	if(saveResp && (rxBuff != NULL)){
		memcpy(rxBuff, pInfo->buff, size);
	}
__exit_point:
	PrintMe(pInfo->buff, pInfo->len);
	return ret;
}

static uint8_t HandleTest(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	return SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_TEST, strlen(AT_TEST), NULL, 0, "OK", 0, DEF_TIMEOUT_MS);
}

static uint8_t HandleGetImsi(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	if(buffSize < ATC_IMSI_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_IMSI_MAX_LEN + 10];	//IMSI\r\nOK\r\n
	memset(rxBuff, 0, sizeof(rxBuff));
	if(SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_IMSI, strlen(AT_IMSI), rxBuff, sizeof(rxBuff)-1, "OK", 1, DEF_TIMEOUT_MS)){
		atc_utils_CopyDigitsFromBuff(rxBuff, sizeof(rxBuff)-1, buff, buffSize);
		return 1;
	}else{
		return 0;
	}
}

static uint8_t HandleGetIccid(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	if(buffSize < ATC_ICCID_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_ICCID_MAX_LEN + 10];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_ICCID, strlen(AT_ICCID), rxBuff, sizeof(rxBuff)-1, "OK", 1, DEF_TIMEOUT_MS)){
		atc_utils_CopyDigitsFromBuff(rxBuff, sizeof(rxBuff)-1, buff, buffSize);
		return 1;
	}else{
		return 0;
	}
}

static uint8_t HandleGetModemInfo(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	if(buffSize < ATC_MODEM_INFO_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_MODEM_INFO_MAX_LEN];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_MODEM_INFO, strlen(AT_MODEM_INFO), rxBuff, sizeof(rxBuff)-1, "OK", 1, DEF_TIMEOUT_MS)){
		sscanf((char*)rxBuff+2, "+CGMM: %s\r\n", (char*)buff);
		return 1;
	}else{
		return 0;
	}
}

static uint8_t HandleGetNwRegStatus(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[20];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_NW_REG, strlen(AT_NW_REG), rxBuff, sizeof(rxBuff)-1, "OK", 1, DEF_TIMEOUT_MS)){
		if((NULL != strstr((char*)rxBuff, "0,1")) || (NULL != strstr((char*)rxBuff, "0,2"))){
			return 1;
		}
	}
	return 0;
}

static uint8_t HandleCheckNwRssi(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	uint8_t rxBuff[20];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_RSSI, strlen(AT_RSSI), rxBuff, sizeof(rxBuff)-1, "OK", 1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "+CSQ")){
			int val = 999, err = 999;
			sscanf((char*)rxBuff+2, "+CSQ: %d,%d\r\n", &val, &err);
			if(err == 0){	//no error
				int16_t rssi = atc_utils_MapSQ(val);
				if(rssi != 0){
					snprintf((char*)buff, buffSize, "%d", rssi);
					return 1;
				}
			}
		}
	}
	return 0;
}

static uint8_t HandleGetNwOperatorName(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	uint8_t rxBuff[50];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_NW_OP_NAME, strlen(AT_NW_OP_NAME), rxBuff, sizeof(rxBuff)-1, "OK", 1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "+COPS")){
			int mode = 999;
			int opNameFormat = 999;
			int act = 999;	//radio access technology
			sscanf((char*)rxBuff+2, "+COPS:%d,%d,\"%[^\"]\",%d", &mode, &opNameFormat, rxBuff, &act);
			if(mode == 0 || mode == 1){	//0 : auto , 1 = manual
			//TODO
			/* Need to map the NW OP name based on operator name format.
			 * opNameFormat = 0 : Long alphanumeric nw op name
			 * opNameFormat = 1 : short alphanumeric nw op name
			 * opNameFormat = 2 : numeric nw op name
			 */
			 strncpy((char*)buff, (char*)rxBuff, buffSize);
			 return 1;
			}
		}
	}
	return 0;
}

static uint8_t HandleNetworkDeregister(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	return SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_NW_DEREG, strlen(AT_NW_DEREG), NULL, 0, "OK", 0, DEF_TIMEOUT_MS);
}

static uint8_t HandleNetworkReRegister(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	return SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_NW_REREG, strlen(AT_NW_REREG), NULL, 0, "OK", 0, DEF_TIMEOUT_MS);
}

static uint8_t HandleScanAvailableNetworks(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	//TODO if required
	return 0;
}

static uint8_t HandleSimChannelCheck(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	uint8_t rxBuff[20] = {0};
	if(SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_SIM_CHANNEL_GET, strlen(AT_SIM_CHANNEL_GET), rxBuff, sizeof(rxBuff)-1, "OK", 1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "AT^SIMSWAP:")){
			sscanf((char*)rxBuff, "AT^SIMSWAP:%c", &buff[0]);
			if((buff[0] == '0' || buff[0] == '1')&& (buffSize >= 1)){
				return 1;
			}
		}
	}
	return 0;
}

static uint8_t HandleSimSwapEsim(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	return SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_SIMSWAP_ESIM, strlen(AT_SIMSWAP_ESIM), NULL, 0, "OK", 0, DEF_TIMEOUT_MS);
}

static uint8_t HandleSimSwapExtSim(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	return SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_SIMSWAP_USIM, strlen(AT_SIMSWAP_USIM), NULL, 0, "OK", 0, DEF_TIMEOUT_MS);
}

static uint8_t HandleHubbleRegStatus(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	return SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_HUBBLE_REG_STATUS, strlen(AT_HUBBLE_REG_STATUS), NULL, 0, "+HUBBLEREG: REGISTERED", 0, DEF_TIMEOUT_MS);
}

static uint8_t HandleModemReboot(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	return SendAtCmd(&gGsmAtcInfo, (uint8_t*)AT_REBOOT, strlen(AT_REBOOT), NULL, 0, "OK", 0, DEF_TIMEOUT_MS);
}

uint8_t atc_CmdLookUpTable(atc_cmd_t cmd, uint8_t *buff, uint16_t len)
{
	for(int16_t i = ATC_TEST; i < ATC_MAX; i++){
		if(cmd == cmdRegisterTable[i].cmd){
			LOG_V("Command Found\r\n");
			return cmdRegisterTable[i].handler(buff, len);
		}
		if(i == ATC_MAX){
			LOG_W("[%s] Command not found\r\n", __func__);
			break;
		}
	}
	return 0;
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
