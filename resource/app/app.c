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
#include "services.h"

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
//__attribute__((unused)) static app_stateMachineStatus_t AppSleepState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);
//__attribute__((unused)) static app_stateMachineStatus_t AppGpsState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);
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
  .stack_size = 1024*6,
  .priority = (osPriority_t)osPriorityNormal,
};

/************************************************************************************************************************************************
*************************************************************************************************************************************************
*************************************************************************************************************************************************/

static app_stateMachineStatus_t AppPreOPState(app_stateInstance_t *pInstance, app_eventParam_t *pParam)
{
	app_stateMachineStatus_t status = APP_STATE_MACHINE_STATUS_HANDLED;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:{
			LOG_I("[%s] %s\r\n", __func__, "Entry");
			pParam->event = APP_CONSUMED_EVENT_INIT;
			app_PostEvent(pParam, 0);
			status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;

		case APP_CONSUMED_EVENT_INIT:{
			LOG_I("[%s] %s\r\n", __func__, "Init");
			atc_Init();
			pInstance->nextState = AppGsmState;
			status = APP_STATE_MACHINE_STATUS_TRANS;
			//status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;

		case APP_RESERVED_EVENT_EXIT:{
			LOG_I("[%s] %s\r\n", __func__, "Exit");
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
	app_eventParam_t appEvent = {0};
	int16_t ret = 0;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:{
			LOG_I("[%s] %s\r\n", __func__, "Entry");
			appEvent.event = APP_CONSUMED_EVENT_TEST;
			app_PostEvent(&appEvent, 0);
			status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;

		case APP_CONSUMED_EVENT_TEST:{
			LOG_I("[%s] %s\r\n", __func__, "TEST");
			uint8_t temp[512] = {0};

			ret = atc_CmdLookUpTable(ATC_TEST, NULL, 0);
			LOG_I("ATC_TEST status = %d\r\n", ret);

			memset(temp, 0, sizeof(temp));
			ret = atc_CmdLookUpTable(ATC_IMSI, temp, ATC_IMSI_MAX_LEN);
			LOG_I("ATC_IMSI status = %d\r\n", ret);
			LOG_I("IMSI : %s\r\n", (char*)temp);

			memset(temp, 0, 512);
			ret = atc_CmdLookUpTable(ATC_ICCID, temp, ATC_ICCID_MAX_LEN);
			LOG_I("ATC_ICCID status = %d\r\n", ret);
			LOG_I("ICCID : %s\r\n", (char*)temp);

			memset(temp, 0, 512);
			ret = atc_CmdLookUpTable(ATC_MODEM_INFO, temp, ATC_MODEM_INFO_MAX_LEN);
			LOG_I("ATC_MODEM_INFO status = %d\r\n", ret);
			LOG_I("Modem info : %s\r\n", (char*)temp);

			ret = atc_CmdLookUpTable(ATC_NW_REG_STATUS, NULL, 0);
			LOG_I("ATC_NW_REG_STATUS = %d\r\n", ret);

			ret = atc_CmdLookUpTable(ATC_CAVLI_HUBBLE_REG_STATUS, NULL, 0);
			LOG_I("ATC_CAVLI_HUBBLE_REG_STATUS = %d\r\n", ret);

			memset(temp, 0, 512);
			ret = atc_CmdLookUpTable(ATC_NW_RSSI_CHECK, temp, ATC_RSSI_MAX_LEN);
			LOG_I("ATC_NW_RSSI_CHECK status = %d\r\n", ret);
			LOG_I("RSSI : %s\r\n", (char*)temp);

			memset(temp, 0, 512);
			ret = atc_CmdLookUpTable(ATC_NW_OP_NAME, temp, ATC_NW_OP_CODE_MAX_LEN);
			LOG_I("ATC_NW_OP_NAME status = %d\r\n", ret);
			LOG_I("O/P name : %s\r\n", (char*)temp);

			osDelay(3000);
			appEvent.event = APP_CONSUMED_EVENT_TEST;
			app_PostEvent(&appEvent, 0);
			status = APP_STATE_MACHINE_STATUS_HANDLED;
		}break;

		case APP_RESERVED_EVENT_EXIT:{
			LOG_I("[%s] %s\r\n", __func__, "Exit");
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
		LOG_W("[%s]Failed to send Message\r\n", __func__);
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
					LOG_W("State switch requested but next state is NULL\r\n");
				}
			}
		}else{
			LOG_W("[%s %d]Failed to receive message\r\n", __func__, __LINE__);
			osDelay(1000);
		}
	}
}


void app_main(void)
{
	LOG_I("\r\nVTS GEN 1\r\n");
	LOG_I("Build Date %s\r\n", __DATE__);
	LOG_I("Build Time %s\r\n", __TIME__);

  /* Init scheduler */
	osKernelInitialize();

	gQHndl = osMessageQueueNew(2, sizeof(app_eventParam_t), NULL);
	if(gQHndl == NULL){
		LOG_E("Failed to create msg queue\r\n");
		return;
	}

	gStateInstance.currentState = AppPreOPState;
	gStateInstance.nextState = NULL;
	app_eventParam_t param = {.event = APP_RESERVED_EVENT_ENTRY};
	app_PostEvent(&param, 0);

	if(NULL == osThreadNew(AppDispatcherTask, NULL, &appDispatcherAttr)){
		LOG_E("Failed to create App Dispatcher Task\r\n");
		return;
	}

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1);
}
