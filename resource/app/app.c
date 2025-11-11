//includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsis_os2.h"

#include "app.h"
#include "port_i2c.h"
#include "service_log.h"
#include "utils_gps.h"

//Macros
#define MSG_POST_TIMEOUT_MS 100U
#define SEC_TO_MS (1000)
#define MINS_TO_MS (60 * SEC_TO_MS)

//User-defined data types
typedef enum{
	APP_STATE_STATUS_HANDLED,
	APP_STATE_STATUS_TRANS,
	APP_STATE_STATUS_IGNORED,
}app_stateStatus_e;

typedef enum{
	APP_EVENT_IDLE,
	APP_EVENT_INIT,
	APP_EVENT_FETCH_GPS,
	APP_EVENT_CONNECT_CELLULAR,
	APP_EVENT_CONNECT_MQTT,
	APP_EVENT_PUB_DATA,
	APP_EVENT_SLEEP,
	APP_EVENT_MAX
}app_event_e;

typedef enum{
	APP_RESERVED_EVENT_ENTRY = APP_EVENT_MAX,
	APP_RESERVED_EVENT_EXIT
}app_reservedEvent_e;

typedef struct app_tagStateInstance app_stateInst_s;
typedef app_stateStatus_e (*StateHandler_t)(app_eventParam_s *pParam, app_stateInst_s *pInst);

struct app_tagStateInstance{
	StateHandler_t activeState;
	StateHandler_t nextState;
};

//Function prototypes
//State Handlers
static app_stateStatus_e AppStatePreOp(app_eventParam_s *pParam, app_stateInst_s *pInst);
static app_stateStatus_e AppStateSampling(app_eventParam_s *pParam, app_stateInst_s *pInst);
static app_stateStatus_e AppStatePublish(app_eventParam_s *pParam, app_stateInst_s *pInst);
static app_stateStatus_e AppStateIdle(app_eventParam_s *pParam, app_stateInst_s *pInst);

static void SamplingTmrCb(void *arg);
static void ReportingTmrCb(void *arg);

/*!
 * @fn App dispatcher task that handle events
 */
static void AppDisPatcher(void);

//Global Variables
static app_stateInst_s gAppStateInstance = {0};
static osMessageQueueId_t gQueueHndl = NULL;
static port_timer_hndle_t gSamplingTmrHnd = NULL;
static port_timer_hndle_t gReportingTmrHnd = NULL;

static volatile uint32_t gAppFlags = 0;
/*!**************************************************************************************************************************
 *
 ***************************************************************************************************************************/

static inline void AppSetFlag(appFlagBits_e bit)
{
	gAppFlags |= bit;
}

static inline void AppClearFlag(appFlagBits_e bit)
{
	if(gAppFlags & bit){
		gAppFlags = ~bit;
	}
}

uint8_t app_flagGet(appFlagBits_e bit)
{
	return (gAppFlags & bit);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == ADXL_INT1_Pin){
		AppSetFlag(APP_FLAG_BIT_ADXL_TAP);
	}
  if(GPIO_Pin == BUTTON_Pin){
		service_btn_IrqCb();
  }
}

static void SamplingTmrCb(void *arg)
{
	(void)(arg);
}

static void ReportingTmrCb(void *arg)
{
	(void)(arg);
}

void service_at_UnsolRespCallback(service_at_unsolResp_t type, uint8_t *buff, uint16_t len)
{
	(void)(buff);
	(void)(len);
	LOG_I("[%s] Type : %d\r\n", __func__, type);
}

void AppPostEvent(app_eventParam_s *pParam)
{
	osStatus_t osStatus = osError;
	osStatus = osMessageQueuePut(gQueueHndl, pParam, 0, MSG_POST_TIMEOUT_MS);
	if(osOK != osStatus){
		LOG_E("[%s] Failed (%d)\r\n", __func__, osStatus);
	}
}

void AppPostEventFromIsr(app_eventParam_s *pParam)
{
	osStatus_t osStatus = osError;
	osStatus = osMessageQueuePut(gQueueHndl, pParam, 0, 0);
	if(osOK != osStatus){
		// do something. Should be interrupt safe
	}
}

void service_btn_EventCb(service_btn_pattern_t pattern)
{
	switch (pattern) {
		case SERVICE_BTN_PATTERN_ONBOARDING:
			LOG_I("SERVICE_BTN_PATTERN_ONBOARDING\r\n");
			break;
		case SERVICE_BTN_PATTERN_REBOOT:
			LOG_I("SERVICE_BTN_PATTERN_REBOOT\r\n");
			HAL_NVIC_SystemReset();
			break;
		case SERVICE_BTN_PATTERN_CLEARALL:
			LOG_I("SERVICE_BTN_PATTERN_CLEARALL\r\n");
			break;
		case SERVICE_BTN_PATTERN_TESTMODE:
			LOG_I("SERVICE_BTN_PATTERN_TESTMODE\r\n");
			break;
		case SERVICE_BTN_PATTERN_MAX:
			LOG_W("UNDEFINED BUTTON\r\n");
		default:
			break;
	}
}


static app_stateStatus_e AppStatePreOp(app_eventParam_s *pParam, app_stateInst_s *pInst)
{
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	app_eventParam_s eventParam = {0};
	uint8_t readData[512];
	int rc = 0;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:
			break;
		case APP_EVENT_INIT:{
			LOG_I("[%s] Init\r\n", __func__);
			do{
				//Initialize the button service
				service_btn_Init();

				//Check cellular communication
				LOG_I("Checking comm.with Cavli\r\n");
				if(!service_at_Test(SERVICE_AT_UART_INST0)){
					LOG_E("Communication failed with Cellular\r\n");
					break;
				}
				//Cellular echo-off
				service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_ECHO_OFF, 1000);
				memset(readData, 0, sizeof(readData));
				rc = service_at_Read(SERVICE_AT_UART_INST0, AT_READ_MFG_INFO, readData, sizeof(readData)-1, 1000);
				if(!rc){
					printf("Failed to read MFG data\r\n");
				}else{
					printf("MFG Data : %s\r\n", (char*)readData);
				}

				//Initialize the timers
				port_timer_InitOneShot(gReportingTmrHnd, ReportingTmrCb);
				port_timer_InitPeriodic(gSamplingTmrHnd, SamplingTmrCb);

				//Initialize the accelerometer
				LOG_I("Initializing the Accelerometer\r\n");
				port_i2c_Scan(ADXL_I2C_HNDL);
				uint8_t devID = 0;
				devID = drv_adxl_GetDevId();
				LOG_I("ADXL DEVICE ID : 0x%2X\r\n", devID);
				drv_adxl_Init(DRV_ADXL_MODE_STREAM);
				float x = 0, y = 0, z = 0;
				for(uint8_t i = 0; i < 3; i++){
					drv_adxl_ReadAxesXYZ(&x, &y, &z);
					LOG_I("x = %.2f, y = %.2f, z = %.2f\r\n", x,y,z);
					osDelay(100);
				}
				LOG_I("Enabling crash detection\r\n");
				drv_adxl_Init(DRV_ADXL_MODE_TAP_DETECT_SINGLE);
				rc = 1;	//all succeeded
			}while(0);

			if(rc == 1){
				LOG_I("System initialization successful. Switching to ---> Normal mode\r\n");
				eventParam.event = APP_EVENT_FETCH_GPS;
				pInst->nextState = AppStateSampling;
			}else{
				LOG_E("Error : Pre-operating state failed\r\n");
				eventParam.event = APP_EVENT_SLEEP;
				pInst->nextState = AppStateIdle;
			}
			AppPostEvent(&eventParam);
			stateStatus = APP_STATE_STATUS_TRANS;
		}break;
		case APP_RESERVED_EVENT_EXIT:
			break;
		default:
			LOG_W("[%s] Unknown event %d\r\n", __func__, pParam->event);
	}
	return stateStatus;
}

static app_stateStatus_e AppStateSampling(app_eventParam_s *pParam, app_stateInst_s *pInst)
{
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	nmea_s nmea = {0};
	int rc = 0;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:
			break;
		case APP_EVENT_FETCH_GPS:
			LOG_I("[%s] %s\r\n", __func__, "Event GPS");
			//Turn on GPS
			service_at_Execute(SERVICE_AT_UART_INST0, AT_EXE_GPS_ON, 1000);
			//Start reading GPS data
			for(uint8_t i = 0; i < 20; i++){
				rc = service_at_Read(SERVICE_AT_UART_INST0, AT_READ_GPS_POS, (uint8_t*)&nmea, sizeof(nmea_s), 1000);
				if(rc){
					break;
				}else{
					osDelay(3000);
				}
			}
			if(rc){
				utils_gps_PrintNmea(&nmea);
				eventParam.event = APP_EVENT_PUB_DATA;
				pInst->nextState = AppStatePublish;
			}else{
				LOG_W("GPS loc failed\r\n");
				eventParam.event = APP_EVENT_PUB_DATA;
				pInst->nextState = AppStateIdle;
			}
			stateStatus = APP_STATE_STATUS_TRANS;
			AppPostEvent(&eventParam);
			break;
		case APP_RESERVED_EVENT_EXIT:
			break;
		default:
			LOG_W("[%s] Unknown event %d\r\n", __func__, pParam->event);
	}
	return stateStatus;
}

static app_stateStatus_e AppStatePublish(app_eventParam_s *pParam, app_stateInst_s *pInst)
{
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:
			break;
		case APP_EVENT_PUB_DATA:
			LOG_I("[%s] %s\r\n", __func__, "Event Pub data");
			while(1){
				if(app_flagGet(APP_FLAG_BIT_ADXL_TAP)){
					LOG_D("ADXL IRQ recvd\r\n");
					AppClearFlag(APP_FLAG_BIT_ADXL_TAP);
				}
				osDelay(500);
			}
			break;
		case APP_RESERVED_EVENT_EXIT:
			break;
		default:
			LOG_W("[%s] Unknown event %d\r\n", __func__, pParam->event);
	}
	return stateStatus;
}

static app_stateStatus_e AppStateIdle(app_eventParam_s *pParam, app_stateInst_s *pInst)
{
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	switch(pParam->event){
		case APP_RESERVED_EVENT_ENTRY:
			break;
		case APP_EVENT_SLEEP:
			LOG_I("[%s] %s\r\n", __func__, "Event Sleep");
			break;
		case APP_RESERVED_EVENT_EXIT:
			break;
		default:
			LOG_W("[%s] Unknown event %d\r\n", __func__, pParam->event);
	}
	return stateStatus;
}

static void AppDisPatcher(void)
{
	app_eventParam_s appEvent;
	app_stateStatus_e stateStatus = APP_STATE_STATUS_HANDLED;
	while(1){
		memset(&appEvent, 0, sizeof(app_eventParam_s));
		if(osOK == osMessageQueueGet(gQueueHndl, &appEvent, 0, osWaitForever)){
			stateStatus = gAppStateInstance.activeState(&appEvent, &gAppStateInstance);
			if(APP_STATE_STATUS_TRANS == stateStatus){													//state change requested
				if(gAppStateInstance.nextState != NULL){													//just to make sure target state is not empty
					appEvent.event = APP_RESERVED_EVENT_EXIT;
					gAppStateInstance.activeState(&appEvent, &gAppStateInstance);		//clear exit the current state
					gAppStateInstance.activeState = gAppStateInstance.nextState;		//state changed to target state
					appEvent.event = APP_RESERVED_EVENT_ENTRY;
					gAppStateInstance.activeState(&appEvent, &gAppStateInstance);		//clear entry the target state
					gAppStateInstance.nextState = NULL;
				}else{
					LOG_W("[%s] State change requested but next state is empty\r\n", __func__);
				}
			}
		}else{
			LOG_E("[%s]Failed to get new message\r\n", __func__);
		}
		osDelay(1);
	}
}

void app_main(void)
{
	LOG_I("\r\n>>BOOT UP\r\n");
	LOG_I("%s\r\nFW Ver : %s\r\nHW Ver : %s\r\n", CONFIG_FW_NAME, CONFIG_FW_VER, CONFIG_HW_VER);
	for(uint8_t i = 0; i < 10; i++){
		port_led_Toggle(PORT_LED_COLOUR_BLUE);
		osDelay(100);
	}
	do{
		gQueueHndl = osMessageQueueNew(5, sizeof(app_eventParam_s), NULL);
		if(!gQueueHndl){
			LOG_E("[%s]Failed to create message queue\r\n", __func__);
			break;
		}
		if(!service_at_Init()){
			LOG_E("AT Initialization failed\r\n");
			break;
		}
		LOG_I("AT Initialized successfully\r\n");

		//Initialize the state machine
		gAppStateInstance.activeState = AppStatePreOp;
		gAppStateInstance.nextState = NULL;
		app_eventParam_s *pParam = calloc(sizeof(app_eventParam_s), 1);
		if(!pParam){
			break;
		}
		pParam->event = APP_EVENT_INIT;
		AppPostEvent(pParam);
		free(pParam);
		AppDisPatcher();
	}while(0);

	//Reaching here indicates error at boot-up
	for(;;){
		LOG_E("System Initialization failed\r\n");
		osDelay(10000);
	}
}
