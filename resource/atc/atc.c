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

/*! Private variables */

/*! Function prototypes */
static port_uart_fnStatus_t Commander(atc_module_t module, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t buffSize, uint16_t timeoutMs);


// all type of result will be stored in uint8_t buffer. User needs to parse it
uint8_t HandleTest(uint8_t *buff, uint16_t buffSize);
uint8_t HandleGetImsi(uint8_t *buff, uint16_t buffSize);
uint8_t HandleGetIccid(uint8_t *buff, uint16_t buffSize);
uint8_t HandleGetModemInfo(uint8_t *buff, uint16_t buffSize);
uint8_t HandleGetNwRegStatus(uint8_t *buff, uint16_t buffSize);
uint8_t HandleCheckNwRssi(uint8_t *buff, uint16_t buffSize);
uint8_t HandleGetNwOperatorName(uint8_t *buff, uint16_t buffSize);
uint8_t HandleNetworkDeregister(uint8_t *buff, uint16_t buffSize);
uint8_t HandleNetworkReRegister(uint8_t *buff, uint16_t buffSize);
uint8_t HandleScanAvailableNetworks(uint8_t *buff, uint16_t buffSize);
uint8_t HandleSimChannelCheck(uint8_t *buff, uint16_t buffSize);
uint8_t HandleSimSwapEsim(uint8_t *buff, uint16_t buffSize);
uint8_t HandleSimSwapExtSim(uint8_t *buff, uint16_t buffSize);
uint8_t HandleHubbleRegStatus(uint8_t *buff, uint16_t buffSize);
uint8_t HandleModemReboot(uint8_t *buff, uint16_t buffSize);

/** COMMAND RESPONSE TABLE****/

/**!
 * @note
 * "=?" means whether command is supported or not
 * "=" means command is given to set some values
 * ex : "AT+CGMI" without any operator or '?' operator means to return some information
 */

/*! Variable definition */
port_uart_handle_t gsmHandle;
__attribute__((unused)) port_uart_handle_t gpsHandle;

static volatile uint8_t flagGsmTxCmplt = 0;
static volatile uint8_t flagGsmRxCmplt = 0;
static volatile uint8_t flagGsmErrXfer = 0;

// below for GPS uart
//static volatile uint8_t flagGsmTxCmplt = 0;
//static volatile uint8_t flagGsmRxCmplt = 0;
//static volatile uint8_t flagGsmErrXfer = 0;

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
void port_uart_Callback(port_uart_handle_t *huart, port_uart_cb_id_t id)
{
	if(huart == &gsmHandle){
		switch(id){
			case PORT_UART_CB_ID_TX_CMPLT:
				flagGsmTxCmplt = 1;
				break;
			case PORT_UART_CB_ID_RX_CMPLT:
				flagGsmRxCmplt = 1;
				break;
			case PORT_UART_CB_ID_XFER_ERR:
				flagGsmErrXfer = 1;
				break;
			default:
				break;
		}
	}
}

int8_t atc_Init(void)
{
	gsmHandle.Instance = USART1;
	gsmHandle.Init.BaudRate = 115200;
	port_uart_fnStatus_t ret = port_uart_Init(&gsmHandle);
	VERIFY_AND_RETURN(ret, -1, __func__, __LINE__);
	return 0;
}

static port_uart_fnStatus_t Commander(atc_module_t module, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t buffSize, uint16_t timeoutMs)
{
	port_uart_fnStatus_t ret = PORT_UART_FN_STATUS_ERR;
	port_uart_handle_t *handle = NULL;
	/* Assign the uart handle*/
	if(ATC_LTE_MODULE == module){
		handle = &gsmHandle;
		flagGsmErrXfer = 0;
		flagGsmTxCmplt = 0;
		flagGsmRxCmplt = 0;
		__HAL_UART_FLUSH_DRREGISTER(handle);
		__HAL_UART_CLEAR_OREFLAG(handle);
	}
	ret = port_uart_Receive(handle, rxBuff, buffSize);
	if(PORT_UART_FN_STATUS_OK != ret){
		return ret;
	}
	LOG_V("rx func ret %d\r\n", ret);
	ret = port_uart_Transmit(handle, cmd, cmdLen);
	if(PORT_UART_FN_STATUS_OK != ret){
		return ret;
	}
	LOG_V("tx func ret %d\r\n", ret);
	//Wait for tx complete
	while((0 < timeoutMs--) && (!flagGsmTxCmplt)){
		osDelay(1);
		if(flagGsmErrXfer){
			LOG_V("tx flag err\r\n");
			return PORT_UART_FN_STATUS_ERR;
		}
	}
	if(!flagGsmTxCmplt){
		LOG_V("tx timeout\r\n");
		return PORT_UART_FN_STATUS_TIMEOUT;
	}
	LOG_V("tx success\r\n");
	//Wait for rx complete
	while((0 < timeoutMs--) && (!flagGsmRxCmplt)){
		osDelay(1);
		if(flagGsmErrXfer){
			LOG_V("rx flag err\r\n");
			return PORT_UART_FN_STATUS_ERR;
		}
	}
	if(!flagGsmRxCmplt){
		LOG_V("rx timeout\r\n");
		return PORT_UART_FN_STATUS_TIMEOUT;
	}
	LOG_V("data %s\r\n", rxBuff);
	if()


	return PORT_UART_FN_STATUS_OK;
}

uint8_t HandleTest(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[10+1];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_TEST, strlen(AT_TEST), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "OK")){
			return 1;	//success
		}
	}
	return 0;
}

uint8_t HandleGetImsi(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	if(buffSize < ATC_IMSI_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_IMSI_MAX_LEN + 10];	//IMSI\r\nOK\r\n
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_IMSI, strlen(AT_IMSI), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "OK")){
			sscanf((char*)rxBuff+2, "%s\r\n", (char*)buff);
			if(buff[0] != '\0'){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleGetIccid(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	if(buffSize < ATC_ICCID_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_ICCID_MAX_LEN + 10];	//IMSI\r\nOK\r\n
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_ICCID, strlen(AT_ICCID), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "+ICCID")){
			sscanf((char*)rxBuff+2, "+ICCID: %s\r\n", (char*)buff);
			if(1 == atc_utils_IsDigitBuff(buff, strlen((char*)buff))){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleGetModemInfo(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	if(buffSize < ATC_MODEM_INFO_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_MODEM_INFO_MAX_LEN];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_MODEM_INFO, strlen(AT_MODEM_INFO), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "+CGMM")){
			sscanf((char*)rxBuff+2, "+CGMM: %s\r\n", (char*)buff);
			return 1;
		}
	}
	return 0;
}

uint8_t HandleGetNwRegStatus(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[20];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_NW_REG, strlen(AT_NW_REG), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "+CREG")){
			sscanf((char*)rxBuff+2, "+CREG: %s\r\n", (char*)buff);
			if((NULL != strstr((char*)rxBuff, "0,1")) || (NULL != strstr((char*)rxBuff, "0,5"))){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleCheckNwRssi(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	uint8_t rxBuff[20];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_RSSI, strlen(AT_RSSI), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
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

uint8_t HandleGetNwOperatorName(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	uint8_t rxBuff[50];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_NW_OP_NAME, strlen(AT_NW_OP_NAME), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
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

uint8_t HandleNetworkDeregister(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[10] = {0};
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_NW_DEREG, strlen(AT_NW_DEREG), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "OK")){
			return 1;
		}
	}
	return 0;
}

uint8_t HandleNetworkReRegister(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[10] = {0};
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_NW_REREG, strlen(AT_NW_REREG), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "OK")){
			return 1;
		}
	}
	return 0;
}

uint8_t HandleScanAvailableNetworks(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	//TODO if required
	return 0;
}

uint8_t HandleSimChannelCheck(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	uint8_t rxBuff[20] = {0};
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_SIM_CHANNEL_GET, strlen(AT_SIM_CHANNEL_GET), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "AT^SIMSWAP:")){
			sscanf((char*)rxBuff, "AT^SIMSWAP:%c", &buff[0]);
			if((buff[0] == '0' || buff[0] == '1')&& (buffSize >= 1)){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleSimSwapEsim(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[50] = {0};
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_SIMSWAP_ESIM, strlen(AT_SIMSWAP_ESIM), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "OK")){
			return 1;
		}
	}
	return 0;
}

uint8_t HandleSimSwapExtSim(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[50] = {0};
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_SIMSWAP_USIM, strlen(AT_SIMSWAP_USIM), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "OK")){
			return 1;
		}
	}
	return 0;
}

uint8_t HandleHubbleRegStatus(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[50] = {0};
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_HUBBLE_REG_STATUS, strlen(AT_HUBBLE_REG_STATUS), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "+HUBBLEREG: REGISTERED")){
			return 1;
		}
	}
	return 0;
}

uint8_t HandleModemReboot(uint8_t *buff, uint16_t buffSize)
{
	LOG_V("[%s]\r\n", __func__);
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[10] = {0};
	if(PORT_UART_FN_STATUS_OK == Commander(ATC_LTE_MODULE, (uint8_t*)AT_REBOOT, strlen(AT_REBOOT), rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
		if(NULL != strstr((char*)rxBuff, "OK")){
			return 1;
		}
	}
	return 0;
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

int8_t atc_DeInit(void)
{
	port_uart_fnStatus_t ret = port_uart_DeInit(&gsmHandle);
	VERIFY_AND_RETURN(ret, -1, __func__, __LINE__);
	return 0;
}
