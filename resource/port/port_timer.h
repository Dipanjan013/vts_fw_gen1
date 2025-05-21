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

typedef enum{
	PORT_TIMER_FN_STATUS_FAIL = -1,
	PORT_TIMER_FN_STATUS_OK = 0,
}port_timer_fnStatus_t;

typedef osTimerId_t port_timer_hndle_t;
typedef osTimerFunc_t port_timer_Callback_t;

port_timer_fnStatus_t port_timer_InitOneShot(port_timer_hndle_t handle, port_timer_Callback_t fn);
port_timer_fnStatus_t port_timer_InitPeriodic(port_timer_hndle_t handle, port_timer_Callback_t fn);
port_timer_fnStatus_t port_timer_StartOneShot(port_timer_hndle_t handle, uint32_t ms);
port_timer_fnStatus_t port_timer_StartPeriodic(port_timer_hndle_t handle, uint32_t ms);
port_timer_fnStatus_t port_timer_Stop(port_timer_hndle_t handle);
uint8_t port_timer_IsRunning(port_timer_hndle_t handle);
port_timer_fnStatus_t port_timer_DeInit(port_timer_hndle_t handle);
void port_timer_Delay(uint32_t ms);
uint32_t port_timer_GetMillis(void);

#endif /* PORT_PORT_TIMER_H_ */
