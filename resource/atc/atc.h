/*
 * atc.h
 *
 *  Created on: Dec 25, 2024
 *      Author: Dipan
 */

#ifndef ATC_ATC_H_
#define ATC_ATC_H_

#include "port_uart.h"

#define ATC_MAX_BUFF_SIZE 512
#define ATC_TX_DATA_MAX_LEN 100
#define ATC_RX_DATA_MAX_LEN 100
#define ATC_MAX_SAVED_RESPONSE 5

/*!
 * @enum : Unsolicited response codes
 */
typedef enum atc_tagUnsolRespCodes{
	ATC_RESP_OK = 0,
	ATC_RESP_ERROR,
	ATC_RESP_CREG,
	ATC_RESP_ICCID,
	ATC_RESP_CSQ,
	ATC_RESP_CGATT,
	ATC_RESP_CGMM,
	ATC_RESP_MAX,
}atc_unsolRespCodes_t;

typedef struct{
	atc_unsolRespCodes_t code;
	char *respStr;
}atc_unsolResp_t;

typedef struct atc_tagRespQueue{
	uint8_t validRespCnt;
	struct{
		atc_unsolRespCodes_t type;
		uint8_t data[ATC_RX_DATA_MAX_LEN + 1];
	}respData[ATC_MAX_SAVED_RESPONSE];
}atc_respQ_t;

typedef enum atc_tagFnStatus{
	ATC_FN_STATUS_ERR_PARAM = -3,
	ATC_FN_STATUS_TIMEOUT = -2,
	ATC_FN_STATUS_FAIL = -1,
	ATC_FN_STATUS_OK = 0,
}atc_fnStatus_t;

typedef struct tagAtcData{
	char txData[ATC_TX_DATA_MAX_LEN + 1];
	char rxData[ATC_RX_DATA_MAX_LEN + 1];
}atc_data_t;


uint8_t atc_Init(port_uart_handle_t *handle);
void atc_StartReceiverTask(void);
/*!
 * @brief : This function is used to send command and save the received data in the provided pAtcData structure
 * It can be called from other layers. It is specially used when the command is not listed from above command list
 */
atc_fnStatus_t atc_SendCommand(port_uart_handle_t *handle, atc_data_t *pAtcData, uint16_t  timeoutMs);

#endif /* ATC_ATC_H_ */
