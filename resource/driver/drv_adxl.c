#include <stdio.h>
#include "port_i2c.h"
#include "drv_adx.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t drv_adxl_Init(drv_adxl_mode_e mode)
{
	port_i2c_fnStatus_e fn = PORT_I2C_FN_STATUS_FAIL;
	switch(mode){
		case DRV_ADXL_MODE_STREAM:{
			uint8_t writeData = 0x08;
			//Set power control register
			fn = port_i2c_RegWrite(&hi2c1, DRV_ADXL345_ADDR, DRV_ADXL345_PWR_CTL_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			//Set Data format register
			writeData = 0x00;
			fn = port_i2c_RegWrite(&hi2c1, DRV_ADXL345_ADDR, DRV_ADXL345_DATA_FORMAT_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
		}break;
		default:
			return 0;
	}

	if(fn != PORT_I2C_FN_STATUS_OK){
		printf("[%s] Failed (%d)\r\n", __func__, fn);
		return 0;
	}
	return 1;
}

uint8_t drv_adxl_DeInit()
{
	return 0;
}

uint8_t drv_adxl_GetDevId(void)
{
	port_i2c_fnStatus_e fn = PORT_I2C_FN_STATUS_FAIL;
	uint8_t device_id = 0;
	fn = port_i2c_RegRead(&hi2c1, DRV_ADXL345_ADDR, DRV_ADXL345_DEVID_REG, &device_id, 1, PORT_I2C_DEF_TIMEOUT_MS);
	if (fn == PORT_I2C_FN_STATUS_OK) {
		return device_id;
	} else {
		printf("[%s] Failed (%d)\r\n", __func__, fn);
		return 0;
	}
}

void drv_adxl_Sleep(void)
{
}

uint8_t drv_adxl_ReadAxesXYZ(float *pX, float *pY, float *pZ)
{
	port_i2c_fnStatus_e fn = PORT_I2C_FN_STATUS_FAIL;
	*pX = 0,*pY = 0,*pZ = 0;
	int16_t x = 0, y = 0, z = 0;
	uint8_t readData[6] = {0};
	//Read the data register 0x32 to 0x37
	fn = port_i2c_RegRead(&hi2c1, DRV_ADXL345_ADDR, 0x32, readData, 6, PORT_I2C_DEF_TIMEOUT_MS);
	if(fn != PORT_I2C_FN_STATUS_OK){
		printf("[%s] Failed (%d)\r\n", __func__, fn);
		return 0;
	}

	x = (readData[1] << 8) | readData[0];
	y = (readData[3] << 8) | readData[2];
	z = (readData[5] << 8) | readData[4];

	*pX = (float)x / (float)DRV_ADXL345_SENSITIVITY;
	*pY = (float)y / (float)DRV_ADXL345_SENSITIVITY;
	*pZ = (float)z / (float)DRV_ADXL345_SENSITIVITY;
	return 1;
}
