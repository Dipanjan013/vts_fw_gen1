#include <stdio.h>
#include <string.h>
#include "app.h"
#include "service_at.h"

//Macros
#define APP_FW_VER "1.0"
#define APP_FW_NAME "VTS GEN1"

//Function prototypes

//User-defined data types
static enum{
	APP_STATE_IDLE,
	APP_STATE_START,
	APP_STATE_READ_INFO,
	APP_STATE_READ_NW_REG_STAT,
}gAppState = APP_STATE_START;


//Private Variables


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

	if(!service_at_Init()){
		printf("CAVLI UART comm. init failed\r\n");
		return;
	}
	printf("CAVLI comm. initialized on UART1\r\n");

	while(1){
		switch(gAppState){
			case APP_STATE_START:
				uint8_t recvData[512] = {0};
				if(service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_TEST, 1000U)){
					if(service_at_Read(SERVICE_AT_UART_INST0, AT_READ_MFG_INFO, recvData, 500, 2000)){
						printf("MFG INFO ALL = %s\r\n", (char*)recvData);
						gAppState = APP_STATE_READ_NW_REG_STAT;
					}
				}
				break;

			case APP_STATE_READ_NW_REG_STAT:
				uint8_t buff[5] = {0};
				if(service_at_Read(SERVICE_AT_UART_INST0, AT_READ_NW_REG_STAT, buff, 2, 2000)){
					printf("CREG = %s\r\n", (char*)buff);
					gAppState = APP_STATE_IDLE;
				}
				break;

			case APP_STATE_IDLE:
				break;

			default:
				break;
		}
		HAL_Delay(1);
	}
}
