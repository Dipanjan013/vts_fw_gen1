#include "port_timer.h"

port_timer_fnStatus_t port_timer_InitOneShot(port_timer_hndle_t handle, port_timer_Callback_t fn)
{
	handle = osTimerNew(fn, osTimerOnce, NULL, NULL);
	if(!handle){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

port_timer_fnStatus_t port_timer_InitPeriodic(port_timer_hndle_t handle, port_timer_Callback_t fn)
{
	handle = osTimerNew(fn, osTimerPeriodic, NULL, NULL);
	if(!handle){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

port_timer_fnStatus_t port_timer_StartOneShot(port_timer_hndle_t handle, uint32_t ms)
{
	osStatus_t ret;
	ret = osTimerStart(handle, ms);
	if(ret != osOK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

port_timer_fnStatus_t port_timer_StartPeriodic(port_timer_hndle_t handle, uint32_t ms)
{
	osStatus_t ret;
	ret = osTimerStart(handle, ms);
	if(ret != osOK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

port_timer_fnStatus_t port_timer_Stop(port_timer_hndle_t handle)
{
	osStatus_t ret;
	ret = osTimerStop(handle);
	if(ret != osOK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

uint8_t port_timer_IsRunning(port_timer_hndle_t handle)
{
	uint32_t t;
	t = osTimerIsRunning(handle);
	if(t > 0){
		return 1;
	}else{
		return 0;
	}
}

port_timer_fnStatus_t port_timer_DeInit(port_timer_hndle_t handle)
{
	osStatus_t ret;
	ret = osTimerDelete(handle);
	if(ret != osOK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

void port_timer_Delay(uint32_t ms)
{
	osDelay(ms);
}

uint32_t port_timer_GetMillis(void)
{
	return osKernelGetTickCount();
}
