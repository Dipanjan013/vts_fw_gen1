#ifndef _PORT_UART_H_
#define _PORT_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "config.h"

typedef enum port_uart_tagFnStatus
{
    PORT_UART_FN_STATUS_INVALID_ARG = -2,
    PORT_UART_FN_STATUS_ERR = -1,
    PORT_UART_FN_STATUS_OK = 0,
} port_uart_FnStatus_t;

typedef enum port_uart_tagNum
{
    PORT_UART_NUM0 = CONFIG_UART_NUM0,
    PORT_UART_NUM1 = CONFIG_UART_NUM1,
    PORT_UART_NUM2 = CONFIG_UART_NUM2,
} port_uart_num_t;

port_uart_FnStatus_t port_uart_Init(port_uart_num_t uartNum);

port_uart_FnStatus_t port_uart_Tx(port_uart_num_t uartNum, uint8_t *txData, size_t dataLen);

port_uart_FnStatus_t port_uart_Rx(port_uart_num_t uartNum, uint8_t *rxBuff, size_t buffSize, uint32_t waitMs);

port_uart_FnStatus_t port_uart_Deinit(port_uart_num_t uartNum);

#endif /* _PORT_UART_H_ */