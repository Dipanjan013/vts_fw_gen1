#include "service_at.h"
#include <string.h>
#include <stdio.h>
#include "utils.h"

static uint8_t CgmiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t CgmmHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t CgmrHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t CimiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t AtiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t CclkHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t IccidHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t MemStatusHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t SimStatusHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t SimSwapHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t CregHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);
static uint8_t CsqHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);

typedef uint8_t (*CmdHandler_t)(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs);

typedef struct{
	service_at_cmd_t type;
	char *cmdStr;
	CmdHandler_t handler;
}service_at_cmd_s;

service_at_cmd_s gAtCmdSet[AT_MAX] = {
    {AT_EXE_TEST, "AT\r", NULL},
    {AT_READ_MFG_ID, "AT+CGMI=?\r", CgmiHandler},
    {AT_READ_MODEL_REV, "AT+CGMR=?\r", CgmrHandler},
    {AT_READ_MODEL_INFO, "AT+CGMM=?\r", CgmmHandler},
    {AT_READ_IMSI, "AT+CIMI?\r", CimiHandler},
    {AT_READ_MFG_INFO, "ATI\r", AtiHandler},
    {AT_READ_CLK, "AT+CCLK?\r", CclkHandler},
    {AT_SET_CLK, "AT+CCLK=$\r", CclkHandler},
    {AT_READ_ICCID, "AT+ICCID\r", IccidHandler},
    {AT_READ_FREE_MEM, "AT+MEMSTATUS\r", MemStatusHandler},
    {AT_READ_SIM_SLOT, "AT^SIMSWAP=?\r", SimSwapHandler},
    {AT_SET_ESIM, "AT^SIMSWAP=0\r", SimSwapHandler},
	{AT_SET_EXTSIM, "AT^SIMSWAP=1\r", SimSwapHandler},
    {AT_READ_NW_REG_STAT, "AT+CREG?\r", CregHandler},
    {AT_READ_CSQ, "AT+CSQ\r", CsqHandler},
    {AT_EXE_RESET, "AT+TRB\r", NULL},
    {AT_EXE_ECHO_OFF, "ATE0\r", NULL}
};

static uint8_t CgmiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t CgmmHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t CgmrHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t CimiHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
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
	return 1;
}

static uint8_t CclkHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t IccidHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t MemStatusHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t SimStatusHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t SimSwapHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t CregHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

static uint8_t CsqHandler(port_uart_handle_t *handle, uint8_t *cmd, uint16_t cmdLen, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, cmd, cmdLen, rxBuff, size, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

uint8_t service_at_Set(port_uart_handle_t *handle, service_at_cmd_t type, uint8_t *txBuff, uin16_t size, uint16_t timeoutMs)
{
	return gAtCmdSet[type].handler(handle, txBuffr, size, NULL, 0, timeoutMs);	//Transmits command with addntl value & checks "OK" or "ERROR" response
}

uint8_t service_at_Read(port_uart_handle_t *handle, service_at_cmd_t type, uint8_t *rxBuff, uint16_t size, uint16_t timeoutMs)
{
	if(gAtCmdSet[type].handler == NULL){
		intf_at_fnStatus_t ret;
		ret = intf_at_Command(handle, (uint8_t*)gAtCmdSet[type].cmdStr, strlen(gAtCmdSet[type].cmdStr), rxBuff, size, 1, NULL, timeoutMs);
		if(INTF_AT_FN_STATUS_OK != ret){
			return 0;
		}else{
			return 1;
		}
	}else{
		return gAtCmdSet[type].handler(handle, (uint8_t*)gAtCmdSet[type].cmdStr, strlen(gAtCmdSet[type].cmdStr), rxBuff, size, timeoutMs);	//Sends the command and saves the response after parsing. "OK" will also mean success
	}
}

uint8_t service_at_Execute(port_uart_handle_t *handle, service_at_cmd_t type, uint16_t timeoutMs)
{
	if(gAtCmdSet[type].handler == NULL){
		intf_at_fnStatus_t ret;
		ret = intf_at_Command(handle, (uint8_t*)gAtCmdSet[type].cmdStr, strlen(gAtCmdSet[type].cmdStr), NULL, 0, 0, NULL, timeoutMs);
		if(INTF_AT_FN_STATUS_OK != ret){
			return 0;
		}else{
			return 1;
		}
	}else{
		return gAtCmdSet[type].handler(handle, (uint8_t*)gAtCmdSet[type].cmdStr, strlen(gAtCmdSet[type].cmdStr), NULL, 0, timeoutMs);	//sends the command and "OK" is checked
	}
}










//============================================================================//
uint8_t service_at_AT(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, (uint8_t*)SERVICE_AT_STR, strlen(SERVICE_AT_STR), NULL, 0, 0, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

uint8_t service_at_GetIMSI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[50] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, (uint8_t*)SERVICE_AT_CIMI_STR, strlen(SERVICE_AT_CIMI_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	utils_CopyDigitsBuff(temp, rxBuff, size);
	return 1;
}

uint8_t service_at_GetIMEI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[300] = {0};	//to get the full modem info
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, (uint8_t*)SERVICE_AT_MFG_SPEC_INFO_STR, strlen(SERVICE_AT_MFG_SPEC_INFO_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	utils_StrParser((char*)temp, "IMEI: ", "\r", (char*)rxBuff, size);
	return 1;
}

uint8_t service_at_GetICCID(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[50] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, (uint8_t*)SERVICE_AT_ICCID_STR, strlen(SERVICE_AT_ICCID_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	utils_CopyDigitsBuff(temp, rxBuff, size);
	return 1;
}

uint8_t service_at_GetCmpltInfo(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[300] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, (uint8_t*)SERVICE_AT_MFG_SPEC_INFO_STR, strlen(SERVICE_AT_MFG_SPEC_INFO_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

uint8_t service_at_GetModemInfo(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[20] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, (uint8_t*)SERVICE_AT_CGMM_STR, strlen(SERVICE_AT_CGMM_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	utils_SafeSscanfStr((char*)temp, strlen((char*)temp), (char*)rxBuff, size, "\r\n+CGMM:%s\r\n");
	return 1;
}

uint8_t service_at_IsNwReg(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[20] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, (uint8_t*)SERVICE_AT_CREG_READ_STR, strlen(SERVICE_AT_CREG_READ_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	unsigned int stat = 0xFF;
	sscanf((char*)temp, "\r\n+CREG: 0,%u\r\n", &stat);
	if(stat == 1 || stat == 5){
		return 1;
	}else{
		return 0;
	}
}

uint8_t service_at_CheckSignalStrength(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	uint8_t temp[20] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, (uint8_t*)SERVICE_AT_CSQ_STR, strlen(SERVICE_AT_CSQ_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	unsigned int val = 0xFF;
	sscanf((char*)temp, "\r\n+CSQ: %u", &val);
	int rssi = utils_MapCSQ(val);
	snprintf((char*)rxBuff, size, "%d", rssi);
	return 1;
}

uint8_t service_at_ManNwSearch(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	return 1;
}

uint8_t service_at_DeRegNw(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);

	return 1;
}

uint8_t service_at_ReRegNw(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	return 1;
}

uint8_t service_at_CheckSimChannel(port_uart_handle_t *handle, uint8_t channel, uint16_t timeoutMs)
{
	printf("[%s]\r\n", __func__);
	return 1;
}
