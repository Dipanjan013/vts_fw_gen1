#ifndef _PORT_UART_H_
#define _PORT_UART_H_

#include <stdint.h>
#include <stddef.h>
#include "driver/uart.h"

#define PORT_UART_BUFF_SIZE 512

typedef enum port_uart_tagFnStatus
{
    PORT_UART_FN_STATUS_INVALID_ARG = -2,
    PORT_UART_FN_STATUS_ERR = -1,
    PORT_UART_FN_STATUS_OK = 0,
} port_uart_FnStatus_t;

port_uart_FnStatus_t port_uart_Init(uart_port_t uartNum, uint8_t txPin, uint8_t rxPin, uart_config_t *pUartConfig);

port_uart_FnStatus_t port_uart_Tx(uart_port_t uartNum, uint8_t *txData, size_t dataLen);

port_uart_FnStatus_t port_uart_Rx(uart_port_t uartNum, uint8_t *rxBuff, size_t buffSize, uint32_t waitMs);

port_uart_FnStatus_t port_uart_Deinit(uart_port_t uartNum);

#endif /* _PORT_UART_H_ */