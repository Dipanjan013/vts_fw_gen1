#include "service_at.h"
#include "utils.h"


uint8_t service_at_AT(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, SERVICE_AT_STR, strlen(SERVICE_AT_STR), NULL, 0, 0, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

uint8_t service_at_GetIMSI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	uint8_t temp[50] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, SERVICE_AT_CIMI_STR, strlen(SERVICE_AT_CIMI_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	service_at_CpyDigitsBuff(temp, rxBuff, size);
	return 1;
}

uint8_t service_at_GetIMEI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	uint8_t temp[50] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, SERVICE_AT_CGSN_STR, strlen(SERVICE_AT_CGSN_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	service_at_CpyDigitsBuff(temp, rxBuff, size);
	return 1;
}

uint8_t service_at_GetICCID(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	uint8_t temp[50] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, SERVICE_AT_ICCID_STR, strlen(SERVICE_AT_ICCID_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	service_at_CpyDigitsBuff(temp, rxBuff, size);
	return 1;
}

uint8_t service_at_GetCmpltInfo(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	uint8_t temp[300] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, SERVICE_AT_MFG_SPEC_INFO_STR, strlen(SERVICE_AT_MFG_SPEC_INFO_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

uint8_t service_at_GetModemInfo(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	uint8_t temp[20] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, SERVICE_AT_CGMM_STR, strlen(SERVICE_AT_CGMM_STR), temp, sizeof(temp)-1, 1, NULL, timeoutMs);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	char temp2[20] = {0};
	sscanf((char*)temp, "\r\n+CGMM:%s\r\n", (char*)temp2);
	strncpy((char*)rxBuff, temp2, size);
	return 1;
}

uint8_t service_at_IsNwReg(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	return 1;
}

uint8_t service_at_CheckSignalStrength(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	return 1;
}

uint8_t service_at_ManNwSearch(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	return 1;
}

uint8_t service_at_DeRegNw(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	return 1;
}

uint8_t service_at_ReRegNw(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	return 1;
}

uint8_t service_at_CheckSimChannel(port_uart_handle_t *handle, uint8_t channel, uint16_t timeoutMs)
{
	return 1;
}

void service_at_CpyDigitsBuff(uint8_t *pSourceBuff, uint8_t *pTargetBuff, uint16_t size)
{
	uint16_t cnt = 0;
	while(*pSourceBuff){
		if((*pSourceBuff >= 0x30) && (*pSourceBuff <= 0x39)){
			if(cnt < size){
				target[cnt++] = *pSourceBuff;
			}
		}
		pSourceBuff++;
	}
}

void SafeSscanfStr(char *source, char *target, int size, char *format)
{
	char temp[strlen(source)] = {0};
	int ret = sscanf(source, format, temp);
	if(ret == 1){
			strncpy(target, temp, size);
	}
}

