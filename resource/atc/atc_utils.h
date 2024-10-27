/*
 * atc_utils.h
 *
 *  Created on: Oct 19, 2024
 *      Author: Dipan
 */

#ifndef ATC_ATC_UTILS_H_
#define ATC_ATC_UTILS_H_

#include <stdint.h>

uint8_t atc_utils_IsDigitBuff(uint8_t *buff, uint16_t size);
int16_t atc_utils_MapSQ(uint8_t uc);

#endif /* ATC_ATC_UTILS_H_ */
