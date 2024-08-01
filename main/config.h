#ifndef __CONFIG_H__
#define __CONFIG_H__

// Some useful macros for devs
#define WEAK __attribute__((weak))

/*! UART Configuration */
// ESP32 Node MCU dev kit
#define CONFIG_UART_NUM0 0
#define CONFIG_UART_NUM0_TX_PIN 1
#define CONFIG_UART_NUM0_RX_PIN 3
#define CONFIG_UART_NUM0_BAUD 115200

#define CONFIG_UART_NUM1 1
#define CONFIG_UART_NUM1_TX_PIN 0
#define CONFIG_UART_NUM1_RX_PIN 0
#define CONFIG_UART_NUM1_BAUD 115200

#define CONFIG_UART_NUM2 2
#define CONFIG_UART_NUM2_TX_PIN 17
#define CONFIG_UART_NUM2_RX_PIN 16
#define CONFIG_UART_NUM2_BAUD 115200

// for additional config, go to port_uart.c
#define CONFIG_UART_BUFF_SIZE 512

#endif /* __CONFIG_H__ */