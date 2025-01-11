#include "port_timer.h"

#include "cmsis_os2.h"

void port_timer_DelayMs(uint16_t ms)
{
	osDelay(ms);
}
