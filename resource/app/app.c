#include <stdio.h>
#include <string.h>
#include "app.h"
#include "service_at.h"
#include "utils_gps.h"

//Macros
#define APP_FW_VER "1.0"
#define APP_FW_NAME "VTS GEN1"

//User-defined data types
typedef struct{
	int state;
	union{
		uint8_t data[INTF_AT_RX_DATA_MAX];
		struct{
			uint8_t fixFlag;
			nmea_s nmea;
		}gps_s;
	}eventData_u;
}appParam_s;

static enum{
	APP_STATE_IDLE,
	APP_STATE_START,
	APP_STATE_READ_INFO,
	APP_STATE_READ_NW_REG_STAT,
	APP_STATE_READ_GPS_LOC,
	APP_STATE_WAIT_FOR_MSG,
}gAppState = APP_STATE_START;


//Function prototypes
typedef void (*StateHandler_t)();

//Private Variables
static appParam_s gAppParam = {.state = APP_STATE_IDLE};
/***********************************************************************************************************
 * *********************************************************************************************************
 * ********************************************************************************************************/

void service_at_UnsolRespCallback(service_at_unsolResp_t type, uint8_t *buff, uint16_t len)
{
//	printf("[%s]\r\n", __func__);
	switch(type){
		case AT_UNSOL_RESP_SMS:
			printf("Data : %.*s\r\n", len, (char*)buff);
			break;
		case AT_UNSOL_RESP_CALL:
			printf("Data : %.*s\r\n", len, (char*)buff);
			break;
		case AT_UNSOL_RESP_GPS:
			memset(&gAppParam, 0, sizeof(appParam_s));
			gAppParam.state = APP_STATE_READ_GPS_LOC;
			memcpy(gAppParam.eventData_u.data, buff, (len < INTF_AT_RX_DATA_MAX) ? len : INTF_AT_RX_DATA_MAX);
			gAppState = APP_STATE_READ_GPS_LOC;
			break;
		default:
	}
}

void app_main(void)
{
  printf(">>BOOT UP\r\n");
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
					if(service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_GPS_ON, 1000U)){
						if(service_at_Execute(SERVICE_AT_UART_INST0, AT_READ_GPS_STREAM, 1000U)){
							printf("GPS Stream start\r\n");
							printf("Tick now : %lu\r\n", HAL_GetTick());
							gAppState = APP_STATE_WAIT_FOR_MSG;
							intf_at_SetUnsolRespCheckerFlag(1);
						}
					}
				}
				break;

			case APP_STATE_WAIT_FOR_MSG:
				service_at_UnsolRespCheckerTask(SERVICE_AT_UART_INST0);
				HAL_Delay(500);
				break;

			case APP_STATE_READ_GPS_LOC:
				nmea_s nmea = {0};
				if(utils_gps_NmeaParse((char*)gAppParam.eventData_u.data, &nmea)){
						utils_gps_PrintNmea(&nmea);
						intf_at_SetUnsolRespCheckerFlag(0);
						if((service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_GPS_STREAM_STOP, 1000U))){
							printf("GPS Stream off\r\n");
							if((service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_GPS_OFF, 1000U))){
								printf("GPS turned off\r\n");
								printf("Tick now : %lu\r\n", HAL_GetTick());
								gAppState = APP_STATE_IDLE;
								break;
							}
						}
				}
				//Fail condition
				gAppState = APP_STATE_WAIT_FOR_MSG;
				break;

			case APP_STATE_IDLE:
				break;

			default:
				break;
		}
		HAL_Delay(1);
	}
}
