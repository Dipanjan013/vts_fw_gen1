/*
 * atc_cmd.h
 *
 *  Created on: Dec 25, 2024
 *      Author: Dipan
 */

#ifndef ATC_ATC_CMD_H_
#define ATC_ATC_CMD_H_

#define CRLF "\r\n"

/****************************************************************
 * 											Solicited command
 * *************************************************************/
//Basic command
#define ATC_TEST_CMD "AT"CRLF
#define ATC_IMSI_CMD "AT+CIMI"CRLF
#define ATC_ICCID_CMD "AT+ICCID"CRLF
#define ATC_MODEM_INFO_CMD "AT+CGMM"CRLF

//Network command
#define ATC_NW_REG_STATUS_CMD "AT+CREG?"CRLF
#define ATC_NW_RSSI_CHECK_CMD "AT+CSQ"CRLF
#define ATC_NW_OP_NAME_CMD "AT+COPS?"CRLF
#define ATC_NW_DEREG_CMD "AT+CGATT=0"CRLF
#define ATC_NW_REREG_CMD "AT+CGATT=1"CRLF
#define ATC_NW_SCAN_AVAIL_CMD "AT+COPS=?"CRLF

//MQTT commands
#define ATC_MQTT_CREATE_CMD "AT+MQTTCREATE"CRLF
#define ATC_MQTT_CONNECT_CMD "AT+MQTTCONNECT"CRLF
#define ATC_MQTT_DISCONNECT_CMD "AT+MQTTDISCONNECT"CRLF
#define ATC_MQTT_PUBLISH_CMD "AT+MQTTPUBLISH"CRLF
#define ATC_MQTT_SUBSCRIBE_CMD "AT+MQTTSUBSCRIBE"CRLF

//SIM command
#define ATC_SIM_CHANNEL_CHECK_CMD "AT^SIMSWAP"CRLF
#define ATC_SIM_SWAP_ESIM_CMD "AT+SIMSWAP=0"CRLF
#define ATC_SIM_SWAP_EXT_SIM_CMD "AT+SIMSWAP=1"CRLF

//Modem specific command
#define ATC_CAVLI_HUBBLE_REG_STATUS_CMD "AT+HUBBLEREG?"CRLF
#define ATC_CAVLI_REBOOT_CMD ""CRLF

#define ATC_MAX_CMD ""CRLF
/**************************************************************
 * 									End of Solicited command
 * *************************************************************/


/**************************************************************
 * 											Unsolicited command
 * *************************************************************/
#define ATC_RESPONSE_OK_CMD "OK"
#define ATC_RESPONSE_ERROR_CMD "ERROR"
/**************************************************************
 * 											End of Unsolicited command
 * *************************************************************/

/**!
 * @brief Contains list of all possible commands that can be send/receive from/to TE
 * @note refer source file for command string
 */
typedef enum atc_tagCmd{
	//basic commands
	ATC_TEST = 0,
	ATC_IMSI,
	ATC_ICCID,
	ATC_MODEM_INFO,

	//network commands
	ATC_NW_REG_STATUS,
	ATC_NW_RSSI_CHECK,
	ATC_NW_OP_NAME,
	ATC_NW_DEREG,
	ATC_NW_REREG,
	ATC_NW_SCAN_AVAIL,

	//SMS related commands

	//call related commands

	//MQTT commands
	ATC_MQTT_CREATE,
	ATC_MQTT_CONNECT,
	ATC_MQTT_DISCONNECT,
	ATC_MQTT_PUBLISH,
	ATC_MQTT_SUBSCRIBE,

	//HTTP commands

	//GPS commands

	//Sleep mode related  commands

	//SIM card related commands
	ATC_SIM_CHANNEL_CHECK,
	ATC_SIM_SWAP_ESIM,
	ATC_SIM_SWAP_EXT_SIM,

	//modem specific commands
	ATC_CAVLI_HUBBLE_REG_STATUS,
	ATC_CAVLI_REBOOT,

	//user-defined commands
	ATC_MAX
}atc_cmd_t;

/*!
 * @brief All possible response to be considered
 * @note refer source file for the response string
 */
typedef enum atc_tagResponse{
	ATC_RESP_OK = 0,
	ATC_RESP_ERROR,
	ATC_RESP_MAX
}atc_response_t;

typedef struct{
	atc_cmd_t cmdNum;
	char *cmdStr;
}atc_cmdTable_t;

typedef struct{
	atc_response_t respNum;
	char *respStr;
}atc_respTable_t;

uint8_t atc_cmd_SearchDesiredResp(char *sourceStr, atc_response_t desiredResp);
atc_response_t atc_cmd_SearchResponse(char *sourceStr);
char* atc_cmd_GetCmdStr(atc_cmd_t desiredCmd);

#endif /* ATC_ATC_CMD_H_ */
