#include "service_at.h"
#include <string.h>
#include <stdio.h>
#include "utils.h"

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
	char *pImei = strstr((char*)temp, "IMEI");
	utils_CopyDigitsBuff(temp, rxBuff, size);
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
