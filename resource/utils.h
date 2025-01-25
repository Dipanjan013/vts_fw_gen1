/*
 * utils.h
 *
 *  Created on: Jan 25, 2025
 *      Author: Dipanjan
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

void utils_SafeSscanfStr(char *source, char *target, int size, char *format);
void utils_CopyDigitsBuff(uint8_t *pSourceBuff, uint8_t *pTargetBuff, uint16_t size);

#endif /* UTILS_H_ */
