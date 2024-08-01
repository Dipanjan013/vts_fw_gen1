#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "atc.h"

atc_fnStatus_t atc_Init(atc_t *atc)
{
    if (atc == NULL)
        return ATC_FN_STATUS_INVALID_ARG;
    if (atc->initiated)
        return ATC_FN_STATUS_OK; // already initialized
    port_uart_FnStatus_t rc = port_uart_Init(atc->uartNum);
    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        return ATC_FN_STATUS_ERR;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        return ATC_FN_STATUS_INVALID_ARG;
    case PORT_UART_FN_STATUS_OK:
        atc->initiated = true;
        return ATC_FN_STATUS_OK;
    default:
        return ATC_FN_STATUS_ERR;
    }
}

atc_fnStatus_t atc_Transmit(atc_t *atc, uint8_t *buff, uint16_t buffSize)
{
    atc_fnStatus_t atcStatus;
    if (!atc->initiated)
        return ATC_FN_STATUS_ERR;
    if (atc->lock)
        return ATC_FN_STATUS_BUSY;
    if (buff == NULL || buffSize == 0)
        return ATC_FN_STATUS_INVALID_ARG;
    atc->lock = true; // enable lock
    port_uart_FnStatus_t rc = port_uart_Tx(atc->uartNum, buff, buffSize);
    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        atcStatus = ATC_FN_STATUS_ERR;
        break;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        atcStatus = ATC_FN_STATUS_INVALID_ARG;
        break;
    case PORT_UART_FN_STATUS_OK:
        atcStatus = ATC_FN_STATUS_OK;
        break;
    default:
        atcStatus = ATC_FN_STATUS_ERR;
    }
    atc->lock = false; // disable lock
    return atcStatus;
}

atc_fnStatus_t atc_Command(atc_t *atc, uint8_t *cmd, uint16_t cmdLen, uint16_t timeoutMs)
{
    atc_fnStatus_t atcStatus;
    if (!atc->initiated)
        return ATC_FN_STATUS_ERR;
    if (atc->lock)
        return ATC_FN_STATUS_BUSY;
    if (cmd == NULL || cmdLen == 0)
        return ATC_FN_STATUS_INVALID_ARG;
    atc->lock = true;
    port_uart_FnStatus_t rc = port_uart_Tx(atc->uartNum, cmd, cmdLen);
    if (PORT_UART_FN_STATUS_OK == rc)
    {
        memset(atc->rxBuff, 0, sizeof(atc->rxBuff));
        rc = port_uart_Rx(atc->uartNum, atc->rxBuff, ATC_BUFF_SIZE, timeoutMs);
    }

    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        atcStatus = ATC_FN_STATUS_ERR;
        break;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        atcStatus = ATC_FN_STATUS_INVALID_ARG;
        break;
    case PORT_UART_FN_STATUS_OK:
        atc->rxBuffLen = strlen((char *)atc->rxBuff);
        atcStatus = ATC_FN_STATUS_OK;
        break;
    default:
        atcStatus = ATC_FN_STATUS_ERR;
    }
    atc->lock = false;
    return atcStatus;
}

atc_fnStatus_t atc_Deinit(atc_t *atc)
{
    atc_fnStatus_t atcStatus;
    if (!atc->initiated)
        return ATC_FN_STATUS_OK;
    if (atc->lock)
        return ATC_FN_STATUS_BUSY;
    port_uart_FnStatus_t rc = port_uart_Deinit(atc->uartNum);
    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        atcStatus = ATC_FN_STATUS_ERR;
        break;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        atcStatus = ATC_FN_STATUS_INVALID_ARG;
        break;
    case PORT_UART_FN_STATUS_OK:
        atc->rxBuffLen = strlen((char *)atc->rxBuff);
        atcStatus = ATC_FN_STATUS_OK;
        break;
    default:
        atcStatus = ATC_FN_STATUS_ERR;
    }
    return atcStatus;
}