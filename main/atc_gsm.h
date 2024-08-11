#include "atc.h"

typedef enum atc_gsm_tagFnStatus
{
    ATC_GSM_FN_STATUS_ERR = -1,
    ATC_GSM_FN_STATUS_OK = 0,
} atc_gsm_fnStatus_t;

atc_gsm_fnStatus_t atc_gsm_StartReceiverTask(port_uart_num_t uartNum);