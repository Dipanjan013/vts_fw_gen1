#include "service_led.h"
#include "port_timer.h"

static port_timer_hndle_t gLedTmrHndl;
static void TmrCb(void *arg);

typedef struct{
	port_led_colour_e led;
	uint16_t on_ms;
	uint16_t off_ms;
}ledStep_s;

typedef struct{
	service_led_pattern_e pattern;
	const ledStep_s *pSet;
	uint8_t cnt;
}ledPatternTbl_s;

const ledStep_s gBootupPattern[] = {
	{PORT_LED_COLOUR_BLUE, 500, 100},
	{PORT_LED_COLOUR_BLUE, 1000, 100}
};

const ledStep_s gRebootPattern[] = {
	{PORT_LED_COLOUR_BLUE, 200, 100}
};

const ledPatternTbl_s gPatternTbl[SERVICE_LED_PATTERN_MAX] = {
	{SERVICE_LED_PATTERN_BOOTUP, gBootupPattern, (sizeof(gBootupPattern)/sizeof(ledStep_s))},
	{SERVICE_LED_PATTERN_REBOOT, gRebootPattern, (sizeof(gRebootPattern)/sizeof(ledStep_s))}
};


/*********************************************************************************************************************
 *
 ********************************************************************************************************************/

static void TmrCb(void *arg)
{
	(void)(arg);
}

void service_led_Init(void)
{
	port_led_Init();
	port_led_Off(PORT_LED_COLOUR_BLUE);
	if(!gLedTmrHndl){
		port_timer_InitPeriodic(&gLedTmrHndl, TmrCb);
	}
}

void service_led_DeInit(void)
{
	if(port_timer_IsRunning(gLedTmrHndl)){
		port_timer_Stop(gLedTmrHndl);
	}
	port_led_Off(PORT_LED_COLOUR_ALL);
	port_led_DeInit();
}

void service_led_SetPattern(service_led_pattern_e pattern)
{
	(void)(pattern);
}
