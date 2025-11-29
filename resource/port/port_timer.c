#include "port_timer.h"

__attribute__((weak)) void port_timer_HwCallback(port_timer_hwInstance_e instance)
{
	(void)(instance);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1){
		port_timer_HwCallback(PORT_TIMER_INST1);
	}else if(htim->Instance == TIM2){
		port_timer_HwCallback(PORT_TIMER_INST2);
	}else if(htim->Instance == TIM3){
		port_timer_HwCallback(PORT_TIMER_INST3);
	}else if(htim->Instance == TIM4){
		port_timer_HwCallback(PORT_TIMER_INST4);
	}else if(htim->Instance == TIM5){
		port_timer_HwCallback(PORT_TIMER_INST5);
	}else
		;
}

void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
	(void)(htim);
}

port_timer_fnStatus_t port_timer_InitOneShot(port_timer_hndle_t *phandle, port_timer_Callback_t fn)
{
	*phandle = osTimerNew(fn, osTimerOnce, NULL, NULL);
	if(*phandle == NULL){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

port_timer_fnStatus_t port_timer_InitPeriodic(port_timer_hndle_t *phandle, port_timer_Callback_t fn)
{
	*phandle = osTimerNew(fn, osTimerPeriodic, NULL, NULL);
	if(*phandle == NULL){
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

uint8_t port_timer_CalcPscArr(uint8_t fclk_mhz, uint16_t ms, uint16_t *pPsc, uint16_t *pArr)
{
	uint8_t flag = 0;
	float target_counts = ((fclk_mhz * 1000000U) / 1000U) * (float)ms;
	float temp_psc = 0;
	// Find max ARR under 65535 → start from max and find lowest PSC
	for (uint32_t arr = 0xFFFF; arr >= 1; arr--) {
		temp_psc = (target_counts / (float)arr);
		if((float)((uint32_t)(temp_psc)) != temp_psc){ //more precision
			continue;
		}
		if (temp_psc <= 0xFFFF) {
			*pArr = (uint16_t)(arr - 1);
			*pPsc = (uint16_t)(temp_psc - 1);
			flag = 1;
			break;
		}
	}

	if (!flag) {
//		printf("Failed to find suitable PSC/ARR\r\n");
		*pPsc = 0, pArr = 0;
		return 0;
	}
	// printf("PSC = %u, ARR = %u\r\n", PSC, ARR);
	return 1;
}

port_timer_fnStatus_t port_timer_HwTimInit(port_timer_hwTimHndl *pHndl, port_timer_hwInstance_e instance, uint16_t ms, uint16_t psc, uint16_t arr)
{
	HAL_StatusTypeDef ret;
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	switch(instance){
		case PORT_TIMER_INST3:
			pHndl->Instance = TIM3;
		break;
		default:
		return PORT_TIMER_FN_STATUS_ERR_ARG;
	}
	pHndl->Init.Prescaler = psc;
	pHndl->Init.CounterMode = TIM_COUNTERMODE_UP;
	pHndl->Init.Period = arr;
	pHndl->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	pHndl->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	ret = HAL_TIM_Base_Init(pHndl);
	if(ret != HAL_OK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	ret = HAL_TIM_ConfigClockSource(pHndl, &sClockSourceConfig);
	if (ret != HAL_OK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	ret = HAL_TIMEx_MasterConfigSynchronization(pHndl, &sMasterConfig);
	if (ret != HAL_OK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

port_timer_fnStatus_t port_timer_HwTimDeinit(port_timer_hwTimHndl *pHndl)
{
	HAL_StatusTypeDef ret;
	ret = HAL_TIM_Base_DeInit(pHndl);
	if(ret != HAL_OK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

port_timer_fnStatus_t port_timer_HwTimStartIT(port_timer_hwTimHndl *pHndl)
{
	HAL_StatusTypeDef ret;
	ret = HAL_TIM_Base_Start_IT(pHndl);
	if (ret != HAL_OK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

port_timer_fnStatus_t port_timer_HwTimStopIT(port_timer_hwTimHndl *pHndl)
{
	HAL_StatusTypeDef ret;
	ret = HAL_TIM_Base_Stop_IT(pHndl);
	if (ret != HAL_OK){
		return PORT_TIMER_FN_STATUS_FAIL;
	}
	return PORT_TIMER_FN_STATUS_OK;
}

uint8_t port_timer_HwTimIsRunning(port_timer_hwTimHndl *pHndl)
{
	HAL_TIM_StateTypeDef state;
	state = HAL_TIM_Base_GetState(pHndl);
	if(HAL_TIM_STATE_BUSY == state){
		return 1;
	}
	return 0;
}

void port_timer_Delay(uint32_t ms)
{
//	while(ms--){
//		osDelay(1);
//	}
	osDelay(ms);
}

uint32_t port_timer_GetMillis(void)
{
	return osKernelGetTickCount();
}

uint32_t port_timer_GetElapsedTimeMs(uint32_t startTimeMs)
{
	return (osKernelGetTickCount() - startTimeMs);
}

uint32_t port_timer_GetUnixTime(void)
{
	return 0;
}
