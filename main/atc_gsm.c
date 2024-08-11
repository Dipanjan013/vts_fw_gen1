#include "atc_gsm.h"
#include "debug_logs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

TaskHandle_t gsmTaskHndl;
static atc_t gGsmAtc;

WEAK void GsmRecvdDataCallback(uint8_t *data, size_t dataLen)
{
    (void)data;
    (void)(dataLen);
}

static void GsmTask(void *arg)
{
    (void)arg;
    LOG_V("[%s] Task Created\r\n", (char *)__func__);
    port_uart_FnStatus_t rc;
    memset(gGsmAtc.rxBuff, 0, sizeof(gGsmAtc.rxBuff));
    while (1)
    {
        rc = port_uart_Rx(gGsmAtc.uartNum, gGsmAtc.rxBuff, ATC_BUFF_SIZE, 20 / portTICK_PERIOD_MS);
        if (PORT_UART_FN_STATUS_OK == rc)
        {
            if (gGsmAtc.rxBuff[0] != '\0')
            {
                GsmRecvdDataCallback(gGsmAtc.rxBuff, strlen((char *)gGsmAtc.rxBuff));
                memset(gGsmAtc.rxBuff, 0, sizeof(gGsmAtc.rxBuff));
            }
        }
        else
        {
            LOG_E("[%s]Uart receive failed\r\n", (char *)__func__);
            break;
        }
        vTaskDelay(1);
    }
    LOG_W("[%s]Deleting the task due to exception\r\n", (char *)__func__);
    vTaskDelete(gsmTaskHndl);
}

atc_gsm_fnStatus_t atc_gsm_StartReceiverTask(port_uart_num_t uartNum)
{
    memset(&gGsmAtc, 0, sizeof(atc_t));
    gGsmAtc.uartNum = uartNum;
    atc_fnStatus_t rc = atc_Init(&gGsmAtc);
    if (ATC_FN_STATUS_OK != rc)
    {
        LOG_E("[%s] ATC init failed (%d)\r\n", (char *)__func__, rc);
        return ATC_GSM_FN_STATUS_ERR;
    }
    LOG_I("[%s]GSM ATC initialized\r\n", (char *)__func__);
    xTaskCreate(GsmTask, "gsm task", 1024 * 3, NULL, 10, &gsmTaskHndl);
    if (gsmTaskHndl == NULL)
    {
        LOG_E("[%s] Failed to create the task\r\n", (char *)__func__);
        return ATC_GSM_FN_STATUS_ERR;
    }
    return ATC_GSM_FN_STATUS_OK;
}