/*
 * port_i2c.h
 *
 *  Created on: Jun 20, 2025
 *      Author: Dipan
 */

#ifndef SRC_PORT_I2C_H_
#define SRC_PORT_I2C_H_

#include "stm32f4xx_hal.h"

#define PORT_I2C_DEF_TIMEOUT_MS 3000
#define PORT_I2C_MEM_ADDR_8BIT I2C_MEMADD_SIZE_8BIT
#define PORT_I2C_MEM_ADDR_16BIT I2C_MEMADD_SIZE_16BIT

typedef I2C_HandleTypeDef port_i2c_handle_t;

typedef enum{
	PORT_I2C_INST1,
	PORT_I2C_INST2,
}port_i2c_instance_e;

typedef enum{
	PORT_I2C_FN_STATUS_INVALID_ARG = -4,
	PORT_I2C_FN_STATUS_BUSY = -3,
	PORT_I2C_FN_STATUS_TIMEOUT = -2,
	PORT_I2C_FN_STATUS_FAIL = -1,
	PORT_I2C_FN_STATUS_OK = 0,
}port_i2c_fnStatus_e;

/**
 * @brief Initializes the specified I2C instance and prepares the handle.
 *
 * @param[in,out] pHndl      Pointer to I2C handle structure.
 * @param[in]     instance   I2C instance (e.g., I2C1, I2C2).
 * @return port_i2c_fnStatus_e  Status of the initialization.
 */
port_i2c_fnStatus_e port_i2c_Init(port_i2c_handle_t *pHndl, port_i2c_instance_e instance);

void port_i2c_Scan(port_i2c_handle_t *pHndl);

/**
 * @brief Deinitializes the I2C handle and underlying hardware resources.
 *
 * @param[in,out] pHndl      Pointer to I2C handle structure.
 * @return port_i2c_fnStatus_e  Status of the deinitialization.
 */
port_i2c_fnStatus_e port_i2c_DeInit(port_i2c_handle_t *pHndl);

/**
 * @brief Performs an I2C master write to a device in interrupt mode.
 *
 * @param[in]  pHndl        Pointer to I2C handle structure.
 * @param[in]  address      7-bit I2C slave address (left-aligned).
 * @param[in]  pdata        Pointer to data buffer to send.
 * @param[in]  len          Number of bytes to transmit.
 * @param[in]  timeoutMs    Timeout duration in milliseconds.
 * @return port_i2c_fnStatus_e  Status of the write operation.
 */
port_i2c_fnStatus_e port_i2c_Write(port_i2c_handle_t *pHndl, uint16_t address, uint8_t *pdata, uint16_t len, uint32_t timeoutMs);

/**
 * @brief Performs an I2C master read from a device in interrupt mode.
 *
 * @param[in]  pHndl        Pointer to I2C handle structure.
 * @param[in]  address      7-bit I2C slave address (left-aligned).
 * @param[out] pdata        Pointer to buffer for receiving data.
 * @param[in]  len          Number of bytes to read.
 * @param[in]  timeoutMs    Timeout duration in milliseconds.
 * @return port_i2c_fnStatus_e  Status of the read operation.
 */
port_i2c_fnStatus_e port_i2c_Read(port_i2c_handle_t *pHndl, uint16_t address, uint8_t *pdata, uint16_t len, uint32_t timeoutMs);

/**
 * @brief Writes data to a specific register of an I2C slave device.
 *
 * @param[in] pHndl         Pointer to I2C handle structure.
 * @param[in] address       7-bit I2C slave address (left-aligned).
 * @param[in] regAddress    Register address to write to.
 * @param[in] pdata         Pointer to data buffer.
 * @param[in] len           Number of bytes to write.
 * @param[in] timeoutMs     Timeout duration in milliseconds.
 * @return port_i2c_fnStatus_e  Status of the register write operation.
 */
port_i2c_fnStatus_e port_i2c_RegWrite(port_i2c_handle_t *pHndl, uint16_t address, uint16_t regAddress, uint8_t *pdata, uint16_t len, uint32_t timeoutMs);

/**
 * @brief Reads data from a specific register of an I2C slave device.
 *
 * @param[in]  pHndl        Pointer to I2C handle structure.
 * @param[in]  address      7-bit I2C slave address (left-aligned).
 * @param[in]  regAddress   Register address to read from.
 * @param[out] pdata        Pointer to buffer for received data.
 * @param[in]  len          Number of bytes to read.
 * @param[in]  timeoutMs    Timeout duration in milliseconds.
 * @return port_i2c_fnStatus_e  Status of the register read operation.
 */
port_i2c_fnStatus_e port_i2c_RegRead(port_i2c_handle_t *pHndl, uint16_t address, uint16_t regAddress, uint8_t *pdata, uint16_t len, uint32_t timeoutMs);

/**
 * @brief Checks if a device is ready (responding) on the I2C bus.
 * @note This is a blocking API
 * @param[in] pHndl         Pointer to I2C handle structure.
 * @param[in] address       7-bit I2C slave address (left-aligned).
 * @param[in] attempts      Number of retry attempts.
 * @param[in] timeoutMs     Timeout per attempt in milliseconds.
 * @return port_i2c_fnStatus_e  Result of the device readiness check.
 */
port_i2c_fnStatus_e port_i2c_CheckDevice(port_i2c_handle_t *pHndl, uint16_t address, uint16_t attempts, uint32_t timeoutMs);

/**
 * @brief I2C transfer complete callback. Should be called from HAL_I2C_MasterTxCpltCallback or HAL_I2C_MasterRxCpltCallback.
 *
 * @param[in] pHndl         Pointer to I2C handle structure.
 */
void port_i2c_CmpltCallback(port_i2c_handle_t *pHndl);

/**
 * @brief I2C error callback. Should be called from HAL_I2C_ErrorCallback.
 *
 * @param[in] pHndl         Pointer to I2C handle structure.
 */
void port_i2c_ErrorCallback(port_i2c_handle_t *pHndl);


#endif /* SRC_PORT_I2C_H_ */
