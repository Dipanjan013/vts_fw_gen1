/*
 * atc_cmd.c
 *
 *  Created on: Dec 25, 2024
 *      Author: Dipan
 */
#include "atc_cmd.h"

static atc_cmdTable_t atc_cmdTable[] = {
	// Basic commands
	{ATC_TEST, ATC_TEST_CMD},
	{ATC_IMSI, ATC_IMSI_CMD},
	{ATC_ICCID, ATC_ICCID_CMD},
	{ATC_MODEM_INFO, ATC_MODEM_INFO_CMD},

	// Network commands
	{ATC_NW_REG_STATUS, ATC_NW_REG_STATUS_CMD},
	{ATC_NW_RSSI_CHECK, ATC_NW_RSSI_CHECK_CMD},
	{ATC_NW_OP_NAME, ATC_NW_OP_NAME_CMD},
	{ATC_NW_DEREG, ATC_NW_DEREG_CMD},
	{ATC_NW_REREG, ATC_NW_REREG_CMD},
	{ATC_NW_SCAN_AVAIL, ATC_NW_SCAN_AVAIL_CMD},

	// MQTT commands
	{ATC_MQTT_CREATE, ATC_MQTT_CREATE_CMD},
	{ATC_MQTT_CONNECT, ATC_MQTT_CONNECT_CMD},
	{ATC_MQTT_DISCONNECT, ATC_MQTT_DISCONNECT_CMD},
	{ATC_MQTT_PUBLISH, ATC_MQTT_PUBLISH_CMD},
	{ATC_MQTT_SUBSCRIBE, ATC_MQTT_SUBSCRIBE_CMD},

	// SIM card related commands
	{ATC_SIM_CHANNEL_CHECK, ATC_SIM_CHANNEL_CHECK_CMD},
	{ATC_SIM_SWAP_ESIM, ATC_SIM_SWAP_ESIM_CMD},
	{ATC_SIM_SWAP_EXT_SIM, ATC_SIM_SWAP_EXT_SIM_CMD},

	// Modem specific commands
	{ATC_CAVLI_HUBBLE_REG_STATUS, ATC_CAVLI_HUBBLE_REG_STATUS_CMD},
	{ATC_CAVLI_REBOOT, ATC_CAVLI_REBOOT_CMD},

	// Max command (Used as a boundary or error condition)
	{ATC_MAX, ATC_MAX_CMD}
};

static atc_respTable_t atc_RespTable[] = {
	{OK, "OK"},
	{ERROR, "ERROR"},
};

uint8_t atc_cmd_SearchDesiredResp(char *sourceStr, atc_response_t desiredResp)
{
	for(atc_response_t i = ATC_RESP_OK; i < ATC_RESP_MAX; i++){
		if(NULL != strstr(atc_RespTable.respStr, sourceStr)){
			return 1;
		}
	}
	return 0;
}

atc_response_t atc_cmd_SearchResponse(char *sourceStr)
{
	for(atc_response_t i = ATC_RESP_OK; i < ATC_RESP_MAX; i++){
		if(NULL != strstr(atc_RespTable.respStr, sourceStr)){
			return i;
		}
	}
	return ATC_RESP_MAX;
}

char* atc_cmd_GetCmdStr(atc_cmd_t desiredCmd)
{
	if(desiredCmd >= ATC_TEST && desiredCmd < ATC_MAX){
		return atc_cmdTable.cmdStr;
	}else{
		return NULL;
	}
}
