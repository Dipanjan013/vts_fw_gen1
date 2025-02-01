#include "port_timer.h"

//#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

void port_timer_DelayMs(uint16_t ms)
{
//	osDelay(ms);
	HAL_Delay(ms);
}
