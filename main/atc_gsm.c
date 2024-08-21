#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "atc_gsm.h"
#include "debug_logs.h"
#include "config.h"

static TaskHandle_t gsmTaskHndl;
static uint8_t gsmBuff[PORT_UART_BUFF_SIZE + 1] = {0};

WEAK void GsmRecvdDataCallback(uint8_t *data, size_t dataLen)
{
    (void)data;
    (void)(dataLen);
}

static void GsmReceiverTask(void *arg)
{
    port_uart_FnStatus_t rc;
    memset(gsmBuff, 0, sizeof(gsmBuff));
    while (1)
    {
        static uint8_t gsmBuff[PORT_UART_BUFF_SIZE + 1] = {0};
        rc = port_uart_Rx(*(uart_port_t *)arg, gsmBuff, PORT_UART_BUFF_SIZE, 20 / portTICK_PERIOD_MS);
        if (PORT_UART_FN_STATUS_OK == rc)
        {
            if (gsmBuff[0] != '\0')
            {
                GsmRecvdDataCallback(gsmBuff, strlen((char *)gsmBuff));
                memset(gsmBuff, 0, sizeof(gsmBuff));
            }
        }
    }
}

atc_fnStatus_t atc_StartGsmReceiverTask(uart_port_t uartNum)
{
    xTaskCreate(GsmReceiverTask, "gsm task", 1024 * 3, &uartNum, 10, &gsmTaskHndl);
    if (gsmTaskHndl == NULL)
    {
        LOG_E("[%s] Failed to create the task\r\n", (char *)__func__);
        return ATC_FN_STATUS_ERR;
    }
    LOG_I("[%s] : Success\r\n", (char *)__func__);
    return ATC_FN_STATUS_OK;
}