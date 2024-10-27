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

#define ATC_IMSI_MAX_LEN 15
#define ATC_ICCID_MAX_LEN 22
#define ATC_MODEM_INFO_MAX_LEN 50
#define ATC_RSSI_MAX_LEN 4
#define ATC_NW_OP_CODE_MAX_LEN 6

typedef enum atc_tagFnStatus{
	ATC_FN_STATUS_TIMEOUT = -2,
	ATC_FN_STATUS_FAIL = -1,
	ATC_FN_STATUS_OK = 0,
}atc_fnStatus_t;

typedef enum atc_tag_module{
	ATC_GPS_MODULE = 1,
	ATC_LTE_MODULE,
}atc_module_t;

typedef struct atc_tagLookUpTbl{
	const char *cmd;
	const char *response;
}atc_lookUpTbl_t;

typedef struct atc_tagData{
	uint8_t data[ATC_MAX_BUFF_SIZE+1];
	uint16_t len;
}atc_data_t;

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
uint8_t atc_CmdLookUpTable(atc_cmd_t cmd, uint8_t *buff, uint16_t len);
int8_t atc_DeInit(void);

#endif /* ATC_ATC_H_ */
