#include "port_spi.h"
#include <stdio.h>

port_spi_fnStatus_e port_spi_Init(port_spi_hndl_t *pHndl, port_spi_instance_e instance)
{
	switch(instance){
		case PORT_SPI_INST1:
			pHndl->Instance = SPI1;
			break;
		case PORT_SPI_INST2:
			pHndl->Instance = SPI2;
			break;
		default:
			return PORT_SPI_FN_STATUS_INVALID_ARG;
	}
	pHndl->Init.Mode = SPI_MODE_MASTER;
	pHndl->Init.Direction = SPI_DIRECTION_2LINES;
	pHndl->Init.DataSize = SPI_DATASIZE_8BIT;
	pHndl->Init.CLKPolarity = SPI_POLARITY_LOW;
	pHndl->Init.CLKPhase = SPI_PHASE_1EDGE;
	pHndl->Init.NSS = SPI_NSS_SOFT;
	pHndl->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	pHndl->Init.FirstBit = SPI_FIRSTBIT_MSB;
	pHndl->Init.TIMode = SPI_TIMODE_DISABLE;
	pHndl->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	pHndl->Init.CRCPolynomial = 10;
	if(HAL_SPI_Init(pHndl) != HAL_OK){
		return PORT_SPI_FN_STATUS_FAIL;
	}
	return PORT_SPI_FN_STATUS_OK;
}

port_spi_fnStatus_e port_spi_DeInit(port_spi_hndl_t *pHndl)
{
	HAL_StatusTypeDef ret;
	if(!pHndl){
		return PORT_SPI_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_SPI_DeInit(pHndl);
	if(HAL_SPI_Init(pHndl) != HAL_OK){
		return PORT_SPI_FN_STATUS_FAIL;
	}
	return PORT_SPI_FN_STATUS_OK;
}

port_spi_fnStatus_e port_spi_Read(port_spi_hndl_t *pHndl, uint8_t *buff, uint16_t len, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret;
	if(!pHndl){
		return PORT_SPI_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_SPI_Receive(pHndl, buff, len, timeoutMs);
	if(HAL_OK != ret){
		return PORT_SPI_FN_STATUS_FAIL;
	}
	return PORT_SPI_FN_STATUS_OK;
}

port_spi_fnStatus_e port_spi_Write(port_spi_hndl_t *pHndl, uint8_t *buff, uint16_t len, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret;
	if(!pHndl){
		return PORT_SPI_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_SPI_Transmit(pHndl, buff, len, timeoutMs);
	if(HAL_OK != ret){
		return PORT_SPI_FN_STATUS_FAIL;
	}
	return PORT_SPI_FN_STATUS_OK;
}

port_spi_fnStatus_e port_spi_Transceive(port_spi_hndl_t *pHndl, uint8_t *txBuff, uint8_t *rxBuff, uint16_t rxLen, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret;
	if(!pHndl){
		return PORT_SPI_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_SPI_TransmitReceive(pHndl, txBuff, rxBuff, rxLen, timeoutMs);
	if(HAL_OK != ret){
		return PORT_SPI_FN_STATUS_FAIL;
	}
	return PORT_SPI_FN_STATUS_OK;
}

void port_spi_SetCSPinLow(port_spi_hndl_t *pHndl)
{
	(void)(pHndl);
	HAL_GPIO_WritePin(PORT_SPI_CS_PORT, PORT_SPI_CS_PIN, GPIO_PIN_RESET);
}

void port_spi_SetCSPinHigh(port_spi_hndl_t *pHndl)
{
	(void)(pHndl);
	HAL_GPIO_WritePin(PORT_SPI_CS_PORT, PORT_SPI_CS_PIN, GPIO_PIN_SET);
}
