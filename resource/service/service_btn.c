#include "service_btn.h"
#include "port_timer.h"
#include <string.h>
#include <stdio.h>

typedef enum{
	BTN_WAIT_FOR_PRESS,
	BTN_WAIT_FOR_RELEASE,
	BTN_COMPLETE,
}btnState_e;

typedef struct{
	service_btn_pattern_t pattern;
  press_e pressArr[SERVICE_BTN_MAX_PRESS];
  uint8_t cnt;
}patternTable_s;

static void BtnTmrCb(void *arg);

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

static volatile uint8_t tempFlag = 0;
void port_gpio_Callback(port_gpio_pin_e pin)
{
	if(pin == SERVICE_BTN_PIN){
		tempFlag = 1;
		if(!port_timer_IsRunning(gBtnTmrHnd)){
			memset(&gCurrentPattern, 0, sizeof(patternTable_s));
			gBtnState = BTN_WAIT_FOR_PRESS;
			printf("Start timer\r\n");
			port_timer_StartPeriodic(gBtnTmrHnd, SERVICE_BTN_DEBOUNCE_MS);
		}
	}
}

static inline press_e PressIdentifier(uint32_t timeMs)
{
	if((port_timer_GetElapsedTimeMs(timeMs) >= SERVICE_BTN_SHORTPRESS_MS) && (port_timer_GetElapsedTimeMs(timeMs) <= (SERVICE_BTN_SHORTPRESS_MS + SERVICE_BTN_OFFSET_MS))){
		return SHORT_PRESS;
	}
	else if((port_timer_GetElapsedTimeMs(timeMs) >= SERVICE_BTN_LONGPRESS_MS) && (port_timer_GetElapsedTimeMs(timeMs) <= (SERVICE_BTN_LONGPRESS_MS + SERVICE_BTN_OFFSET_MS))){
		return LONG_PRESS;
	}else{
		return INVALID_PRESS;
	}
}

static service_btn_pattern_t PatternValidator(patternTable_s *pPattern)
{
	for (uint8_t i = 0; i < SERVICE_BTN_PATTERN_MAX; i++) {
		if (pPattern->cnt != gPatRegTable[i].cnt)
			continue;

		uint8_t match = 1;
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

static void BtnTmrCb(void *arg)
{
	(void)(arg);
	static uint32_t millisNow = 0;
	printf("CB\r\n");
	switch(gBtnState){
		case BTN_WAIT_FOR_PRESS:{
			if(PORT_GPIO_STATE_SET == port_gpio_ReadPin(SERVICE_BTN_PORT, SERVICE_BTN_PIN)){
				printf("Pressed\r\n");
				millisNow = osKernelGetTickCount();
				gBtnState = BTN_WAIT_FOR_RELEASE;
			}else{
				if(port_timer_GetElapsedTimeMs(millisNow) > SERVICE_BTN_WAIT_FOR_PRESS_TIMEOUT_MS){
					gBtnState = BTN_COMPLETE;
				}
			}
		}break;
		case BTN_WAIT_FOR_RELEASE:{
			if(PORT_GPIO_STATE_RESET == port_gpio_ReadPin(SERVICE_BTN_PORT, SERVICE_BTN_PIN)){
				printf("released\r\n");
				gCurrentPattern.pressArr[gCurrentPattern.cnt] = PressIdentifier(millisNow);
				gCurrentPattern.cnt++;
				if(gCurrentPattern.cnt == SERVICE_BTN_MAX_PRESS){
					gCurrentPattern.cnt = 0;
					gCurrentPattern.pressArr[gCurrentPattern.cnt] = INVALID_PRESS;	//replace with INVALID_PRESS in case of overflow
				}
				gBtnState = BTN_WAIT_FOR_PRESS;
			}else{
				if(port_timer_GetElapsedTimeMs(millisNow) > SERVICE_BTN_RELEASE_TIMEOUT_MS){
					gBtnState = BTN_COMPLETE;
				}
			}
		}break;
		case BTN_COMPLETE:{
			service_btn_pattern_t pattern = PatternValidator(&gCurrentPattern);
			service_btn_EventCb(pattern);
			port_timer_Stop(gBtnTmrHnd);
		}break;
	}
}

void service_btn_Init(void)
{
	do{
		port_gpio_fnStatus_e ret;
		ret = port_gpio_Init(SERVICE_BTN_PORT, SERVICE_BTN_PIN, PORT_GPIO_MODE_INPUT_IRQ_RISE, PORT_GPIO_PULL_DOWN);
		if(ret != PORT_GPIO_FN_STATUS_OK){
			printf("[%s] Init Failed\r\n", __func__);
			break;
		}

		if(PORT_TIMER_FN_STATUS_OK != port_timer_InitPeriodic(&gBtnTmrHnd, BtnTmrCb)){
			printf("[%s] Button timer Init Failed\r\n", __func__);
			break;
		}
		port_gpio_EnableIrq(PORT_GPIO_INTERRUPT_RISE_EDGE);
		printf("Button Init OK\r\n");
//
//		int rc = port_timer_InitPeriodic(&tHndl, TestTimer);
//		printf("timer init status %d\r\n", rc);
//
//		rc = port_timer_IsRunning(tHndl);
//		printf("Is running status %d\r\n", rc);
//
//		rc = port_timer_StartPeriodic(tHndl, 1000);
//		printf("Start status %d\r\n", rc);
//
//		rc = port_timer_IsRunning(tHndl);
//		printf("Is running status %d\r\n", rc);
//
//		while(1){
//			if(tempFlag){
//				tempFlag = 0;
//				printf("IRQ recvd\r\n");
//				printf("count %d\r\n", cnt);
//			}
//			osDelay(1);
//		}
	}while(0);
}

void service_btn_Deinit(void)
{
	port_gpio_DeInit(PORT_GPIO_PORT_A, PORT_GPIO_PIN_6);
	port_timer_DeInit(gBtnTmrHnd);
	printf("Button De-Init OK\r\n");
}
