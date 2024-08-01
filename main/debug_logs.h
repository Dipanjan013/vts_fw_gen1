#ifndef _DEBUG_LOGS_H_
#define _DEBUG_LOGS_H_
#include <stdio.h>

/* Log levels for log print
 * 0 : No logs
 * 1 : All logs i.e (error logs, information logs, warning logs)
 * 2 : Error logs only
 * 3 : Error and warning logs only
 */
#define LOG_LEVEL 1

/**
 * LOG_E : Error logs
 * LOG_W : Warning logs
 * LOG_I : Information logs
 * LOG_V : Verbose logs (extra prints like pay load, raw data)
 */
#define LOG_E(...)                         \
    {                                      \
        if (LOG_LEVEL > 0)                 \
            printf("\033[31m"__VA_ARGS__); \
    }
#define LOG_W(...)                            \
    {                                         \
        if (LOG_LEVEL == 1 || LOG_LEVEL == 3) \
            printf("\033[33m"__VA_ARGS__);    \
    }
#define LOG_I(...)                        \
    {                                     \
        if (LOG_LEVEL == 1)               \
            printf("\033[0m"__VA_ARGS__); \
    }
#define LOG_V(...)                         \
    {                                      \
        if (LOG_LEVEL == 1)                \
            printf("\033[32m"__VA_ARGS__); \
    }

#endif /* _DEBUG_LOGS_H_ */