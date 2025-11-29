/*
 * app_utils.h
 *
 *  Created on: Nov 29, 2025
 *      Author: Dipan
 */

#ifndef APP_APP_UTILS_H_
#define APP_APP_UTILS_H_

#include <stdint.h>
#include "utils_gps.h"

uint8_t app_utils_CreateTeleRaw(char *buff, uint16_t size, nmea_s *pNmea);

#endif /* APP_APP_UTILS_H_ */
