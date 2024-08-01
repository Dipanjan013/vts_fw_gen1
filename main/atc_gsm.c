#include "atc_gsm.h"
#include "debug_logs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

TaskHandle_t gsmTaskHndl;
port_uart_num_t gUartNum = -1;

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
    uint8_t rxBuff[ATC_BUFF_SIZE + 1];
    memset(rxBuff, 0, sizeof(rxBuff));
    while (1)
    {
        rc = port_uart_Rx(gUartNum, rxBuff, ATC_BUFF_SIZE, 20 / portTICK_PERIOD_MS);
        if (PORT_UART_FN_STATUS_OK == rc)
        {
            if (rxBuff[0] != '\0')
            {
                GsmRecvdDataCallback(rxBuff, strlen((char *)rxBuff));
                memset(rxBuff, 0, sizeof(rxBuff));
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

atc_gsm_fnStatus_t atc_gsm_StartReceiverTask(atc_t *atc)
{
    atc_fnStatus_t rc = atc_Init(atc);
    if (ATC_FN_STATUS_OK != rc)
    {
        LOG_E("[%s] ATC init failed (%d)\r\n", (char *)__func__, rc);
        return ATC_GSM_FN_STATUS_ERR;
    }
    LOG_I("[%s]GSM ATC initialized\r\n", (char *)__func__);
    gUartNum = atc->uartNum;
    xTaskCreate(GsmTask, "gsm task", 1024 * 3, NULL, 10, &gsmTaskHndl);
    return ATC_GSM_FN_STATUS_OK;
}