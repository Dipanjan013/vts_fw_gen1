#include "port_i2c.h"

static volatile uint8_t gFlagXferCmplt = 0;
static volatile uint8_t gFlagXferErr = 0;
static inline void ClearFlags(void);
/************************************************************************************************************
 *
 ***********************************************************************************************************/

__attribute__((weak)) void port_i2c_CmpltCallback(port_i2c_handle_t *pHndl)
{
	(void)(pHndl);
}

__attribute__((weak)) void port_i2c_ErrorCallback(port_i2c_handle_t *pHndl)
{
	(void)(pHndl);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	(void)(hi2c);
	gFlagXferCmplt = 1;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	(void)(hi2c);
	gFlagXferCmplt = 1;
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	(void)(hi2c);
	gFlagXferCmplt = 1;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	(void)(hi2c);
	gFlagXferCmplt = 1;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	(void)(hi2c);
	gFlagXferErr = 1;
}

static inline void ClearFlags(void)
{
	gFlagXferCmplt = 0;
	gFlagXferErr = 0;
}

port_i2c_fnStatus_e port_i2c_Init(port_i2c_handle_t *pHndl, port_i2c_instance_e instance)
{
	HAL_StatusTypeDef ret;
	switch(instance){
		case PORT_I2C_INST1:
			pHndl->Instance = I2C1;
			break;
		case PORT_I2C_INST2:
			pHndl->Instance = I2C2;
			break;
		default:
			return PORT_I2C_FN_STATUS_INVALID_ARG;
	}
	pHndl->Init.ClockSpeed = 100000;
	pHndl->Init.DutyCycle = I2C_DUTYCYCLE_2;
	pHndl->Init.OwnAddress1 = 0;
	pHndl->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	pHndl->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	pHndl->Init.OwnAddress2 = 0;
	pHndl->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	pHndl->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	ret = HAL_I2C_Init(pHndl);
	if (ret != HAL_OK){
		return PORT_I2C_FN_STATUS_FAIL;
	}
	return PORT_I2C_FN_STATUS_OK;
}

port_i2c_fnStatus_e port_i2c_DeInit(port_i2c_handle_t *pHndl)
{
	HAL_StatusTypeDef ret;
	if(!pHndl){
		return PORT_I2C_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_I2C_DeInit(pHndl);
	if (ret != HAL_OK){
		return PORT_I2C_FN_STATUS_FAIL;
	}
	return PORT_I2C_FN_STATUS_OK;
}

void port_i2c_Scan(port_i2c_handle_t *pHndl)
{
    HAL_StatusTypeDef ret;
    uint8_t i;
    printf("Scanning I2C bus...\r\n");
    for (i = 1; i < 128; i++){ // valid 7-bit addresses: 0x01..0x7F
			ret = HAL_I2C_IsDeviceReady(pHndl, (uint16_t)(i << 1), 1, 10);
			if (ret == HAL_OK) {
				printf("Found device at 0x%02X\r\n", i);
			}
    }
    printf("Scan complete.\r\n");
}


port_i2c_fnStatus_e port_i2c_Write(port_i2c_handle_t *pHndl, uint16_t address, uint8_t *pdata, uint16_t len, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret;
	ClearFlags();
	port_i2c_fnStatus_e rc = PORT_I2C_FN_STATUS_FAIL;
	if((!pHndl) || (!pdata) || (len == 0)){
		return PORT_I2C_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_I2C_Master_Transmit_IT(pHndl, address, pdata, len);
	if((ret == HAL_OK) || (ret == HAL_BUSY)){
		rc = PORT_I2C_FN_STATUS_TIMEOUT;
		while(timeoutMs--){
			if(gFlagXferCmplt){
				rc = PORT_I2C_FN_STATUS_OK;
				break;
			}
			if(gFlagXferErr){
				rc = PORT_I2C_FN_STATUS_FAIL;
				break;
			}
			HAL_Delay(1);
		}
	}
	return rc;
}

port_i2c_fnStatus_e port_i2c_Read(port_i2c_handle_t *pHndl, uint16_t address, uint8_t *pdata, uint16_t len, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret;
	ClearFlags();
	port_i2c_fnStatus_e rc = PORT_I2C_FN_STATUS_FAIL;
	if((!pHndl) || (!pdata) || (len == 0)){
		return PORT_I2C_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_I2C_Master_Receive_IT(pHndl, address, pdata, len);
	if((ret == HAL_OK) || (ret == HAL_BUSY)){
		rc = PORT_I2C_FN_STATUS_TIMEOUT;
		while(timeoutMs--){
			if(gFlagXferCmplt){
				rc = PORT_I2C_FN_STATUS_OK;
				break;
			}
			if(gFlagXferErr){
				rc = PORT_I2C_FN_STATUS_FAIL;
				break;
			}
			HAL_Delay(1);
		}
	}
	return rc;
}

port_i2c_fnStatus_e port_i2c_RegWrite(port_i2c_handle_t *pHndl, uint16_t address, uint16_t regAddress, uint8_t *pdata, uint16_t len, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret;
	ClearFlags();
	port_i2c_fnStatus_e rc = PORT_I2C_FN_STATUS_FAIL;
	if((!pHndl) || (!pdata) || (len == 0)){
		return PORT_I2C_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_I2C_Mem_Write_IT(pHndl, address, regAddress, PORT_I2C_MEM_ADDR_8BIT, pdata, len);
	if((ret == HAL_OK) || (ret == HAL_BUSY)){
		rc = PORT_I2C_FN_STATUS_TIMEOUT;
		while(timeoutMs--){
			if(gFlagXferCmplt){
				rc = PORT_I2C_FN_STATUS_OK;
				break;
			}
			if(gFlagXferErr){
				rc = PORT_I2C_FN_STATUS_FAIL;
				break;
			}
			HAL_Delay(1);
		}
	}
	return rc;
}

port_i2c_fnStatus_e port_i2c_RegRead(port_i2c_handle_t *pHndl, uint16_t address, uint16_t regAddress, uint8_t *pdata, uint16_t len, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret;
	ClearFlags();
	port_i2c_fnStatus_e rc = PORT_I2C_FN_STATUS_FAIL;
	if((!pHndl) || (!pdata) || (len == 0)){
		return PORT_I2C_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_I2C_Mem_Read_IT(pHndl, address, regAddress, PORT_I2C_MEM_ADDR_8BIT, pdata, len);
	if((ret == HAL_OK) || (ret == HAL_BUSY)){
		rc = PORT_I2C_FN_STATUS_TIMEOUT;
		while(timeoutMs--){
			if(gFlagXferCmplt){
				rc = PORT_I2C_FN_STATUS_OK;
				break;
			}
			if(gFlagXferErr){
				rc = PORT_I2C_FN_STATUS_FAIL;
				break;
			}
			HAL_Delay(1);
		}
	}
	return rc;
}

port_i2c_fnStatus_e port_i2c_CheckDevice(port_i2c_handle_t *pHndl, uint16_t address, uint16_t attempts, uint32_t timeoutMs)
{
	HAL_StatusTypeDef ret;
	if(!pHndl){
		return PORT_I2C_FN_STATUS_INVALID_ARG;
	}
	ret = HAL_I2C_IsDeviceReady(pHndl, address, attempts, timeoutMs);
	if(ret != HAL_OK){
		return PORT_I2C_FN_STATUS_FAIL;
	}
	return PORT_I2C_FN_STATUS_OK;
}
