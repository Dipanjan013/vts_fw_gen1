/*
 * app.c
 *
 *  Created on: Sep 1, 2024
 *      Author: Dipan
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cmsis_os2.h>

#include "app.h"
#include "atc.h"

/*! Macros */

/*! Private function*/

/*! Variable definition */
osThreadId_t mainTaskHandle;

const osThreadAttr_t mainTaskAttr = {
  .name = "mainTask",
  .stack_size = 1024*3,
  .priority = (osPriority_t)osPriorityLow,
};

/************************************************************************************************************************************************
*************************************************************************************************************************************************
*************************************************************************************************************************************************/

static void MyTask(void *arg)
{
	(void)(arg);
	atc_fnStatus_t ret;
  printf("In main task\r\n");

  ret = atc_Init();
  printf("ATC init status %d\r\n", ret);
  atc_InitCmdRespTable();

  while(1){
  	ret = atc_Test(ATC_LTE_MODULE, ATC_TEST);
  	printf("ATC TEST status %d\r\n", ret);

  	osDelay(1000);
  }

}


void app_main(void)
{
	printf("\r\nVTS GEN 1\r\n");
	printf("Build Date %s\r\n", __DATE__);
	printf("Build Time %s\r\n", __TIME__);

  /* Init scheduler */
  osKernelInitialize();

  mainTaskHandle = osThreadNew(MyTask, NULL, &mainTaskAttr);

  printf("m here\r\n");

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1);
}
