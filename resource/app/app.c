#include <stdio.h>
#include <string.h>
#include "app.h"
#include "service_at.h"

//Macros
#define APP_FW_VER "1.0"
#define APP_FW_NAME "VTS GEN1"

//Function prototypes

//User-defined data types

//Private Variables
port_uart_handle_t gsmUartHndl;	//cavli uart handle

/*!********************************************************************************************************
 * ********************************************************************************************************/

void intf_at_UnsolRespCallback(intf_ble_unsolRespParam_t *pParam)
{
	printf("[%s]\r\n", __func__);
	printf("resp code = %d\r\n", pParam->respCode);
	printf("data = %s\r\n", (char*)pParam->data);
}

void app_main(void)
{
	printf("%s\r\nFW Ver : %s\r\n", APP_FW_NAME, APP_FW_VER);

	//Initialize the UART handle for CAVLI C16Qs
	gsmUartHndl.Instance = USART1;
	gsmUartHndl.Init.BaudRate = 115200;
	if(INTF_AT_FN_STATUS_OK != intf_at_Init(&gsmUartHndl)){
		printf("CAVLI UART comm. init failed\r\n");
		return;
	}
	printf("CAVLI comm. initialized on UART1\r\n");

	uint8_t ret = 0;
	uint8_t rxbuff[100];

	while(!service_at_AT(&gsmUartHndl, INTF_AT_DEFAULT_TIMEOUT_MS)){
		HAL_Delay(1000);
	}
	printf("AT test pass\r\n");

	while(1){
		memset(rxbuff, 0, sizeof(rxbuff));
		ret = service_at_GetIMSI(&gsmUartHndl, rxbuff, sizeof(rxbuff)-1, INTF_AT_DEFAULT_TIMEOUT_MS);
		printf("AT IMSI status %u\r\n", ret);
		if(ret){
				printf("IMSI = %s\r\n", rxbuff);
		}else{
			printf("IMSI failed\r\n");
		}
		HAL_Delay(2000);
		memset(rxbuff, 0, sizeof(rxbuff));
		ret = service_at_GetIMEI(&gsmUartHndl, rxbuff, sizeof(rxbuff)-1, INTF_AT_DEFAULT_TIMEOUT_MS);
		printf("AT IMEI status %u\r\n", ret);
		if(ret){
				printf("IMEI = %s\r\n", rxbuff);
		}else{
			printf("IMEI failed\r\n");
		}
		HAL_Delay(2000);
		memset(rxbuff, 0, sizeof(rxbuff));
		ret = service_at_GetICCID(&gsmUartHndl, rxbuff, sizeof(rxbuff)-1, INTF_AT_DEFAULT_TIMEOUT_MS);
		printf("AT ICCID status %u\r\n", ret);
		if(ret){
				printf("ICCID = %s\r\n", rxbuff);
		}else{
			printf("ICCID failed\r\n");
		}
		HAL_Delay(2000);
	}
}
