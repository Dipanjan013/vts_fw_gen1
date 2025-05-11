/*
 * utils.h
 *
 *  Created on: Jan 25, 2025
 *      Author: Dipanjan
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

/**
 * @brief Function to find substring in main using strstr (case incensitive)
 * @note : This converts the main str and sub str to upper case 1st and then  uses strstr (bcoz strstr is case sensitive)
 * Always provide null terminated strings
 * @return : 1 if found else 0 if not
 */
uint8_t utils_MyStrCaseStr(char *pMainStr, char *pSubStr);

/**
 * @brief Function to extract values from a string buffer given the start the substring and end substring.
 * @note: It will extract the value if the start sub string is found! If end sub-string not found, it will copy until null/target buffer size
 * @param pMainStr - Main character buffer
 * @param pSubStrStart - Sub string start
 * @param pSubStrEnd - sub string end
 * @param pTargetBuff - Buffer where extracted value will be copied
 * @param size - size of the target buffer size
 * @return 1 when success or 0 if fail
 */
uint8_t utils_StrParser(char *pMainStr, char *pSubStrStart, char *pSubStrEnd, char *pTargetBuff, uint16_t size);

/*!
 * @brief : This function is used for single string extraction only using sscanf through which buffer over-flow risk is handled
 * Must supply null terminated buffer
 */
void utils_SafeSscanfStr(char *source, uint16_t sourceSize, char *target, uint16_t targetSize, char *format);

/*!
 * @brief: Function to extract the digits from a buffer to a char buffer
 */
void utils_CopyDigitsBuff(uint8_t *pSourceBuff, uint8_t *pTargetBuff, uint16_t size);

/*!
 * @brief: Maps the signal quality value to respective RSSI value
 * Check datasheet for map details
 */
int16_t utils_MapCSQ(uint8_t val);

#endif /* UTILS_H_ */
