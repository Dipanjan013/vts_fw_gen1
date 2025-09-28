/*
 * port_spi.h
 *
 *  Created on: June 18, 2025
 *  Author: Dipanjan
 */

#ifndef PORT_SPI_H_
#define PORT_SPI_H_

#include <stdint.h>
#include "port_gpio.h"
#include "stm32f4xx_hal.h"

#define PORT_SPI_ENABLE 0

#if(PORT_SPI_ENABLE)
typedef SPI_HandleTypeDef port_spi_hndl_t;

#define PORT_SPI_CS_PORT PORT_GPIO_PORT_A
#define PORT_SPI_CS_PIN  PORT_GPIO_PIN_4

typedef enum {
  PORT_SPI_FN_STATUS_INVALID_ARG = -2,
	PORT_SPI_FN_STATUS_FAIL = -1,
	PORT_SPI_FN_STATUS_OK = 0,
}port_spi_fnStatus_e;

typedef enum{
	PORT_SPI_INST1,
	PORT_SPI_INST2,
}port_spi_instance_e;

port_spi_fnStatus_e port_spi_Init(port_spi_hndl_t *pHndl, port_spi_instance_e instance);
port_spi_fnStatus_e port_spi_DeInit(port_spi_hndl_t *pHndl);
port_spi_fnStatus_e port_spi_Read(port_spi_hndl_t *pHndl, uint8_t *buff, uint16_t len, uint32_t timeoutMs);
port_spi_fnStatus_e port_spi_Write(port_spi_hndl_t *pHndl, uint8_t *buff, uint16_t len, uint32_t timeoutMs);
port_spi_fnStatus_e port_spi_Transceive(port_spi_hndl_t *pHndl, uint8_t *txBuff, uint8_t *rxBuff, uint16_t rxLen, uint32_t timeoutMs);
void port_spi_SetCSPinLow(port_spi_hndl_t *pHndl);
void port_spi_SetCSPinHigh(port_spi_hndl_t *pHndl);
#endif
#endif /* INTF_SPI_H_ */
