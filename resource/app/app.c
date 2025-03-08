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
	APP_STATE_WAIT_FOR_MSG,
}gAppState = APP_STATE_START;


//Private Variables


/***********************************************************************************************************
 * *********************************************************************************************************
 * ********************************************************************************************************/

void service_at_UnsolRespCallback(service_at_unsolResp_t type, uint8_t *buff, uint16_t len)
{
	printf("[%s]\r\n", __func__);
	printf("Response code : %d\r\n", type);
	if(len > 0){
		printf("Response Data[%d] : %s\r\n",len, (char*)buff);
	}else{
		printf("Empty data\r\n");
	}
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
					gAppState = APP_STATE_WAIT_FOR_MSG;
					intf_at_SetUnsolRespCheckerFlag(1);
				}
				break;

			case APP_STATE_WAIT_FOR_MSG:
				service_at_UnsolRespCheckerTask(SERVICE_AT_UART_INST0);
				break;

			case APP_STATE_IDLE:
				break;

			default:
				break;
		}
		HAL_Delay(1);
	}
}
