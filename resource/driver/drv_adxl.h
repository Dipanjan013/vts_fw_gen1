/*
 * drv_adx.h
 *
 *  Created on: Jun 28, 2025
 *      Author: Dipanjan
 */

#ifndef SRC_DRV_ADX_H_
#define SRC_DRV_ADX_H_

#include <stdint.h>
#include "i2c.h"

#define ADXL_I2C_HNDL (&hi2c2)

//Register addresses
#define DRV_ADXL345_ADDR (uint8_t)(0x53 << 1)
#define DRV_ADXL345_DEVID_REG (uint8_t)0x00
#define DRV_ADXL345_TAP_AXES_REG (uint8_t)0x2A
#define DRV_ADXL345_TAP_THRESH_REG (uint8_t)0x1D
#define DRV_ADXL345_DUR_REG (uint8_t)0x21
#define DRV_ADXL345_LAT_REG (uint8_t)0x22
#define DRV_ADXL345_WIN_REG (uint8_t)0x23
#define DRV_ADXL345_PWR_CTL_REG (uint8_t)0x2D
#define DRV_ADXL345_INT_MAP_REG (uint8_t)0x2F
#define DRV_ADXL345_INT_SOURCE_REG (uint8_t)0x30
#define DRV_ADXL345_INT_ENABLE_REG (uint8_t)0x2E
#define DRV_ADXL345_DATA_FORMAT_REG (uint8_t)0x31
#define DRV_ADXL345_DATA_START_REG (uint8_t)0x32
#define DRV_ADXL345_DATA_END_REG (uint8_t)0x37
#define DRV_ADXL345_FIFO_CTRL_REG (uint8_t)0x38
#define DRV_ADXL345_FIFO_STATUS_REG (uint8_t)0x39

#define DRV_ADXL345_SENSITIVITY 256U	// 1/256 for +-2G

typedef enum {
	DRV_ADXL345_INT_DATA_READY = 0x80,
	DRV_ADXL345_INT_SINGLE_TAP = 0x40,
//	DRV_ADXL345_INT_DOUBLE_TAP = 0x20,
//	DRV_ADXL345_INT_ACTIVITY   = 0x10,
//	DRV_ADXL345_INT_INACTIVITY = 0x08,
//	DRV_ADXL345_INT_FREE_FALL  = 0x04,
//	DRV_ADXL345_INT_WATERMARK  = 0x02,
//	DRV_ADXL345_INT_OVERRUN    = 0x01
} ADXL345_IntSource_t;

typedef enum{
	DRV_ADXL_MODE_STREAM = 0,
	DRV_ADXL_MODE_TAP_DETECT_SINGLE,
}drv_adxl_mode_e;

uint8_t drv_adxl_Init(drv_adxl_mode_e mode);
uint8_t drv_adxl_DeInit();
uint8_t drv_adxl_GetDevId(void);
void drv_adxl_Sleep(void);
uint8_t drv_adxl_ReadAxesXYZ(float *pX, float *pY, float *pZ);
uint8_t drv_adxl_ReadInterruptSrc(uint8_t *readByte);

#endif /* SRC_DRV_ADX_H_ */
