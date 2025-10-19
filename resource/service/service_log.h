#ifndef __SERVICE_LOG_H__
#define __SERVICE_LOG_H__

#include <stdio.h>

// Log levels
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_DEBUG 4

// ===== USER CONFIG =====
#define LOG_LEVEL       LOG_LEVEL_DEBUG   // global log level

// ANSI color codes
#define CLR_RESET   "\033[0m"
#define CLR_RED     "\033[31m"
#define CLR_YELLOW  "\033[33m"
#define CLR_GREEN   "\033[32m"
#define CLR_CYAN    "\033[36m"

// ===== Core Macros =====
#define LOG_E(fmt, ...)  do { if (LOG_LEVEL >= LOG_LEVEL_ERROR) printf(CLR_RED fmt CLR_RESET "\r\n", ##__VA_ARGS__); } while(0)
#define LOG_W(fmt, ...)  do { if (LOG_LEVEL >= LOG_LEVEL_WARN)  printf(CLR_YELLOW fmt CLR_RESET "\r\n", ##__VA_ARGS__); } while(0)
#define LOG_I(fmt, ...)  do { if (LOG_LEVEL >= LOG_LEVEL_INFO)  printf(CLR_GREEN fmt CLR_RESET "\r\n", ##__VA_ARGS__); } while(0)
#define LOG_D(fmt, ...)  do { if (LOG_LEVEL >= LOG_LEVEL_DEBUG) printf(CLR_CYAN fmt CLR_RESET "\r\n", ##__VA_ARGS__); } while(0)

#endif
