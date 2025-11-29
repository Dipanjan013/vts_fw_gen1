/*
 * port_timer.h
 *
 *  Created on: Jan 11, 2025
 *      Author: Dipan
 */

#ifndef PORT_PORT_TIMER_H_
#define PORT_PORT_TIMER_H_

#include <stdint.h>
#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

typedef enum{
	PORT_TIMER_FN_STATUS_ERR_ARG = -2,
	PORT_TIMER_FN_STATUS_FAIL = -1,
	PORT_TIMER_FN_STATUS_OK = 0,
}port_timer_fnStatus_t;

typedef enum{
	PORT_TIMER_INST1,
	PORT_TIMER_INST2,
	PORT_TIMER_INST3,
	PORT_TIMER_INST4,
	PORT_TIMER_INST5,
}port_timer_hwInstance_e;

typedef osTimerId_t port_timer_hndle_t;
typedef osTimerFunc_t port_timer_Callback_t;
typedef TIM_HandleTypeDef port_timer_hwTimHndl;

port_timer_fnStatus_t port_timer_InitOneShot(port_timer_hndle_t *phandle, port_timer_Callback_t fn);
port_timer_fnStatus_t port_timer_InitPeriodic(port_timer_hndle_t *phandle, port_timer_Callback_t fn);
port_timer_fnStatus_t port_timer_StartOneShot(port_timer_hndle_t handle, uint32_t ms);
port_timer_fnStatus_t port_timer_StartPeriodic(port_timer_hndle_t handle, uint32_t ms);
port_timer_fnStatus_t port_timer_Stop(port_timer_hndle_t handle);
uint8_t port_timer_IsRunning(port_timer_hndle_t handle);
port_timer_fnStatus_t port_timer_DeInit(port_timer_hndle_t handle);
void port_timer_Delay(uint32_t ms);
uint32_t port_timer_GetMillis(void);
uint32_t port_timer_GetElapsedTimeMs(uint32_t startTimeMs);
uint32_t port_timer_GetUnixTime(void);

/*!
 * @fn API to calculate the Prescaler register value & auto-reload register value
 * @note : 2bytes max for PSc and ARR
 * Not recommended to use this as multiple float calculation are done. You can refer online calculator as well
 * @param ms - timeout interval. Max = 65535ms
 * @param fclk_mhz : clock frequency (E.g if 16MHz, fclk_mhz = 16)
 * @param pPsc - pointer to 16bit prescaler variable
 * @param pArr - pointer to 16bit auto reload variable
 * @return 1 or 0
 */
uint8_t port_timer_CalcPscArr(uint8_t fclk_mhz, uint16_t ms, uint16_t *pPsc, uint16_t *pArr);

/*!
 * @fn Hardware timer initialize function. It supports max delay of 65535, arr & psc = 2bytes each.
 * @note This function can be modified inorder to use ns and us timeout along 4byte of psc and arr. ref data-sheet
 * @param pHndl
 * @param port_timer_hwInstance_e - timer instance to be used
 * @param ms
 * @param psc
 * @param arr
 * @return
 */
port_timer_fnStatus_t port_timer_HwTimInit(port_timer_hwTimHndl *pHndl, port_timer_hwInstance_e instance, uint16_t ms, uint16_t psc, uint16_t arr);
port_timer_fnStatus_t port_timer_HwTimDeinit(port_timer_hwTimHndl *pHndl);
port_timer_fnStatus_t port_timer_HwTimStartIT(port_timer_hwTimHndl *pHndl);
port_timer_fnStatus_t port_timer_HwTimStopIT(port_timer_hwTimHndl *pHndl);
uint8_t port_timer_HwTimIsRunning(port_timer_hwTimHndl *pHndl);
void port_timer_HwCallback(port_timer_hwInstance_e instance);

#endif /* PORT_PORT_TIMER_H_ */
;
