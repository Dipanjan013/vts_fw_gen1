/*
 * system_stm32.h
 *
 *  Created on: Oct 19, 2025
 *      Author: Dipan
 */

#ifndef SYSTEM_SYSTEM_STM32_H_
#define SYSTEM_SYSTEM_STM32_H_

void system_stm32_ApplyPP(void);
__attribute__((weak)) void system_stm32_PreSleep(void);
__attribute__((weak)) void system_stm32_PostSleep(void);

#endif /* SYSTEM_SYSTEM_STM32_H_ */
