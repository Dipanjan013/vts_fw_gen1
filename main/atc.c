#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "atc.h"
#include "debug_logs.h"
#include "config.h"

static uart_config_t gGpsUartConfig = {
    .baud_rate = CONFIG_UART_NUM0_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

static uart_config_t gGsmUartConfig = {
    .baud_rate = CONFIG_UART_NUM2_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

atc_fnStatus_t atc_Init(atc_mod_type module)
{
    port_uart_FnStatus_t rc = PORT_UART_FN_STATUS_ERR;
    switch (module)
    {
    case ATC_GSM_MOD:
        rc = port_uart_Init(UART_NUM_0, CONFIG_UART_NUM0_TX_PIN, CONFIG_UART_NUM0_RX_PIN, &gGpsUartConfig);
        break;
    case ATC_GPS_MOD:
        rc = port_uart_Init(UART_NUM_2, CONFIG_UART_NUM2_TX_PIN, CONFIG_UART_NUM2_RX_PIN, &gGsmUartConfig);
        break;
    default:
        return ATC_FN_STATUS_INVALID_ARG;
    }
    // return status
    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        return ATC_FN_STATUS_ERR;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        return ATC_FN_STATUS_INVALID_ARG;
    case PORT_UART_FN_STATUS_OK:
        return ATC_FN_STATUS_OK;
    default:
        return ATC_FN_STATUS_ERR;
    }
}

atc_fnStatus_t atc_Command(atc_mod_type module, uint8_t *writeCmd, size_t writeLen, uint8_t *readBuff, size_t readLen, uint32_t waitMs)
{
    port_uart_FnStatus_t rc = port_uart_Tx((uart_port_t)module, writeCmd, writeLen);
    if (PORT_UART_FN_STATUS_OK == rc)
    {
        rc = port_uart_Rx(module, readBuff, readLen, waitMs);
    }
    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        return ATC_FN_STATUS_ERR;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        return ATC_FN_STATUS_INVALID_ARG;
    case PORT_UART_FN_STATUS_OK:
        return ATC_FN_STATUS_OK;
    default:
        return ATC_FN_STATUS_ERR;
    }
}

atc_fnStatus_t atc_SendCmd(atc_mod_type module, uint8_t *writeBuff, size_t writeLen)
{
    port_uart_FnStatus_t rc = port_uart_Tx((uart_port_t)module, writeBuff, writeLen);
    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        return ATC_FN_STATUS_ERR;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        return ATC_FN_STATUS_INVALID_ARG;
    case PORT_UART_FN_STATUS_OK:
        return ATC_FN_STATUS_OK;
    default:
        return ATC_FN_STATUS_ERR;
    }
}

atc_fnStatus_t atc_RecvData(atc_mod_type module, uint8_t *readBuff, size_t readLen, uint32_t timeoutMs)
{
    port_uart_FnStatus_t rc = port_uart_Rx((uart_port_t)module, readBuff, readLen, timeoutMs);
    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        return ATC_FN_STATUS_ERR;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        return ATC_FN_STATUS_INVALID_ARG;
    case PORT_UART_FN_STATUS_OK:
        return ATC_FN_STATUS_OK;
    default:
        return ATC_FN_STATUS_ERR;
    }
}

atc_fnStatus_t atc_Deinit(atc_mod_type module)
{
    port_uart_FnStatus_t rc = port_uart_Deinit((uart_port_t)module);
    switch (rc)
    {
    case PORT_UART_FN_STATUS_ERR:
        return ATC_FN_STATUS_ERR;
    case PORT_UART_FN_STATUS_INVALID_ARG:
        return ATC_FN_STATUS_INVALID_ARG;
    case PORT_UART_FN_STATUS_OK:
        return ATC_FN_STATUS_OK;
    default:
        return ATC_FN_STATUS_ERR;
    }
}

bool atcTestAT(atc_mod_type module)
{
    uint8_t cmd[] = "AT\r\n";
    uint8_t readBuff[5];
    memset(readBuff, 0, sizeof(readBuff));
    atc_Command(module, cmd, strlen((char *)cmd), readBuff, 2, 20);
    if (0 == strncmp(cmd, "OK", 2))
        return true;
    return false;
}

atc_fnStatus_t atc_GetModuleInfo(atc_mod_type module)
{
    return 0; // todo
}