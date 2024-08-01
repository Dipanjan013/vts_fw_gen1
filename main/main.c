/* Includes */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "atc_gsm.h"
#include "time.h"
#include "debug_logs.h"
#include <string.h>

/* Macros */

/* structure and enums */

typedef struct
{
    uint16_t event;
    union
    {
        struct
        {
            atc_t gsm;
        } gsmData;
        struct
        {
            atc_t gps;
        } gpsData;
    };
} app_eventParam_t;

/* variable definition */
static bool gFlagGsmCallback = false;

/* Variable definition */
atc_t gsmAtc;
atc_t gpsAtc;
QueueHandle_t eventhandler = NULL;

/* Function declaration */
void AppDispatcher(void);
uint8_t AppPostEvent(app_eventParam_t *pEventParam);

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
    memset(gsmAtc.rxBuff, 0, sizeof(gsmAtc.rxBuff));
    strncpy((char *)gsmAtc.rxBuff, (char *)data, ATC_BUFF_SIZE);
    gsmAtc.rxBuffLen = dataLen;
}

void app_main(void)
{
    printf("\033[0m"
           "BUILD DATE : %s\r\n",
           __DATE__);
    printf("BUILD TIME : %s\r\n", __TIME__);

    gsmAtc.uartNum = CONFIG_UART_NUM2;
    // status = atc_Init(&gsmAtc);
    // LOG_I("ATC init status = %d\r\n", status);

    atc_gsm_StartReceiverTask(&gsmAtc);

    eventhandler = xQueueCreate(2, sizeof(app_eventParam_t));
    if (eventhandler == NULL)
    {
        LOG_E("[%s]Failed to create event Queue\r\n", (char *)__func__);
        return;
    }
    LOG_I("Success to create event Queue\r\n");

    // static uint8_t cmd[] = "AT\r\n";
    while (1)
    {
        if (gFlagGsmCallback == true)
        {
            gFlagGsmCallback = false;
            memset(gsmAtc.rxBuff, 0, sizeof(gsmAtc.rxBuff));
            gsmAtc.rxBuffLen = 0;
        }
        // status = atc_Command(&gsmAtc, cmd, strlen((char *)cmd), 1000, 1);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
