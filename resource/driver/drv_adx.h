/*
 * drv_adx.h
 *
 *  Created on: Jun 28, 2025
 *      Author: Dipanjan
 */

#ifndef SRC_DRV_ADX_H_
#define SRC_DRV_ADX_H_

#include <stdint.h>

//Register addresses
#define DRV_ADXL345_ADDR (0x53 << 1)
#define DRV_ADXL345_DEVID_REG 0x00
#define DRV_ADXL345_PWR_CTL_REG 0x2D
#define DRV_ADXL345_INT_SOURCE_REG 0x30
#define DRV_ADXL345_DATA_FORMAT_REG 0x31

#define DRV_ADXL345_SENSITIVITY 256	// 1/256 for +-2G

typedef enum{
	DRV_ADXL_MODE_STREAM = 0,
	DRV_ADXL_MODE_TAP_DETECT_SINGLE,
}drv_adxl_mode_e;

uint8_t drv_adxl_Init(drv_adxl_mode_e mode);
uint8_t drv_adxl_DeInit();
uint8_t drv_adxl_GetDevId(void);
void drv_adxl_Sleep(void);
uint8_t drv_adxl_ReadAxesXYZ(float *pX, float *pY, float *pZ);

#endif /* SRC_DRV_ADX_H_ */
