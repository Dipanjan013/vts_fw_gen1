#include "service_btn.h"
#include "port_timer.h"
#include "tim.h"
#include <string.h>
#include <stdio.h>

typedef enum{
	BTN_WAIT_FOR_PRESS,
	BTN_WAIT_FOR_RELEASE,
	BTN_COMPLETE,
}btnState_e;

typedef enum{
  SHORT_PRESS,
  LONG_PRESS,
	INVALID_PRESS,
}press_e;

typedef struct{
	service_btn_pattern_t pattern;
  press_e pressArr[SERVICE_BTN_MAX_PRESS];
  uint8_t cnt;
}patternTable_s;

static port_timer_hndle_t gBtnTmrHnd;
static patternTable_s gCurrentPattern;
static btnState_e gBtnState = BTN_WAIT_FOR_PRESS;

static const patternTable_s gPatRegTable[SERVICE_BTN_PATTERN_MAX] = {
	{SERVICE_BTN_PATTERN_ONBOARDING, {SHORT_PRESS, SHORT_PRESS, SHORT_PRESS}, 3},
	{SERVICE_BTN_PATTERN_REBOOT, {LONG_PRESS}, 1},
	{SERVICE_BTN_PATTERN_CLEARALL, {LONG_PRESS, LONG_PRESS}, 2},
	{SERVICE_BTN_PATTERN_TESTMODE, {LONG_PRESS, SHORT_PRESS}, 2}
};
/***************************************************************************************************************
 *
 ***************************************************************************************************************/

__attribute__((weak)) void service_btn_EventCb(service_btn_pattern_t pattern)
{
	(void)(pattern);
}

void service_btn_IrqCb(void)
{
	if(!port_timer_HwTimIsRunning(&htim3)){
		memset(&gCurrentPattern, 0, sizeof(patternTable_s));
		gBtnState = BTN_WAIT_FOR_PRESS;
		printf("Start timer\r\n");
		port_timer_HwTimStartIT(&htim3);
	}
}

static inline press_e PressIdentifier(uint32_t timeMs)
{
	uint32_t elapsedTimeMs = port_timer_GetElapsedTimeMs(timeMs);
	printf("elapsed time %lu\r\n", elapsedTimeMs);
	if((elapsedTimeMs >= SERVICE_BTN_SHORTPRESS_MS) && (elapsedTimeMs <= (SERVICE_BTN_SHORTPRESS_MS + SERVICE_BTN_SHORTPRESS_OFFSET_MS))){
		printf("SHORT_PRESS\r\n");
		return SHORT_PRESS;
	}
	else if((elapsedTimeMs >= SERVICE_BTN_LONGPRESS_MS) && (elapsedTimeMs <= (SERVICE_BTN_LONGPRESS_MS + SERVICE_BTN_LONGPRESS_OFFSET_MS))){
		return LONG_PRESS;
		printf("LONG_PRESS\r\n");
	}else{
		printf("INVALID_PRESS\r\n");
		return INVALID_PRESS;
	}
}

static service_btn_pattern_t PatternValidator(patternTable_s *pPattern)
{
	uint8_t match = 0;
	for (uint8_t i = 0; i < SERVICE_BTN_PATTERN_MAX; i++) {
		if (pPattern->cnt != gPatRegTable[i].cnt)
			continue;
		match = 1;
		for (uint8_t j = 0; j < pPattern->cnt; j++) {
			if (pPattern->pressArr[j] != gPatRegTable[i].pressArr[j]) {
				match = 0;
				break;
			}
		}
		if (match){
			return gPatRegTable[i].pattern;
		}
	}
	return SERVICE_BTN_PATTERN_MAX; // Indicating no match
}

void port_timer_HwCallback(port_timer_hwInstance_e instance)
{
	if(instance != PORT_TIMER_INST3){
		return;
	}
	static uint32_t millisNow = 0;
	port_gpio_state_e pinState = port_gpio_ReadPin(SERVICE_BTN_PORT, SERVICE_BTN_PIN);
	switch(gBtnState){
		case BTN_WAIT_FOR_PRESS:{
			if(PORT_GPIO_STATE_SET == pinState){
				millisNow = port_timer_GetMillis();
				gBtnState = BTN_WAIT_FOR_RELEASE;
			}else{
				if(port_timer_GetElapsedTimeMs(millisNow) > SERVICE_BTN_WAIT_FOR_PRESS_TIMEOUT_MS){
					gBtnState = BTN_COMPLETE;
				}
			}
		}break;
		case BTN_WAIT_FOR_RELEASE:{
			if(PORT_GPIO_STATE_RESET == pinState){
					uint8_t idx = gCurrentPattern.cnt;
				if(gCurrentPattern.cnt == SERVICE_BTN_MAX_PRESS){
					gCurrentPattern.cnt = 0;
					gCurrentPattern.pressArr[idx] = INVALID_PRESS;	//replace with INVALID_PRESS in case of overflow
				}else{
					gCurrentPattern.pressArr[idx] = PressIdentifier(millisNow);
				}
				gCurrentPattern.cnt++;
				millisNow = port_timer_GetMillis();		//update
				gBtnState = BTN_WAIT_FOR_PRESS;
			}else{
				if(port_timer_GetElapsedTimeMs(millisNow) > SERVICE_BTN_RELEASE_TIMEOUT_MS){
					uint8_t idx = gCurrentPattern.cnt;
					if(gCurrentPattern.cnt == SERVICE_BTN_MAX_PRESS){
						gCurrentPattern.cnt = 0;
						gCurrentPattern.pressArr[idx] = INVALID_PRESS;	//replace with INVALID_PRESS in case of overflow
					}else{
						gCurrentPattern.pressArr[idx] = LONG_PRESS;
						++gCurrentPattern.cnt;
					}
					gBtnState = BTN_COMPLETE;
				}
			}
		}break;
		case BTN_COMPLETE:{
			service_btn_pattern_t pattern = PatternValidator(&gCurrentPattern);
			service_btn_EventCb(pattern);
			port_timer_HwTimStopIT(&htim3);
		}break;
	}
}

void service_btn_Init(void)
{
	do{
		if(PORT_GPIO_FN_STATUS_OK != port_gpio_Init(SERVICE_BTN_PORT, SERVICE_BTN_PIN, PORT_GPIO_MODE_INPUT_IRQ_RISE, PORT_GPIO_PULL_DOWN)){
			printf("[%s] GPIO Initialization Failed\r\n", __func__);
			break;
		}

		uint16_t psc = 0, arr = 0;
		if(!port_timer_CalcPscArr(16, SERVICE_BTN_DEBOUNCE_MS, &psc, &arr)){
			printf("[%s]Failed to calculate PSC/ARR\r\n", __func__);
			break;
		}

		if(PORT_TIMER_FN_STATUS_OK != port_timer_HwTimInit(&htim3, PORT_TIMER_INST3, SERVICE_BTN_DEBOUNCE_MS, psc, arr)){
			printf("[%s] Button timer Initialization Failed\r\n", __func__);
			break;
		}

		port_gpio_EnableIrq(PORT_GPIO_INTERRUPT_RISE_EDGE);
		printf("Button service initialized\r\n");
	}while(0);
}

void service_btn_Deinit(void)
{
	port_gpio_DeInit(PORT_GPIO_PORT_A, PORT_GPIO_PIN_6);
	port_timer_DeInit(gBtnTmrHnd);
	printf("Button De-Init OK\r\n");
}
