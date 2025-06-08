//Include
#include "service_at.h"
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "usart.h"

//Macros
typedef uint8_t (*CmdHandler_t)(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
typedef uint8_t (*UnsolCmdHandler_t)(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen);

// User-defined data type
typedef struct{
	service_at_cmd_t type;
	char *cmdStr;
	CmdHandler_t handler;
}service_at_cmd_s;

typedef struct{
	service_at_unsolResp_t type;
	char *codeStr;
	UnsolCmdHandler_t unsolRespHandler;
}service_at_unsolRespCmd_s;

//Function prototype
/* Solicited response handlers */
static uint8_t CimiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t AtiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t IccidHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t SimSlotHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t CregHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t CsqHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);

/* Unsolicited response handlers */
static uint8_t UnsolSmsHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen);
static uint8_t UnsolGpsHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen);

//Global Variables
__attribute__((unused)) port_uart_handle_t gpsUartHndl;

/**
 * @brief Maps the command with their respective handlers for parsing the data
 * For those command which doesn't need to parse the received data, it simply checks for OK or ERROR in the received data. Handler is NULL for those.
 */
static service_at_cmd_s gAtCmdTable[AT_MAX] = {
    {AT_EXE_TEST, "AT\r", NULL},
    {AT_READ_IMSI, "AT+CIMI?\r", CimiHandler},
    {AT_READ_MFG_INFO, "ATI\r", AtiHandler},
    {AT_READ_ICCID, "AT+ICCID\r", IccidHandler},
    {AT_READ_SIM_SLOT, "AT^SIMSWAP?\r", SimSlotHandler},
    {AT_SET_ESIM, "AT^SIMSWAP=0\r", NULL},
		{AT_SET_EXTSIM, "AT^SIMSWAP=1\r", NULL},
    {AT_READ_NW_REG_STAT, "AT+CREG?\r", CregHandler},
    {AT_READ_CSQ, "AT+CSQ\r", CsqHandler},
    {AT_EXE_RESET, "AT+TRB\r", NULL},
    {AT_EXE_ECHO_OFF, "ATE0\r", NULL},

		//GPS Commands
		{AT_EXE_GPS_ON, "AT+CGPS=1\r", NULL},
		{AT_EXE_GPS_OFF, "AT+CGPS=0\r", NULL},
		{AT_READ_GPS_STREAM, "AT+GPSPORT=1\r", NULL},
		{AT_EXE_GPS_STREAM_STOP, "AT+GPSPORT=0\r", NULL},
};

static service_at_unsolRespCmd_s gUnsolRespCmdTable[AT_UNSOL_RESP_MAX] = {
		{AT_UNSOL_RESP_SMS, "+CMT", UnsolSmsHandler},
		{AT_UNSOL_RESP_CALL, "+RING", NULL},
		{AT_UNSOL_RESP_GPS, "RMC", UnsolGpsHandler},
};

//Extern variables


/****************************************************************************************************************************************
 *
 ***************************************************************************************************************************************/
/**!
 * @brief It is used to look up the command from the command table.
 * Using this removes the command order mapping over-head. For e.g, if the service_at_cmd_t is changed  and not mapped w.r.t gAtCmdTable[], then the result will be mislead
 * @param type command enum
 * @return Exact command structure index from the command table.
 */
static service_at_cmd_s *pCmdLukUpTbl(service_at_cmd_t type)
{
	for(uint8_t i = 0; i < AT_MAX; i++){
		if(gAtCmdTable[i].type == type){
			return &gAtCmdTable[i];
		}
	}
	return NULL;	//Command not found
}

static uint8_t CimiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[50]={0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	utils_CopyDigitsBuff(temp, rxBuff, size);
	return 1;
}

static uint8_t AtiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}

	//Let the app handle whatever info required
//	utils_StrParser(pMainStr, pSubStrStart, pSubStrEnd, pTargetBuff, size);

	return 1;
}

static uint8_t IccidHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[50] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	utils_CopyDigitsBuff(temp, rxBuff, size);
	return 1;
}

static uint8_t SimSlotHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	uint8_t temp[30] = {0};
	ret = intf_at_Command(handle, cmd, cmdLen, temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	if(strstr((char*)temp, "AT^SIMSWAP: 1")){
		rxBuff[0] = '1';
	}else{
		rxBuff[0] = '0';
	}
	return 1;
}

static uint8_t CregHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[30] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	unsigned int val = 0, stat = 0;
	sscanf((char*)temp, "\r\n+CREG: %u,%u\r\n", &val, &stat);
	if(size >= 2){
		rxBuff[0] = 0x30 + (uint8_t)val;
		rxBuff[1] = 0x30 + (uint8_t)stat;
	}
	return 1;
}

static uint8_t CsqHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[30] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	unsigned int val = 0;
	sscanf((char*)temp, "\r\n+CSQ: %u", &val);
	int rssi = utils_MapCSQ(val);
	snprintf((char*)rxBuff, size, "%d", rssi);
	return 1;
}

static uint8_t UnsolSmsHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen)
{
	if(cmdLen){
		uint8_t temp[cmdLen+1];
		memset(temp, 0, sizeof(cmdLen));
		memcpy(temp, cmd, cmdLen);
		service_at_UnsolRespCallback(AT_UNSOL_RESP_SMS, temp, cmdLen);
	}else{
		service_at_UnsolRespCallback(AT_UNSOL_RESP_SMS, NULL, 0);
	}
	return 1;
}

static uint8_t UnsolGpsHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen)
{
	if(cmdLen){
		uint8_t temp[cmdLen+1];
		memset(temp, 0, sizeof(cmdLen));
		memcpy(temp, cmd, cmdLen);
		service_at_UnsolRespCallback(AT_UNSOL_RESP_GPS, temp, cmdLen);
	}else{
		service_at_UnsolRespCallback(AT_UNSOL_RESP_GPS, NULL, 0);
	}
	return 1;
}

uint8_t service_at_Init(void)
{
	intf_at_fnStatus_t ret = INTF_AT_FN_STATUS_FAIL;
	//Initialize the UART handle for CAVLI C16Qs
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	ret = intf_at_Init(&huart1);
//	ret = intf_at_Init(&gpsUartHndl);	//not using separate module
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}else{
		return 1;
	}
}

uint8_t service_at_Set(service_at_uartInst_t instance, service_at_cmd_t type, uint8_t *txBuff, uint16_t size, uint16_t timeoutMs)
{
	port_uart_handle_t *handle = NULL;
	if(instance == SERVICE_AT_UART_INST0){
			handle = &huart1;
	}else{
			handle = &gpsUartHndl;
	}

	service_at_cmd_s *pCmdInfo = pCmdLukUpTbl(type);
	if(pCmdInfo == NULL){
		printf("Command type [%d] not found!\r\n",  type);
		return 0;
	}

	return pCmdInfo->handler(handle, txBuff, size, NULL, 0, timeoutMs);	//Transmits command with addntl value & checks "OK" or "ERROR" response
}

uint8_t service_at_Read(service_at_uartInst_t instance, service_at_cmd_t type, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	port_uart_handle_t *handle = NULL;
	if(instance == SERVICE_AT_UART_INST0){
			handle = &huart1;
	}else{
			handle = &gpsUartHndl;
	}

	service_at_cmd_s *pCmdInfo = pCmdLukUpTbl(type);
	if(pCmdInfo == NULL){
		printf("Command type [%d] not found!\r\n",  type);
		return 0;
	}

	if(pCmdInfo->handler == NULL){
		intf_at_fnStatus_t ret;
		ret = intf_at_Command(handle, (uint8_t*)pCmdInfo->cmdStr, strlen(pCmdInfo->cmdStr), rxBuff, size, 1, NULL, timeoutMs);
		if(INTF_AT_FN_STATUS_OK != ret){
			return 0;
		}else{
			return 1;
		}
	}else{
		return pCmdInfo->handler(handle, (uint8_t*)pCmdInfo->cmdStr, strlen(pCmdInfo->cmdStr), rxBuff, size, timeoutMs);	//Sends the command and saves the response after parsing. "OK" will also mean success
	}
}

uint8_t service_at_Execute(service_at_uartInst_t instance, service_at_cmd_t type, uint16_t timeoutMs)
{
	port_uart_handle_t *handle = NULL;
	if(instance == SERVICE_AT_UART_INST0){
			handle = &huart1;
	}else{
			handle = &gpsUartHndl;
	}

	service_at_cmd_s *pCmdInfo = pCmdLukUpTbl(type);
	if(pCmdInfo == NULL){
		printf("Command type [%d] not found!\r\n",  type);
		return 0;
	}

	if(pCmdInfo->handler == NULL){
		intf_at_fnStatus_t ret;
		ret = intf_at_Command(handle, (uint8_t*)pCmdInfo->cmdStr, strlen(pCmdInfo->cmdStr), NULL, 0, 0, NULL, timeoutMs);
		if(INTF_AT_FN_STATUS_OK != ret){
			return 0;
		}else{
			return 1;
		}
	}else{
		return pCmdInfo->handler(handle, (uint8_t*)pCmdInfo->cmdStr, strlen(pCmdInfo->cmdStr), NULL, 0, timeoutMs);	//sends the command and "OK" is checked
	}
}

uint8_t service_at_Test(service_at_uartInst_t instance)
{
	port_uart_handle_t *handle = NULL;
	if(instance == SERVICE_AT_UART_INST0){
		handle = &huart1;
	}else{
		handle = &gpsUartHndl;
	}
	intf_at_fnStatus_t ret;
	for(uint8_t retry = 0; retry < 10; retry++){
		ret = intf_at_Command(handle, (uint8_t*)"AT\r\n", 4, NULL, 0, 0, NULL, SERVICE_AT_DEF_TIMEOUT_MS);
		if(ret == INTF_AT_FN_STATUS_OK){
			return 1;
		}
	}
	return 0;
}

void service_at_UnsolRespCheckerTask(service_at_uartInst_t instance)
{
	if(!intf_at_GetUnsolRespCheckerFlag()){		// If this flag is not set, the task won't check for unsol response code in the receive buffer. This is used to avoid usage of same buffer for unsol & sol.response code
		return;
	}

	static port_uart_handle_t *handle = NULL;
	static uint8_t *ptr = NULL;
	if(instance == SERVICE_AT_UART_INST0){
			handle = &huart1;
	}else{
			handle = &gpsUartHndl;
	}
	for(uint8_t i = 0; i < AT_UNSOL_RESP_MAX; i++){
		ptr = intf_at_pUnsolRespChecker(gUnsolRespCmdTable[i].codeStr);
		if(NULL != ptr){
			if(gUnsolRespCmdTable[i].unsolRespHandler != NULL){
				gUnsolRespCmdTable[i].unsolRespHandler(handle, ptr, strlen((char*)ptr));
			}else{
				service_at_UnsolRespCallback(gUnsolRespCmdTable[i].type, NULL, 0);
			}
			intf_at_ClearRecvBuff();
		}
	}
}
