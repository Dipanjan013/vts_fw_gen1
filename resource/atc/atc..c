/*
 * atc..c
 *
 *  Created on: Sep 10, 2024
 *      Author: Dipan
 */

/*! Includes */
#include <stdio.h>
#include <string.h>

#include "atc.h"
#include "port_uart.h"
#include <cmsis_os2.h>

/*! Macros */
#define EOS "\r\n\0"	//end of string
#define DEF_TIMEOUT_MS (uint16_t)10000

/*! Private variables */

/*! Function prototypes */
static port_uart_fnStatus_t SendCmd(port_uart_handle_t *handle, uint8_t *txBuff, uint16_t len, uint16_t timeoutMs);
static port_uart_fnStatus_t RecvCmd(port_uart_handle_t *handle, uint8_t *rxBuff, uint16_t buffSize, uint16_t timeoutMs);

/*! User-defined  data types */
typedef struct{
	port_uart_callback_t cbType;
	uint8_t ongoing;
}uartInfo_t;

#define VERIFY_AND_RETURN(x, y, s, d) \
do{ \
		if(x != 0)  \
		{						\
			printf("[%s %d]err return %d\r\n", s, d, x); \
			return y;   \
		}						\
}while(0);

/** COMMAND RESPONSE TABLE****/

/**!
 * @note
 * "=?" means whether command is supported or not
 * "=" means command is given to set some values
 * ex : "AT+CGMI" without any operator or '?' operator means to return some information
 */

/*! Variable definition */
port_uart_handle_t gsmHandle;
__attribute__((unused)) port_uart_handle_t gpsHandle;

static atc_lookUpTbl_t lookUpTbl[ATC_MAX] = {0};
static uartInfo_t gsmUartInfo = {0};

/*!************************************************************************************************************************
 *
 *************************************************************************************************************************/

static inline void ClrFlags(port_uart_handle_t *handle)
{
	if(handle == &gsmHandle){
		memset(&gsmUartInfo, 0, sizeof(uartInfo_t));
	}
}

void port_uart_Callback(port_uart_callback_t cbType, port_uart_handle_t *handle)
{
	if(handle == &gsmHandle){
		gsmUartInfo.cbType = cbType;
	}
}

void atc_InitCmdRespTable(void)
{
	memset(&lookUpTbl, 0, sizeof(lookUpTbl));
	/* Basic commands */
	lookUpTbl[ATC_TEST] = (atc_lookUpTbl_t){"AT"EOS, "OK"};
	lookUpTbl[ATC_IMSI] = (atc_lookUpTbl_t){"AT+CIMI"EOS, "OK"};
	lookUpTbl[ATC_ICCID] = (atc_lookUpTbl_t){"AT"EOS, "+ICCID:"};
	lookUpTbl[ATC_MODEM_INFO] = (atc_lookUpTbl_t){"ATI"EOS, "OK"};

	/* Network commands */
	lookUpTbl[ATC_NW_REG_STATUS] = (atc_lookUpTbl_t){"AT+CREG?"EOS, "+CREG:"};
	lookUpTbl[ATC_NW_RSSI_CHECK] = (atc_lookUpTbl_t){"AT+CSQ"EOS, "+CSQ:"};
	lookUpTbl[ATC_NW_OP_NAME] = (atc_lookUpTbl_t){"AT+COPS?"EOS, "+COPS:"};
	lookUpTbl[ATC_NW_DEREG] = (atc_lookUpTbl_t){"AT+CGATT=0"EOS, "OK"};
	lookUpTbl[ATC_NW_REREG] = (atc_lookUpTbl_t){"AT+CGATT=1"EOS, "OK"};
	lookUpTbl[ATC_NW_SCAN_AVAIL] = (atc_lookUpTbl_t){"AT+COPS=?"EOS, "+COPS:"};

	/* SMS related commands */

	/* MQTT related commands */

	/* HTTP related commands */

	/* GPS related command */

	/* Sleep mode related command */

	/* SIM card related command */
	lookUpTbl[ATC_SIM_CHANNEL_CHECK] = (atc_lookUpTbl_t){"AT^SIMSWAP?"EOS, "^SIMSWAP:"};
	lookUpTbl[ATC_SIM_SWAP_ESIM] = (atc_lookUpTbl_t){"AT^SIMSWAP=0"EOS, "OK"};
	lookUpTbl[ATC_SIM_SWAP_EXT_SIM] = (atc_lookUpTbl_t){"AT^SIMSWAP=1"EOS, "OK"};

	/* Modem specific command */
	lookUpTbl[ATC_CAVLI_HUBBLE_REG_STATUS] = (atc_lookUpTbl_t){"AT+HUBBLEREG?"EOS, "+HUBBLEREG:"};
	lookUpTbl[ATC_CAVLI_REBOOT] = (atc_lookUpTbl_t){"AT+TRB?"EOS, "OK"};

	/* User defined command*/

}

static port_uart_fnStatus_t SendCmd(port_uart_handle_t *handle, uint8_t *txBuff, uint16_t len, uint16_t timeoutMs)
{
	ClrFlags(handle);
	port_uart_fnStatus_t ret = PORT_UART_FN_STATUS_ERR;
	ret = port_uart_Transmit(handle, txBuff, len);
	if(ret != 0){
		return ret;
	}

	uint32_t t0 = HAL_GetTick();
	if(handle == &gsmHandle){
		while(((HAL_GetTick() - t0) < timeoutMs )&& (gsmUartInfo.cbType == PORT_UART_CB_NONE));
		if(gsmUartInfo.cbType == PORT_UART_TX_CMPLT){
			ret = PORT_UART_FN_STATUS_OK;
		}else if(gsmUartInfo.cbType == PORT_UART_XFER_ERR){
			ret = PORT_UART_FN_STATUS_ERR;
		}else{
			ret = PORT_UART_FN_STATUS_BUSY;
		}
	}
	return ret;
}

static port_uart_fnStatus_t RecvCmd(port_uart_handle_t *handle, uint8_t *rxBuff, uint16_t buffSize, uint16_t timeoutMs)
{
	ClrFlags(handle);
	port_uart_fnStatus_t ret = PORT_UART_FN_STATUS_ERR;
	ret = port_uart_Receive(handle, rxBuff, buffSize);
	if(ret != 0){
		return ret;
	}

	uint32_t t0 = HAL_GetTick();
	if(handle == &gsmHandle){
		while(((HAL_GetTick() - t0) < timeoutMs) && (gsmUartInfo.cbType == PORT_UART_CB_NONE));
		if(gsmUartInfo.cbType == PORT_UART_RX_CMPLT){
			ret = PORT_UART_FN_STATUS_OK;
		}else if(gsmUartInfo.cbType == PORT_UART_XFER_ERR){
			ret = PORT_UART_FN_STATUS_ERR;
		}else{
			ret = PORT_UART_FN_STATUS_BUSY;
		}
	}
	return ret;
}

int8_t atc_Init(void)
{
	gsmHandle.Instance = USART1;
	port_uart_fnStatus_t ret = port_uart_Init(&gsmHandle);
	VERIFY_AND_RETURN(ret, -1, __func__, __LINE__);
	//TODO same for GPS uart

	memset(&gsmUartInfo, 0, sizeof(uartInfo_t));
	//TODO same for GPS uart
	return 0;
}

atc_fnStatus_t atc_Test(atc_module_t module, atc_cmd_t cmd)
{
	port_uart_fnStatus_t ret = PORT_UART_FN_STATUS_ERR;
	if(ATC_LTE_MODULE == module){
		ret = SendCmd(&gsmHandle, (uint8_t*)lookUpTbl[ATC_TEST].cmd, strlen(lookUpTbl[ATC_TEST].cmd), DEF_TIMEOUT_MS);
		VERIFY_AND_RETURN(ret, ATC_FN_STATUS_FAIL, __func__, __LINE__);
		uint8_t rxBuff[ATC_MAX_BUFF_SIZE+1];
		memset(rxBuff, 0, sizeof(rxBuff));
		ret = RecvCmd(&gsmHandle, rxBuff, strlen(lookUpTbl[ATC_TEST].response), DEF_TIMEOUT_MS);
		VERIFY_AND_RETURN(ret, ATC_FN_STATUS_FAIL, __func__, __LINE__);
		if(strncmp((char*)rxBuff, lookUpTbl[ATC_TEST].response, strlen(lookUpTbl[ATC_TEST].response)) != 0){
			return ATC_FN_STATUS_FAIL;
		}
	}
	return ATC_FN_STATUS_OK;
}

void MyTest(void)
{
	ClrFlags(&gsmHandle);
	int ret = 0;
	ret = port_uart_Transmit(&gsmHandle, "AT\r\n", strlen("AT\r\n"));
	printf("tx ret = %d\r\n", ret);
	if(ret != 0)
		return;

	ClrFlags(&gsmHandle);
	uint8_t rxBuff[100];
	memset(rxBuff, 0, sizeof(rxBuff));
	ret = port_uart_Receive(&gsmHandle, rxBuff, 5);
	uint16_t t0 = 0;
	while(gsmUartInfo.cbType == PORT_UART_CB_NONE && (++t0 < 5000)){
		HAL_Delay(1);
	}
	printf("cb = %d\r\n", gsmUartInfo.cbType);
	printf("Data %s\r\n", rxBuff);
}

//atc_fnStatus_t atc_Read(atc_module_t module, atc_cmd_t cmd, uint8_t *readBuff, uint16_t *size)
//{
//
//}
//
//atc_fnStatus_t atc_Set(atc_module_t module, atc_cmd_t cmd, uint8_t *writeBuff, uint16_t writeLen)
//{
//
//}
//
//atc_fnStatus_t atc_Execute(atc_module_t module, atc_cmd_t cmd)
//{
//
//}

int8_t atc_DeInit(void)
{
	port_uart_fnStatus_t ret = port_uart_Init(&gsmHandle);
	VERIFY_AND_RETURN(ret, -1, __func__, __LINE__);

	return 0;
}
