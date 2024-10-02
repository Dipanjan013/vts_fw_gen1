/*
 * atc.h
 *
 *  Created on: Sep 10, 2024
 *      Author: Dipan
 */

#ifndef ATC_ATC_H_
#define ATC_ATC_H_

#include <stdint.h>

#define ATC_MAX_BUFF_SIZE 512

typedef enum atc_tagFnStatus{
	ATC_FN_STATUS_TIMEOUT = -3,
	ATC_FN_STATUS_BUSY = -2,
	ATC_FN_STATUS_FAIL = -1,
	ATC_FN_STATUS_OK = 0,
}atc_fnStatus_t;

typedef enum atc_tag_module{
	ATC_GPS_MODULE,
	ATC_LTE_MODULE,
}atc_module_t;

typedef struct atc_tagLookUpTbl{
	const char *cmd;
	const char *response;
}atc_lookUpTbl_t;

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

int8_t atc_Init(void);
void atc_InitCmdRespTable(void);

atc_fnStatus_t atc_Test(atc_module_t module, atc_cmd_t cmd);
atc_fnStatus_t atc_Read(atc_module_t module, atc_cmd_t cmd, uint8_t *readBuff, uint16_t *size);	//size using as in-out ; in = sizeof of readbuff argument supplied, out = bytes read
atc_fnStatus_t atc_Set(atc_module_t module, atc_cmd_t cmd, uint8_t *writeBuff, uint16_t writeLen);
atc_fnStatus_t atc_Execute(atc_module_t module, atc_cmd_t cmd);

int8_t atc_DeInit(void);

#endif /* ATC_ATC_H_ */
