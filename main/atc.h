#ifndef __ATC_H__
#define __ATC_H__

#include "port_uart.h"

/**!
 * Timeout in ms for locking atc command layer
 * During this time, if another AT command comes, it will wait until the current one if finished executing
 */
#define ATC_AT_LOCK_PERIOD_MS 10000

/* Buffer size max for the AT command buffer i.e send /receive buffer */
#define ATC_BUFF_SIZE CONFIG_UART_BUFF_SIZE

typedef enum atc_tagFnStatus
{
    ATC_FN_STATUS_INVALID_ARG = -2,
    ATC_FN_STATUS_ERR = -1,
    ATC_FN_STATUS_OK = 0,
    ATC_FN_STATUS_BUSY = 1,
} atc_fnStatus_t;

/**!
 * @struct atc_t
 * @brief Structure that contains ATC info
 */
typedef struct
{
    bool initiated;                    // flag to check if the ATC layer is initialized for the particular UART NUM
    bool lock;                         // flag to check AT is lock-in state
    size_t rxBuffLen;                  // Received data length (for those commands where data is received after sending AT command)
    port_uart_num_t uartNum;           // UART number with which AT command is send/receive
    uint8_t rxBuff[ATC_BUFF_SIZE + 1]; // received data buffer (for those commands where data is received after sending AT command)
} atc_t;

/**!
 * @fn atc_Init(atc_t *atc);
 * @brief Initialize the AT command with particular UART number
 * @param atc - ATC structure
 * @return atc_fnStatus_t
 */
atc_fnStatus_t atc_Init(atc_t *atc);

/**!
 * @fn: Sends the atc command and doesn't wait for any response
 * @param atc - atc structure
 * @param buff - command buffer
 * @param buffSize - command length
 * @return atc_fnStatus_t
 */
atc_fnStatus_t atc_Transmit(atc_t *atc, uint8_t *buff, uint16_t buffSize);

/**
 * @fn : Sends the command and wait for the response
 * @param atc - atc structure
 * @param cmd - command buffer to send
 * @param cmdLen - command length
 * @param timeoutMs - timeout waiting to send the command and wait for response
 * @note : Received data will be copied to atc->rxBuff
 * @return atc_fnStatus_t
 */
atc_fnStatus_t atc_Command(atc_t *atc, uint8_t *cmd, uint16_t cmdLen, uint16_t timeoutMs);

/**!
 * @fn atc_Deinit(atc_t *atc);
 * @brief Function to deinitialize the ATC. Basically deinitialize the UART
 * @param atc- ATC structure
 * @return atc_fnStatus_t
 */
atc_fnStatus_t atc_Deinit(atc_t *atc);

#endif /* __ATC_H__ */