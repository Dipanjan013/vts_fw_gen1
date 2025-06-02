//includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsis_os2.h"

#include "app.h"
#include "service_at.h"

//Macros
#define MSG_POST_TIMEOUT_MS 100U

//User-defined data types
typedef enum{
	APP_STATE_STATUS_HANDLED,
	APP_STATE_STATUS_TRANS,
	APP_STATE_STATUS_IGNORED,
}app_stateStatus_e;

typedef enum{
	APP_EVENT_IDLE,
	APP_EVENT_INIT,
	APP_EVENT_FETCH_GPS,
	APP_EVENT_CONNECT_CELLULAR,
	APP_EVENT_CONNECT_MQTT,
	APP_EVENT_SLEEP,
	APP_EVENT_MAX
}app_event_e;

typedef enum{
	APP_RESERVED_EVENT_ENTRY = APP_EVENT_MAX,
	APP_RESERVED_EVENT_EXIT
}app_reservedEvent_e;

typedef struct app_tagStateInstance app_stateInst_s;
typedef app_stateStatus_e (*StateHandler_t)(app_eventParam_s *pParam, app_stateInst_s *pInst);

struct app_tagStateInstance{
	StateHandler_t activeState;
	StateHandler_t nextState;
};

//Function prototypes
static app_stateStatus_e AppStatePreOp(app_eventParam_s *pParam, app_stateInst_s *pInst);
static app_stateStatus_e AppStatePublish(app_eventParam_s *pParam, app_stateInst_s *pInst);
static app_stateStatus_e AppStateIdle(app_eventParam_s *pParam, app_stateInst_s *pInst);

/*!
 * @fn App dispatcher task that handle events
 */
static void AppDisPatcher(void);

//Private Variables
static app_stateInst_s gAppStateInstance;
static osMessageQueueId_t gQueueHndl;

/*!**************************************************************************************************************************
 *
 ***************************************************************************************************************************/

void service_at_UnsolRespCallback(service_at_unsolResp_t type, uint8_t *buff, uint16_t len)
{
	(void)(buff);
	(void)(len);
	printf("[%s] Type : %d\r\n", __func__, type);
}

void AppPostEvent(app_eventParam_s *pParam)
{
	osStatus_t osStatus = osError;
	osStatus = osMessageQueuePut(gQueueHndl, pParam, 0, MSG_POST_TIMEOUT_MS);
	if(osOK != osStatus){
		printf("[%s] Failed (%d)\r\n", __func__, osStatus);
	}
}

void AppPostEventFromIsr(app_eventParam_s *pParam)
{
	osStatus_t osStatus = osError;
	osStatus = osMessageQueuePut(gQueueHndl, pParam, 0, 0);
	if(osOK != osStatus){
		// do something. Should be interrupt safe
	}
}

static app_stateStatus_e AppStatePreOp(app_eventParam_s *pParam, app_stateInst_s *pInst)
{
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	uint8_t readData[512];
	int rc = 0;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:
			break;
		case APP_EVENT_INIT:{
			printf("[%s] Init\r\n", __func__);
			do{
				rc = service_at_Test(SERVICE_AT_UART_INST0);
				if(!rc){
					printf("Communication failed with Cellular\r\n");
					break;
				}
				memset(readData, 0, sizeof(readData));
				rc = service_at_Read(SERVICE_AT_UART_INST0, AT_READ_MFG_INFO, readData, sizeof(readData)-1, 1000);
				if(!rc){
					printf("Failed to read MFG data\r\n");
				}else{
					printf("MFG Data : %s\r\n", (char*)readData);
				}
			}while(0);
			if(rc == 1){
				printf("Pre-operating state successful. Switching to ---> Normal mode\r\n");
				pInst->nextState = AppStatePublish;
			}else{
				printf("Error : Pre-operating state failed\r\n");
				pInst->nextState = AppStateIdle;
			}
			stateStatus = APP_STATE_STATUS_TRANS;
		}break;
		case APP_RESERVED_EVENT_EXIT:
			break;
		default:
			printf("[%s] Unknown event %d\r\n", __func__, pParam->event);
	}
	return stateStatus;
}

static app_stateStatus_e AppStatePublish(app_eventParam_s *pParam, app_stateInst_s *pInst)
{
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:
			break;
		case APP_RESERVED_EVENT_EXIT:
			break;
		default:
			printf("[%s] Unknown event %d\r\n", __func__, pParam->event);
	}
	return stateStatus;
}

static app_stateStatus_e AppStateIdle(app_eventParam_s *pParam, app_stateInst_s *pInst)
{
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:
			break;
		case APP_RESERVED_EVENT_EXIT:
			break;
		default:
			printf("[%s] Unknown event %d\r\n", __func__, pParam->event);
	}
	return stateStatus;
}

static void AppDisPatcher(void)
{
	app_eventParam_s appEvent;
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	while(1){
		memset(&appEvent, 0, sizeof(app_eventParam_s));
		if(osOK == osMessageQueueGet(gQueueHndl, &appEvent, 0, osWaitForever)){
			stateStatus = gAppStateInstance.activeState(&appEvent, &gAppStateInstance);
			if(APP_STATE_STATUS_TRANS == stateStatus){													//state change requested
				if(gAppStateInstance.nextState != NULL){													//just to make sure target state is not empty
					appEvent.event = APP_RESERVED_EVENT_EXIT;
					gAppStateInstance.activeState(&appEvent, &gAppStateInstance);		//clear exit the current state
					gAppStateInstance.activeState = gAppStateInstance.nextState;		//state changed to target state
					appEvent.event = APP_RESERVED_EVENT_ENTRY;
					gAppStateInstance.activeState(&appEvent, &gAppStateInstance);		//clear entry the target state
					gAppStateInstance.nextState = NULL;
				}else{
					printf("[%s] State change requested but next state is empty\r\n", __func__);
				}
			}
		}else{
			printf("[%s]Failed to get new message\r\n", __func__);
		}
		osDelay(1);
	}
}

void app_main(void)
{
	printf("\r\n>>BOOT UP\r\n");
	printf("%s\r\nFW Ver : %s\r\nHW Ver : %s\r\n", CONFIG_FW_NAME, CONFIG_FW_VER, CONFIG_HW_VER);

	do{
		gQueueHndl = osMessageQueueNew(5, sizeof(app_eventParam_s), NULL);
		if(!gQueueHndl){
			printf("[%s]Failed to create message queue\r\n", __func__);
			break;
		}

		if(!service_at_Init()){
			printf("AT Initialization failed\r\n");
			break;
		}
		printf("AT Initialized successfully\r\n");
		int rc;
		while(1){
			rc = service_at_Test(SERVICE_AT_UART_INST0);
			if(rc)break;
			osDelay(1000);
		}

		//Initialize the state machine
		gAppStateInstance.activeState = AppStatePreOp;
		gAppStateInstance.nextState = NULL;
		app_eventParam_s *pParam = calloc(sizeof(app_eventParam_s), 1);
		if(!pParam){
			break;
		}
		pParam->event = APP_EVENT_INIT;
		AppPostEvent(pParam);
		free(pParam);
		AppDisPatcher();
	}while(0);

	//Reaching here indicates error at boot-up
}

//#include <stdio.h>
//#include <string.h>
//#include "app.h"
//#include "service_at.h"
//#include "utils_gps.h"
//
////Macros
//
//
////User-defined data types
//
//typedef struct{
//
//}app_eventParam_s;
//
//static enum {
//	APP_STATE_IDLE,
//	APP_STATE_START,
//	APP_STATE_READ_INFO,
//	APP_STATE_READ_NW_REG_STAT,
//	APP_STATE_READ_GPS_LOC,
//	APP_STATE_WAIT_FOR_MSG,
//} gAppState = APP_STATE_START;
//
////Function prototypes
//typedef void (*StateHandler_t)(app_eventParam_s *pParam, app_stateInst_s *pInstance);
//
//typedef struct{
//	StateHandler_t CurrentState;
//	StateHandler_t NextState;
//}app_stateInst_s;
//
////Private Variables
//static appParam_s gAppParam = { .state = APP_STATE_IDLE };
///***********************************************************************************************************
// * *********************************************************************************************************
// * ********************************************************************************************************/
//
//void service_at_UnsolRespCallback(service_at_unsolResp_t type, uint8_t *buff,
//		uint16_t len) {
////	printf("[%s]\r\n", __func__);
//	switch (type) {
//		case AT_UNSOL_RESP_SMS:
//			printf("Data : %.*s\r\n", len, (char*) buff);
//			break;
//		case AT_UNSOL_RESP_CALL:
//			printf("Data : %.*s\r\n", len, (char*) buff);
//			break;
//		case AT_UNSOL_RESP_GPS:
//			memset(&gAppParam, 0, sizeof(appParam_s));
//			gAppParam.state = APP_STATE_READ_GPS_LOC;
//			memcpy(gAppParam.eventData_u.data, buff,
//					(len < INTF_AT_RX_DATA_MAX) ? len : INTF_AT_RX_DATA_MAX);
//			gAppState = APP_STATE_READ_GPS_LOC;
//			break;
//		default:
//	}
//}
//
//void app_main(void) {
//	printf(">>BOOT UP\r\n");
//	printf("%s\r\nFW Ver : %s\r\nHW Ver : %s\r\n", CONFIG_FW_NAME, CONFIG_FW_VER, CONFIG_HW_VER);
//
//	if (!service_at_Init()) {
//		printf("CAVLI UART comm. init failed\r\n");
//		return;
//	}
//	printf("CAVLI comm. initialized on UART1\r\n");
//
//	while (1) {
//		switch (gAppState) {
//			case APP_STATE_START:
//				uint8_t recvData[512] = { 0 };
//				if (service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_TEST, 1000U)) {
//					if (service_at_Read(SERVICE_AT_UART_INST0, AT_READ_MFG_INFO, recvData,
//							500, 2000)) {
//						printf("MFG INFO ALL = %s\r\n", (char*) recvData);
//						gAppState = APP_STATE_READ_NW_REG_STAT;
//					}
//				}
//				break;
//
//			case APP_STATE_READ_NW_REG_STAT:
//				uint8_t buff[5] = { 0 };
//				if (service_at_Read(SERVICE_AT_UART_INST0, AT_READ_NW_REG_STAT, buff, 2,
//						2000)) {
//					printf("CREG = %s\r\n", (char*) buff);
//					if (service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_GPS_ON, 1000U)) {
//						if (service_at_Execute(SERVICE_AT_UART_INST0, AT_READ_GPS_STREAM,
//								1000U)) {
//							printf("GPS Stream start\r\n");
//							printf("Tick now : %lu\r\n", HAL_GetTick());
//							gAppState = APP_STATE_WAIT_FOR_MSG;
//							intf_at_SetUnsolRespCheckerFlag(1);
//						}
//					}
//				}
//				break;
//
//			case APP_STATE_WAIT_FOR_MSG:
//				service_at_UnsolRespCheckerTask(SERVICE_AT_UART_INST0);
//				HAL_Delay(500);
//				break;
//
//			case APP_STATE_READ_GPS_LOC:
//				nmea_s nmea = { 0 };
//				if (utils_gps_NmeaParse((char*) gAppParam.eventData_u.data, &nmea)) {
//					utils_gps_PrintNmea(&nmea);
//					intf_at_SetUnsolRespCheckerFlag(0);
//					if ((service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_GPS_STREAM_STOP,
//							1000U))) {
//						printf("GPS Stream off\r\n");
//						if ((service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_GPS_OFF,
//								1000U))) {
//							printf("GPS turned off\r\n");
//							printf("Tick now : %lu\r\n", HAL_GetTick());
//							gAppState = APP_STATE_IDLE;
//							break;
//						}
//					}
//				}
//				//Fail condition
//				gAppState = APP_STATE_WAIT_FOR_MSG;
//				break;
//
//			case APP_STATE_IDLE:
//				break;
//
//			default:
//				break;
//		}
//		HAL_Delay(1);
//	}
//}
