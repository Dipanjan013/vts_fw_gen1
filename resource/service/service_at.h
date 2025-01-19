/*
 * service_at.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_AT_H_
#define SERVICE_SERVICE_AT_H_

#include "intf_at.h"

#define SERVICE_AT_STR "AT\r\n"
#define SERVICE_AT_CGMI_STR "AT+CGMI=?\r\n"	//mfg ID
#define SERVICE_AT_CGMM_STR "AT+CGMM=?\r\n"	//MT model info
#define SERVICE_AT_CGMR_STR "AT+CGMR=?\r\n"	//MT model revision
#define SERVICE_AT_CIMI_STR "AT+CIMI=?\r\n"	//IMSI
#define SERVICE_AT_CGSN_STR "AT+CGSN=?\r\n"	//IMEI
#define SERVICE_AT_CCLK_READ_STR "AT+CCLK?\r\n"	//read real time clock of MT
/*!
 * @note: write real time clock of MT. Append the <time> as  "yy/MM/dd,hh:mm:ss±zz".
 * For instance, 6th of May 2014, 22:10:00 GMT+2 hours equal"2014/05/06,22:10:00+08"
 */
#define SERVICE_AT_CCLK_WRITE_STR "AT+CCLK="
#define SERVICE_AT_ICCID_STR "AT+ICCID=?\r\n"	//ICCID
#define SERVICE_AT_CBST_READ_STR "AT+CBST?\r\n"	//Read the current  baud rate of UE
#define SERVICE_AT_CBST_SUPPORT_STR "AT+CBST=?\r\n"	//Read the supported baud rate of UE
#define SERVICE_AT_CBST_WRITE_STR "AT+CBST="	//Set the baud rate
#define SERVICE_AT_MEMSTATUS_STR "AT+MEMSTATUS\r\n"	//read the current free heap and minimum free heap
#define SERVICE_AT_READ_SIM_CHANNEL_STR "AT^SIMSWAP=?\r\n"	//read the SIM current slot. (0 = internal SIM and 1 = external SIM)
#define SERVICE_AT_SET_ESIM_STR "AT^SIMSWAP=0\r\n"	//set internal SIM
#define SERVICE_AT_SET_EXT_SIM_STR "AT^SIMSWAP=1\r\n"	//set external SIM

#define SERVICE_AT_CREG_READ_STR "AT+CREG?\r\n"	//Read network registration status
#define SERVICE_AT_CSQ_STR "AT+CSQ\r\n"	//Read network signal quality  strength



/*!
 * @brief : Sends "AT" and wait for OK/ERROR response
 * @return 1 if OK else 0
 */
uint8_t service_at_AT(port_uart_handle_t *handle, uint16_t timeoutMs);
/*!
 * @brief : Command to get the IMSI
 */
uint8_t service_at_GetIMSI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Command to get the IMEI number
 */
uint8_t service_at_GetIMEI(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Command to get the ICCID of SIM card
 */
uint8_t service_at_GetICCID(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Command to get Modem information
 */
uint8_t service_at_GetModemInfo(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);

/*!
 * @brief : Command to check network registration status
 */
uint8_t service_at_IsNwReg(port_uart_handle_t *handle, uint16_t timeoutMs);

/*!
 * @brief : Command to get the IMSI
 */
uint8_t service_at_CheckSignalStrength(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);
uint8_t service_at_ManNwSearch(port_uart_handle_t *handle, uint8_t *rxBuff, uint8_t size, uint16_t timeoutMs);
uint8_t service_at_DeRegNw(port_uart_handle_t *handle, uint16_t timeoutMs);
uint8_t service_at_ReRegNw(port_uart_handle_t *handle, uint16_t timeoutMs);
uint8_t service_at_CheckSimChannel(port_uart_handle_t *handle, uint8_t channel, uint16_t timeoutMs);


#endif /* SERVICE_SERVICE_AT_H_ */
