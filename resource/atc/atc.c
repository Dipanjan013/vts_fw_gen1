/*
 * atc..c
 *
 *  Created on: Sep 10, 2024
 *      Author: Dipan
 */

/*! Includes */
#include <stdio.h>
#include <string.h>

#include "atc.h"
#include "atc_utils.h"
#include "port_uart.h"
#include <cmsis_os2.h>

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
static port_uart_fnStatus_t SendCmd(atc_module_t module, uint8_t *txBuff, uint16_t len, uint16_t timeoutMs);
static port_uart_fnStatus_t RecvCmd(atc_module_t module, uint8_t *rxBuff, uint16_t buffSize, uint16_t timeoutMs);


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
void port_uart_Callback(port_uart_callback_t cbType, UART_HandleTypeDef *huart)
{
	if(huart == &gsmHandle.handle){
		gsmHandle.cbType = cbType;
	}
}

static port_uart_fnStatus_t SendCmd(atc_module_t module, uint8_t *txBuff, uint16_t len, uint16_t timeoutMs)
{
	port_uart_fnStatus_t ret = PORT_UART_FN_STATUS_ERR;
	port_uart_handle_t *handle = NULL;
	/* Assign the uart handle*/
	if(ATC_LTE_MODULE == module){
		handle = &gsmHandle;
	}
	ret = port_uart_Transmit(handle, txBuff, len);
	if(ret != PORT_UART_FN_STATUS_OK){
		return ret;
	}
	while((0 < timeoutMs--) && (handle->cbType == PORT_UART_CB_NONE)){
		osDelay(1);
	}
	if(handle->cbType == PORT_UART_TX_CMPLT){
		ret = PORT_UART_FN_STATUS_OK;
	}else if(handle->cbType == PORT_UART_XFER_ERR){
		ret = PORT_UART_FN_STATUS_ERR;
	}else{
		ret = PORT_UART_FN_STATUS_TIMEOUT;
	}
//	DBG_LOG("ret = %d", ret);
	return ret;
}

static port_uart_fnStatus_t RecvCmd(atc_module_t module, uint8_t *rxBuff, uint16_t buffSize, uint16_t timeoutMs)
{
	port_uart_fnStatus_t ret = PORT_UART_FN_STATUS_ERR;
	port_uart_handle_t *handle = NULL;
	/* Assign the uart handle*/
	if(ATC_LTE_MODULE == module){
		handle = &gsmHandle;
	}
	ret = port_uart_Receive(handle, rxBuff, buffSize);
	if(ret != 0){
		return ret;
	}
	while((0 < timeoutMs--) && (handle->cbType == PORT_UART_CB_NONE)){
		osDelay(1);
	}
	if(handle->cbType == PORT_UART_RX_CMPLT){
		ret = PORT_UART_FN_STATUS_OK;
	}else if(handle->cbType == PORT_UART_XFER_ERR){
		ret = PORT_UART_FN_STATUS_ERR;
	}else{
		ret = PORT_UART_FN_STATUS_TIMEOUT;
	}
//	DBG_LOG("ret = %d, data = %s", ret, rxBuff);
	return ret;
}

int8_t atc_Init(void)
{
	gsmHandle.handle.Instance = USART1;
	gsmHandle.handle.Init.BaudRate = 115200;
	port_uart_fnStatus_t ret = port_uart_Init(&gsmHandle);
	VERIFY_AND_RETURN(ret, -1, __func__, __LINE__);
	return 0;
}

uint8_t HandleTest(uint8_t *buff, uint16_t buffSize)
{
	int ret;
	uint8_t rxBuff[10] = {0};
	while(1){
		ret = SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_TEST, strlen(AT_TEST), 1000);
		printf("send cmd = %d\r\n", ret);
		memset(rxBuff, 0, sizeof(rxBuff));
		ret = RecvCmd(ATC_LTE_MODULE, rxBuff, 8, 3000);
		printf("Recv cmd = %d\r\n", ret);
		for(int i =0; i<10; i++){
			printf("[0x%X] ", rxBuff[i]);
		}
		puts("");
		osDelay(2000);
	}
	return 0;





//	(void)(buff);
//	(void)(buffSize);
//	uint8_t rxBuff[5];
//	memset(rxBuff, 0, 5);
//	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_TEST, strlen(AT_TEST), DEF_TIMEOUT_MS)){
//		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, 2, 1000)){
//			if(0 == strncmp("OK", (char*)rxBuff, 2)){
//				return 1;	//success
//			}
//		}
//	}
//	return 0;
}

uint8_t HandleGetImsi(uint8_t *buff, uint16_t buffSize)
{
	if(buffSize < ATC_IMSI_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_IMSI_MAX_LEN + 10];	//IMSI\r\nOK\r\n
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_IMSI, strlen(AT_IMSI), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("OK", (char*)rxBuff)){
				sscanf((char*)rxBuff, "%s\r\n", (char*)buff);
				if(buff[0] != '\0'){
					DBG_LOG("data = %s", rxBuff);
					return 1;
				}
			}
		}
	}
	return 0;
}

uint8_t HandleGetIccid(uint8_t *buff, uint16_t buffSize)
{
	if(buffSize < ATC_ICCID_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_ICCID_MAX_LEN + 10];	//IMSI\r\nOK\r\n
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_ICCID, strlen(AT_ICCID), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("+ICCID", (char*)rxBuff)){
				sscanf((char*)rxBuff, "+ICCID: %s\r\n", (char*)buff);
				if(1 == atc_utils_IsDigitBuff(rxBuff, ATC_ICCID_MAX_LEN)){
					return 1;
				}
			}
		}
	}
	return 0;
}

uint8_t HandleGetModemInfo(uint8_t *buff, uint16_t buffSize)
{
	if(buffSize < ATC_MODEM_INFO_MAX_LEN){
		return 0;
	}
	uint8_t rxBuff[ATC_MODEM_INFO_MAX_LEN];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_MODEM_INFO, strlen(AT_MODEM_INFO), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("+CGMM", (char*)rxBuff)){
				sscanf((char*)rxBuff, "+CGMM: %s\r\n", (char*)buff);
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleGetNwRegStatus(uint8_t *buff, uint16_t buffSize)
{
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[20];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_NW_REG, strlen(AT_NW_REG), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("+CREG", (char*)rxBuff)){
				sscanf((char*)rxBuff, "+CREG: %s\r\n", (char*)buff);
				if((NULL != strstr("0,1", (char*)rxBuff)) || (NULL != strstr("0,5", (char*)rxBuff))){
					return 1;
				}
			}
		}
	}
	return 0;
}

uint8_t HandleCheckNwRssi(uint8_t *buff, uint16_t buffSize)
{
	uint8_t rxBuff[20];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_RSSI, strlen(AT_RSSI), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("+CSQ", (char*)rxBuff)){
				uint8_t uc = 0, err = 255;
				sscanf((char*)rxBuff, "+CSQ: %c,%c\r\n", &uc, &err);
				if(err == 0){	//no error
					int16_t rssi = atc_utils_MapSQ(uc);
					if(rssi != 0){
						snprintf((char*)buff, buffSize, "%d", rssi);
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

uint8_t HandleGetNwOperatorName(uint8_t *buff, uint16_t buffSize)
{
	uint8_t rxBuff[50];
	memset(rxBuff, 0, sizeof(rxBuff));
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_NW_OP_NAME, strlen(AT_NW_OP_NAME), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("+COPS", (char*)rxBuff)){
				uint8_t mode = 255;
				uint8_t opNameFormat = 255;
				uint8_t act = 255;	//radio access technology
				sscanf((char*)rxBuff, "+COPS:%c,%c,%s,%c", &mode, &opNameFormat, rxBuff, &act);
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
	}
	return 0;
}

uint8_t HandleNetworkDeregister(uint8_t *buff, uint16_t buffSize)
{
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[10] = {0};
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_NW_DEREG, strlen(AT_NW_DEREG), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("OK", (char*)rxBuff)){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleNetworkReRegister(uint8_t *buff, uint16_t buffSize)
{
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[10] = {0};
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_NW_REREG, strlen(AT_NW_REREG), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("OK", (char*)rxBuff)){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleScanAvailableNetworks(uint8_t *buff, uint16_t buffSize)
{
	//TODO if required
	return 0;
}

uint8_t HandleSimChannelCheck(uint8_t *buff, uint16_t buffSize)
{
	uint8_t rxBuff[20] = {0};
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_SIM_CHANNEL_GET, strlen(AT_SIM_CHANNEL_GET), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("AT^SIMSWAP:", (char*)rxBuff)){
				sscanf((char*)rxBuff, "AT^SIMSWAP:%c", &buff[0]);
				if((buff[0] == '0' || buff[0] == '1')&& (buffSize >= 1)){
					return 1;
				}
			}
		}
	}
	return 0;
}

uint8_t HandleSimSwapEsim(uint8_t *buff, uint16_t buffSize)
{
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[50] = {0};
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_SIMSWAP_ESIM, strlen(AT_SIMSWAP_ESIM), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("OK", (char*)rxBuff)){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleSimSwapExtSim(uint8_t *buff, uint16_t buffSize)
{
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[50] = {0};
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_SIMSWAP_USIM, strlen(AT_SIMSWAP_USIM), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("OK", (char*)rxBuff)){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleHubbleRegStatus(uint8_t *buff, uint16_t buffSize)
{
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[50] = {0};
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_HUBBLE_REG_STATUS, strlen(AT_HUBBLE_REG_STATUS), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("+HUBBLEREG: REGISTERED", (char*)rxBuff)){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t HandleModemReboot(uint8_t *buff, uint16_t buffSize)
{
	(void)(buff);
	(void)(buffSize);
	uint8_t rxBuff[10] = {0};
	if(PORT_UART_FN_STATUS_OK == SendCmd(ATC_LTE_MODULE, (uint8_t*)AT_REBOOT, strlen(AT_REBOOT), DEF_TIMEOUT_MS)){
		if(PORT_UART_FN_STATUS_OK == RecvCmd(ATC_LTE_MODULE, rxBuff, sizeof(rxBuff)-1, DEF_TIMEOUT_MS)){
			if(NULL != strstr("OK", (char*)rxBuff)){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t atc_CmdLookUpTable(atc_cmd_t cmd, uint8_t *buff, uint16_t len)
{
	for(int16_t i = ATC_TEST; i < ATC_MAX; i++){
		if(cmd == cmdRegisterTable[i].cmd){
			printf("Command Found\r\n");
			return cmdRegisterTable[i].handler(buff, len);
		}
		if(i == ATC_MAX){
			printf("[%s] Command not found\r\n", __func__);
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
