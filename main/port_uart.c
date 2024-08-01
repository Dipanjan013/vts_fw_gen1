//! Includes
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "port_uart.h"

//! defines
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

//! Variable declaration

static uart_config_t uartConfig1;

//! Variable definition
static uart_config_t uartConfig0 = {
    .baud_rate = CONFIG_UART_NUM0_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

static uart_config_t uartConfig1 = {
    .baud_rate = CONFIG_UART_NUM1_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

static uart_config_t uartConfig2 = {
    .baud_rate = CONFIG_UART_NUM2_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

//!===============================================================================================//

static port_uart_FnStatus_t ConfigUart(port_uart_num_t uartNum, uart_config_t *pUartConfig, uint16_t txPin, uint16_t rxPin)
{
    port_uart_FnStatus_t status = PORT_UART_FN_STATUS_ERR;
    if (pUartConfig == NULL)
    {
        return PORT_UART_FN_STATUS_INVALID_ARG;
    }
    do
    {
        if (ESP_OK != uart_driver_install((uart_port_t)uartNum, CONFIG_UART_BUFF_SIZE * 2, 0, 0, NULL, 0))
        {
            break;
        }
        if (ESP_OK != uart_param_config((uart_port_t)uartNum, pUartConfig))
        {
            break;
        }
        if (ESP_OK != uart_set_pin((uart_port_t)uartNum, txPin, rxPin, ECHO_TEST_RTS, ECHO_TEST_CTS))
        {
            break;
        }
        status = PORT_UART_FN_STATUS_OK;
    } while (0);
    return status;
}

port_uart_FnStatus_t port_uart_Init(port_uart_num_t uartNum)
{
    port_uart_FnStatus_t status = PORT_UART_FN_STATUS_ERR;
    switch (uartNum)
    {
    case PORT_UART_NUM0:
        status = ConfigUart(PORT_UART_NUM0, &uartConfig0, CONFIG_UART_NUM0_TX_PIN, CONFIG_UART_NUM0_RX_PIN);
        break;

    case PORT_UART_NUM1:
        status = ConfigUart(PORT_UART_NUM1, &uartConfig1, CONFIG_UART_NUM1_TX_PIN, CONFIG_UART_NUM1_RX_PIN);
        break;

    case PORT_UART_NUM2:
        status = ConfigUart(PORT_UART_NUM2, &uartConfig2, CONFIG_UART_NUM2_TX_PIN, CONFIG_UART_NUM2_RX_PIN);
        break;

    default:
        status = PORT_UART_FN_STATUS_INVALID_ARG;
        break;
    }
    return status;
}

port_uart_FnStatus_t port_uart_Tx(port_uart_num_t uartNum, uint8_t *txData, size_t dataLen)
{
    int bytesSent = uart_write_bytes((uart_port_t)uartNum, txData, dataLen);
    if (bytesSent > 0)
        return PORT_UART_FN_STATUS_OK;
    else
        return PORT_UART_FN_STATUS_ERR;
}

port_uart_FnStatus_t port_uart_Rx(port_uart_num_t uartNum, uint8_t *rxBuff, size_t buffSize, uint32_t waitMs)
{
    int bytesRecvd = 0;
    if (rxBuff == NULL)
        return PORT_UART_FN_STATUS_INVALID_ARG;
    bytesRecvd = uart_read_bytes((uart_port_t)uartNum, rxBuff, buffSize - 1, waitMs / portTICK_PERIOD_MS);
    if (bytesRecvd >= 0)
        return PORT_UART_FN_STATUS_OK;
    else
        return PORT_UART_FN_STATUS_ERR;
}

port_uart_FnStatus_t port_uart_Deinit(port_uart_num_t uartNum)
{
    if (ESP_OK != uart_driver_delete((uart_port_t)uartNum))
    {
        return PORT_UART_FN_STATUS_ERR;
    }
    return PORT_UART_FN_STATUS_OK;
}