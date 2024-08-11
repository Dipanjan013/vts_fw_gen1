#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>
#include "atc.h"

// typedef enum app_tagProducedEvent
// {
// } app_producedEvent_t;

typedef struct
{
    uint16_t event;
    union
    {
        struct
        {
            char dataBuff[ATC_BUFF_SIZE + 1];
            size_t buffLen;
        } gsmData;
        struct
        {
            char dataBuff[ATC_BUFF_SIZE + 1];
            size_t buffLen;
        } gpsData;
    };
} app_eventParam_t;

#endif /* __AP_H__ */