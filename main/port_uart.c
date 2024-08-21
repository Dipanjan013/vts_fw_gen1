//! Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "port_uart.h"

//! defines
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

//!===============================================================================================//

port_uart_FnStatus_t port_uart_Init(uart_port_t uartNum, uint8_t txPin, uint8_t rxPin, uart_config_t *pUartConfig)
{
    port_uart_FnStatus_t status = PORT_UART_FN_STATUS_ERR;
    if (pUartConfig == NULL) // other param check not required as it is already done in driver layer
    {
        return PORT_UART_FN_STATUS_INVALID_ARG;
    }
    do
    {
        if (ESP_OK != uart_driver_install(uartNum, PORT_UART_BUFF_SIZE * 2, 0, 0, NULL, 0))
        {
            break;
        }
        if (ESP_OK != uart_param_config(uartNum, pUartConfig))
        {
            break;
        }
        if (ESP_OK != uart_set_pin(uartNum, txPin, rxPin, ECHO_TEST_RTS, ECHO_TEST_CTS))
        {
            break;
        }
        status = PORT_UART_FN_STATUS_OK;
    } while (0);
    return status;
}

port_uart_FnStatus_t port_uart_Tx(uart_port_t uartNum, uint8_t *txData, size_t dataLen)
{
    if (txData == NULL || dataLen > PORT_UART_BUFF_SIZE)
        return PORT_UART_FN_STATUS_INVALID_ARG;
    int bytesSent = uart_write_bytes(uartNum, txData, dataLen);
    if (bytesSent > 0)
        return PORT_UART_FN_STATUS_OK;
    else
        return PORT_UART_FN_STATUS_ERR;
}

port_uart_FnStatus_t port_uart_Rx(uart_port_t uartNum, uint8_t *rxBuff, size_t buffSize, uint32_t waitMs)
{
    if (rxBuff == NULL || buffSize > PORT_UART_BUFF_SIZE)
        return PORT_UART_FN_STATUS_INVALID_ARG;
    int bytesRecvd = 0;
    bytesRecvd = uart_read_bytes(uartNum, rxBuff, buffSize - 1, waitMs / portTICK_PERIOD_MS);
    if (bytesRecvd >= 0)
        return PORT_UART_FN_STATUS_OK;
    else
        return PORT_UART_FN_STATUS_ERR;
}

port_uart_FnStatus_t port_uart_Deinit(uart_port_t uartNum)
{
    if (ESP_OK != uart_driver_delete(uartNum))
    {
        return PORT_UART_FN_STATUS_ERR;
    }
    return PORT_UART_FN_STATUS_OK;
}