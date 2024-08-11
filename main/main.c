/* Includes */

#include <stdio.h>
#include <string.h>
#include "time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "atc_gsm.h"
#include "debug_logs.h"
#include "app.h"

/* Macros */

/* Variable declaration */
TaskHandle_t dispatcherTaskHandler = NULL;
static QueueHandle_t eventhandler = NULL;
static bool gFlagGsmCallback = false;

/* structure and enums */
typedef struct app_stateInstance app_stateInstance_t;

typedef enum app_tagStateMachineStatus
{
    APP_STATE_MACHINE_STATUS_HANDLED,
    APP_STATE_MACHINE_STATUS_IGNORED,
    APP_STATE_MACHINE_STATUS_TRANS,
} app_stateMachineStatus_t;

typedef enum app_tagConsumedEvent
{
    APP_CONSUMED_EVENT_IDLE,
    APP_CONSUMED_EVENT_GSM_DATA_RECVD,
    APP_CONSUMED_EVENT_MAX,
} app_consumedEvent_t;

typedef enum app_tagReservedEvent
{
    APP_RESERVED_EVENT_ENTRY = APP_CONSUMED_EVENT_MAX,
    APP_RESERVED_EVENT_EXIT,
} app_reservedEvent_t;

/* Function declaration */
static void AppDispatcher(void *arg);
static void AppPostEvent(app_eventParam_t *pEventParam);

typedef app_stateMachineStatus_t (*app_stateHandle_t)(app_stateInstance_t *pInstance, app_eventParam_t *pParam);

app_stateMachineStatus_t AppIdleState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);
app_stateMachineStatus_t AppGsmState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);
app_stateMachineStatus_t AppGpsState(app_stateInstance_t *pInstance, app_eventParam_t *pParam);

struct app_stateInstance
{
    app_stateHandle_t activeState;
    app_stateHandle_t nextState;
};

/* Global variables */
app_stateInstance_t gstateInstance;

/*************************************************************************************************************************
 ****************************************** Function definition***********************************************************
 * !*********************************************************************************************************************/

void GsmRecvdDataCallback(uint8_t *data, size_t dataLen)
{
    if (gFlagGsmCallback == true)
    {
        LOG_W("[%s] Prev. callback data not cleared !! Overwriting it\r\n", (char *)__func__);
    }
    LOG_I("[%s] Data (%d) : %s\r\n", (char *)__func__, dataLen, (char *)data);
    gFlagGsmCallback = true;

    app_eventParam_t eventParam;
    memset(&eventParam, 0, sizeof(app_eventParam_t));
    strncpy(eventParam.gsmData.dataBuff, (char *)data, ATC_BUFF_SIZE);
    eventParam.gsmData.buffLen = dataLen;
    eventParam.event = APP_CONSUMED_EVENT_GSM_DATA_RECVD;
    AppPostEvent(&eventParam);
}

app_stateMachineStatus_t AppIdleState(app_stateInstance_t *pInstance, app_eventParam_t *pParam)
{
    app_stateMachineStatus_t ret = APP_STATE_MACHINE_STATUS_HANDLED;
    switch (pParam->event)
    {
    case APP_RESERVED_EVENT_ENTRY:
    {
        LOG_I("[%s] %s\r\n", (char *)__func__, "Entry");
        app_eventParam_t eventParam = {.event = APP_CONSUMED_EVENT_IDLE};
        AppPostEvent(&eventParam);
        ret = APP_STATE_MACHINE_STATUS_HANDLED;
    }
    break;
    case APP_CONSUMED_EVENT_IDLE:
    {
        pInstance->nextState = AppGsmState;
        app_eventParam_t eventParam = {.event = APP_CONSUMED_EVENT_GSM_DATA_RECVD};
        AppPostEvent(&eventParam);
        ret = APP_STATE_MACHINE_STATUS_TRANS;
    }
    break;
    case APP_RESERVED_EVENT_EXIT:
        LOG_I("[%s] %s\r\n", (char *)__func__, "Exit");
        ret = APP_STATE_MACHINE_STATUS_HANDLED;
        break;
    default:
        LOG_W("[%s] Unknown event (%d)\r\n", (char *)__func__, pParam->event);
        ret = APP_STATE_MACHINE_STATUS_HANDLED;
        break;
    }
    return ret;
}
app_stateMachineStatus_t AppGsmState(app_stateInstance_t *pInstance, app_eventParam_t *pParam)
{
    app_stateMachineStatus_t ret = APP_STATE_MACHINE_STATUS_HANDLED;
    switch (pParam->event)
    {
    case APP_RESERVED_EVENT_ENTRY:
        LOG_I("[%s] %s\r\n", (char *)__func__, "Entry");
        ret = APP_STATE_MACHINE_STATUS_HANDLED;
        break;
    case APP_CONSUMED_EVENT_GSM_DATA_RECVD:
        LOG_I("[%s] %s\r\n", (char *)__func__, "Gsm Data recvd");
        if (gFlagGsmCallback)
        {
            LOG_I("Data (%d) : %s\r\n", pParam->gsmData.buffLen, pParam->gsmData.dataBuff);
            gFlagGsmCallback = false; // reset the data received flag
        }
        ret = APP_STATE_MACHINE_STATUS_HANDLED;
        break;
    case APP_RESERVED_EVENT_EXIT:
        LOG_I("[%s] %s\r\n", (char *)__func__, "Exit");
        ret = APP_STATE_MACHINE_STATUS_HANDLED;
        break;
    default:
        LOG_W("[%s] Unknown event (%d)\r\n", (char *)__func__, pParam->event);
        ret = APP_STATE_MACHINE_STATUS_HANDLED;
        break;
    }
    return ret;
}

static void AppDispatcher(void *arg)
{
    (void)(arg);
    LOG_I("[%s] Success to create the task\r\n", (char *)__func__);
    app_eventParam_t eventParam;
    app_stateMachineStatus_t rc = APP_STATE_MACHINE_STATUS_HANDLED;
    while (1)
    {
        memset(&eventParam, 0, sizeof(app_eventParam_t));
        if (pdTRUE == xQueueReceive(eventhandler, &eventParam, portMAX_DELAY))
        {
            rc = gstateInstance.activeState(&gstateInstance, &eventParam);
            if (rc == APP_STATE_MACHINE_STATUS_TRANS)
            {
                // Exit the current state
                eventParam.event = APP_RESERVED_EVENT_EXIT;
                gstateInstance.activeState(&gstateInstance, &eventParam);
                // Now switch to next state
                if (gstateInstance.nextState != NULL)
                {
                    gstateInstance.activeState = gstateInstance.nextState;
                    eventParam.event = APP_RESERVED_EVENT_ENTRY;
                    gstateInstance.activeState(&gstateInstance, &eventParam);
                    gstateInstance.nextState = NULL;
                }
                else
                {
                    LOG_W("[%s]Switch to next state requested but next state is NULL\r\n", (char *)__func__)
                }
            }
        }
    }
}

static void AppPostEvent(app_eventParam_t *eventParam)
{
    if (pdTRUE != xQueueSend(eventhandler, eventParam, 20 / portTICK_PERIOD_MS))
    {
        LOG_W("[%s]Failed to send item\r\n", (char *)__func__);
    }
}

void app_main(void)
{
    printf("BUILD DATE : %s\r\nBUILD TIME : %s\r\n", __DATE__, __TIME__);

    eventhandler = xQueueCreate(2, sizeof(app_eventParam_t));
    if (eventhandler == NULL)
    {
        LOG_E("[%s]Failed to create event Queue\r\n", (char *)__func__);
        return;
    }
    LOG_I("Success to create event Queue\r\n");

    xTaskCreate(AppDispatcher, "dispatcher task", 1024 * 3, NULL, 10, &dispatcherTaskHandler);
    if (dispatcherTaskHandler == NULL)
    {
        LOG_E("[%s]Failed to create AppDispatcher task\r\n", (char *)__func__);
        return;
    }

    if (ATC_GSM_FN_STATUS_OK != atc_gsm_StartReceiverTask(CONFIG_UART_NUM2))
    {
        LOG_E("[%s]Failed to Start the GSM receiver task\r\n", (char *)__func__);
        return;
    }

    /* Start the state machine entry */
    app_eventParam_t eventParam = {0};
    eventParam.event = APP_RESERVED_EVENT_ENTRY;
    gstateInstance.activeState = AppIdleState;
    AppPostEvent(&eventParam);

    while (1)
    {
        vTaskDelay(1);
    }
    /* System won't reach here */

    // // For test
    // while (1)
    // {
    //     if (gFlagGsmCallback == true)
    //     {
    //         gFlagGsmCallback = false;
    //         memset(gsmAtc.rxBuff, 0, sizeof(gsmAtc.rxBuff));
    //         gsmAtc.rxBuffLen = 0;
    //     }
    //     // status = atc_Command(&gsmAtc, cmd, strlen((char *)cmd), 1000, 1);

    //     vTaskDelay(10 / portTICK_PERIOD_MS);
    // }
}
