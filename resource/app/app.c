/*
 * app.c
 *
 *  Created on: Sep 1, 2024
 *      Author: Dipanjan
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cmsis_os2.h>

#include "app.h"
#include "atc.h"

/*! Macros */

/*! user defined data types */
typedef struct app_stateInstance app_stateInstance_t;

typedef enum app_tagStateMachineStatus
{
    APP_STATE_MACHINE_STATUS_HANDLED,
    APP_STATE_MACHINE_STATUS_IGNORED,
    APP_STATE_MACHINE_STATUS_TRANS,
} app_stateMachineStatus_t;

typedef enum app_tagConsumedEvent
{
    APP_CONSUMED_EVENT_INIT,
		APP_CONSUMED_EVENT_TEST,
    APP_CONSUMED_EVENT_MAX,
} app_consumedEvent_t;

typedef enum app_tagReservedEvent
{
    APP_RESERVED_EVENT_ENTRY = APP_CONSUMED_EVENT_MAX,
    APP_RESERVED_EVENT_EXIT,
} app_reservedEvent_t;

/*! Private function*/
typedef app_stateMachineStatus_t (*StateHandle_t)(app_stateInstance_t *pInstance, app_eventParam_t *pParam);

static app_stateMachineStatus_t AppPreOPState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);
static app_stateMachineStatus_t AppSleepState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);
static app_stateMachineStatus_t AppGpsState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);
static app_stateMachineStatus_t AppGsmState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);

static void AppDispatcherTask(void *arg);

/*! Variable definition */
struct app_stateInstance{
	StateHandle_t currentState;
	StateHandle_t nextState;
};

static osMessageQueueId_t gQHndl;
static app_stateInstance_t gStateInstance;

static const osThreadAttr_t appDispatcherAttr = {
  .name = "app_dispatcher",
  .stack_size = 1024*3,
  .priority = (osPriority_t)osPriorityLow,
};

/************************************************************************************************************************************************
*************************************************************************************************************************************************
*************************************************************************************************************************************************/

static app_stateMachineStatus_t AppPreOPState(app_stateInstance_t *pInstance, app_eventParam_t *pParam)
{
	app_stateMachineStatus_t status = APP_STATE_MACHINE_STATUS_HANDLED;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:{
			printf("[%s] %s\r\n", __func__, "Entry");
			pParam->event = APP_CONSUMED_EVENT_INIT;
			app_PostEvent(pParam, 0);
			status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;

		case APP_CONSUMED_EVENT_INIT:{
			printf("[%s] %s\r\n", __func__, "Init");
			atc_Init();
			pInstance->nextState = AppGsmState;
			status = APP_STATE_MACHINE_STATUS_TRANS;
			//status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;

		case APP_RESERVED_EVENT_EXIT:{
			printf("[%s] %s\r\n", __func__, "Exit");
			status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;
		default:
			break;
	}
	return status;
}

static app_stateMachineStatus_t AppGsmState(app_stateInstance_t *pInstance, app_eventParam_t *pParam)
{
	app_stateMachineStatus_t status = APP_STATE_MACHINE_STATUS_HANDLED;
	int16_t ret = 0;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:{
			printf("[%s] %s\r\n", __func__, "Entry");
			pParam->event = APP_CONSUMED_EVENT_TEST;
			app_PostEvent(pParam, 0);
			status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;

		case APP_CONSUMED_EVENT_TEST:{
			ret = atc_CmdLookUpTable(ATC_TEST, NULL, 0);
			printf("test status = %d\r\n", ret);
//			char imsi[ATC_IMSI_MAX_LEN+1] = {0};
//			ret = atc_CmdLookUpTable(ATC_IMSI, imsi, ATC_IMSI_MAX_LEN);
//			printf("test status = %d, data = %s\r\n", ret, imsi);
			osDelay(3000);
			app_PostEvent(pParam, 0);
			status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;

		case APP_RESERVED_EVENT_EXIT:{
			printf("[%s] %s\r\n", __func__, "Exit");
			status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;
		default:
			break;
	}
	return status;
}

void app_PostEvent(app_eventParam_t *pParam, uint8_t flagFromISR)
{
	if(osOK != osMessageQueuePut(gQHndl, pParam, 0, (flagFromISR == 1)? 0U : 100U)){
		printf("[%s]Failed to send Message\r\n", __func__);
	}
}

static void AppDispatcherTask(void *arg)
{
	(void)(arg);
	app_eventParam_t eventParam;
	app_stateMachineStatus_t status = APP_STATE_MACHINE_STATUS_HANDLED ;
	while(1){
		memset(&eventParam, 0, sizeof(app_eventParam_t));
		if(osOK == osMessageQueueGet(gQHndl, &eventParam, NULL, osWaitForever)){
			status = gStateInstance.currentState(&gStateInstance, &eventParam);
			if(APP_STATE_MACHINE_STATUS_TRANS == status){
				eventParam.event = APP_RESERVED_EVENT_EXIT;
				gStateInstance.currentState(&gStateInstance, &eventParam);
				if(gStateInstance.nextState != NULL){
					gStateInstance.currentState = gStateInstance.nextState;
					gStateInstance.nextState = NULL;
					eventParam.event = APP_RESERVED_EVENT_ENTRY;
					gStateInstance.currentState(&gStateInstance, &eventParam);
				}else{
					printf("State switch requested but next state is NULL\r\n");
				}
			}
		}else{
			printf("[%s %d]Failed to receive message\r\n", __func__, __LINE__);
			osDelay(1000);
		}
	}
}


void app_main(void)
{
	printf("\r\nVTS GEN 1\r\n");
	printf("Build Date %s\r\n", __DATE__);
	printf("Build Time %s\r\n", __TIME__);

  /* Init scheduler */
	osKernelInitialize();

	gQHndl = osMessageQueueNew(2, sizeof(app_eventParam_t), NULL);
	if(gQHndl == NULL){
		printf("Failed to create msg queue\r\n");
		return;
	}

	gStateInstance.currentState = AppPreOPState;
	gStateInstance.nextState = NULL;
	app_eventParam_t param = {.event = APP_RESERVED_EVENT_ENTRY};
	app_PostEvent(&param, 0);

	if(NULL == osThreadNew(AppDispatcherTask, NULL, &appDispatcherAttr)){
		printf("Failed to create App Dispatcher Task\r\n");
		return;
	}

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1);
}
