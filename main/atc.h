#ifndef __ATC_H__
#define __ATC_H__

#include "port_uart.h"

/**!
 * Timeout in ms for locking atc command layer
 * During this time, if another AT command comes, it will wait until the current one is finished executing
 */
#define ATC_AT_LOCK_PERIOD_MS 10000

typedef enum atc_tagFnStatus
{
    ATC_FN_STATUS_INVALID_ARG = -2,
    ATC_FN_STATUS_ERR = -1,
    ATC_FN_STATUS_OK = 0,
    ATC_FN_STATUS_BUSY = 1,
} atc_fnStatus_t;

typedef enum atc_tagModuleType
{
    ATC_GSM_MOD = UART_NUM_2, /*!< ATC WITH GSM MODULE >*/
    ATC_GPS_MOD = UART_NUM_0, /*!< ATC WITH GPS MODULE >*/
} atc_mod_type;

atc_fnStatus_t atc_Init(atc_mod_type module);

atc_fnStatus_t atc_SendCmd(atc_mod_type module, uint8_t *writeBuff, size_t writeLen); // todo :  required?

atc_fnStatus_t atc_RecvData(atc_mod_type module, uint8_t *readBuff, size_t readLen, uint32_t timeoutMs); // todo : required?

atc_fnStatus_t atc_Command(atc_mod_type module, uint8_t *writeCmd, size_t writeLen, uint8_t *readBuff, size_t readLen, uint32_t waitMs);

atc_fnStatus_t atc_Deinit(atc_mod_type module);

#endif /* __ATC_H__ */