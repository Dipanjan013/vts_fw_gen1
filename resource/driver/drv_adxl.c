#include <stdio.h>
#include "port_i2c.h"
#include "drv_adxl.h"

#include "i2c.h"

/**!
 * @notes
 * To change interrupt, disable it first and then clear the bits in INT_SOURCE register and then set the interrupts in INT_ENABLE
 */

uint8_t drv_adxl_Init(drv_adxl_mode_e mode)
{
	port_i2c_fnStatus_e fn = PORT_I2C_FN_STATUS_FAIL;
	switch(mode){
		case DRV_ADXL_MODE_STREAM:{
			uint8_t writeData = 0x08;
			//Set power control register
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_PWR_CTL_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			//Set Data format register
			writeData = 0x00;
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_DATA_FORMAT_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
		}break;

		case DRV_ADXL_MODE_TAP_DETECT_SINGLE:{
			//Set the power control register
			uint8_t writeData = 0x08;
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_PWR_CTL_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			// Refer application note in data-sheet for recommended values
			writeData = 0x40;	// as per application info in data-sheet, it suggests > 3g. So, for 4g = 4/.0625 = 0x40
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_TAP_THRESH_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			writeData = 0x20;
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_DUR_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			writeData = 0x20;
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_LAT_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			writeData = 0x80;
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_WIN_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			writeData = 0x00;	//map to INT1
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_INT_MAP_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			uint8_t readByte = 0;
			fn = port_i2c_RegRead(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_INT_SOURCE_REG, &readByte, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			writeData = 0x00;	//disable the interrupt
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_INT_ENABLE_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			writeData = 0x40;	//enable the interrupt
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_INT_ENABLE_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
			writeData = 0x03;	//enable TAP detection on x, y & z
			fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_TAP_AXES_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
			if(fn != PORT_I2C_FN_STATUS_OK){
				break;
			}
		}break;

		default:
	}
	if(fn != PORT_I2C_FN_STATUS_OK){
		printf("[%s] mode : %d, Failed (%d)\r\n", __func__, mode, fn);
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
	fn = port_i2c_RegRead(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_DEVID_REG, &device_id, 1, PORT_I2C_DEF_TIMEOUT_MS);
	if (fn == PORT_I2C_FN_STATUS_OK) {
		return device_id;
	} else {
		printf("[%s] Failed (%d)\r\n", __func__, fn);
		return 0;
	}
}

uint8_t drv_adxl_Sleep(void)
{
	//Set the power control register
	uint8_t writeData = 0x04;
	fn = port_i2c_RegWrite(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_PWR_CTL_REG, &writeData, 1, PORT_I2C_DEF_TIMEOUT_MS);
	if(fn != PORT_I2C_FN_STATUS_OK){
		return 0;
	}
	return 1;
}

uint8_t drv_adxl_ReadAxesXYZ(float *pX, float *pY, float *pZ)
{
	port_i2c_fnStatus_e fn = PORT_I2C_FN_STATUS_FAIL;
	*pX = 0,*pY = 0,*pZ = 0;
	int16_t x = 0, y = 0, z = 0;
	uint8_t readData[6] = {0};
	//Read the data register 0x32 to 0x37
	fn = port_i2c_RegRead(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_DATA_START_REG, readData, 6, PORT_I2C_DEF_TIMEOUT_MS);
	if(fn != PORT_I2C_FN_STATUS_OK){
		printf("[%s] Failed (%d)\r\n", __func__, fn);
		return 0;
	}

	x = (readData[1] << 8) | readData[0];
	y = (readData[3] << 8) | readData[2];
	z = (readData[5] << 8) | readData[4];

	/*!
	 * As per datasheet,
	 * sensitivity for +=2g is 256 LSB/g (LSB is raw data from x/y/z axis)
	 * So, g = LSB / 256
	 */
	*pX = (float)x / (float)DRV_ADXL345_SENSITIVITY;
	*pY = (float)y / (float)DRV_ADXL345_SENSITIVITY;
	*pZ = (float)z / (float)DRV_ADXL345_SENSITIVITY;
	return 1;
}

uint8_t drv_adxl_ReadInterruptSrc(uint8_t *readByte)
{
	port_i2c_fnStatus_e fn = PORT_I2C_FN_STATUS_FAIL;
	fn = port_i2c_RegRead(ADXL_I2C_HNDL, DRV_ADXL345_ADDR, DRV_ADXL345_INT_SOURCE_REG, readByte, 1, PORT_I2C_DEF_TIMEOUT_MS);
	if(fn != PORT_I2C_FN_STATUS_OK){
		return 0;
	}else{
		return 1;
	}
}
