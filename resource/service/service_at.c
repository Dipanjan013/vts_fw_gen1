#include "service_at.h"


uint8_t service_at_AT(port_uart_handle_t *handle, uint16_t timeoutMs)
{
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, SERVICE_AT_STR, strlen(SERVICE_AT_STR), NULL, 0, 0, NULL, INTF_AT_DEFAULT_TIMEOUT_MS);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	return 1;
}

uint8_t service_at_GetIMSI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	uint8_t temp[50] = {0};
	intf_at_fnStatus_t ret;
	ret = intf_at_Command(handle, SERVICE_AT_CIMI_STR, strlen(SERVICE_AT_CIMI_STR), temp, sizeof(temp)-1, 1, NULL, INTF_AT_DEFAULT_TIMEOUT_MS);
	if(INTF_AT_FN_STATUS_OK != ret){
		return 0;
	}
	snprintf();
	return 1;
}

uint8_t service_at_GetIMEI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	return 1;
}

uint8_t service_at_GetICCID(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
	return 1;
}

uint8_t service_at_GetModemInfo(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs)
{
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
