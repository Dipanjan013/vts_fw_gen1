#include <string.h>
#include "utils.h"

void utils_SafeSscanfStr(char *source, char *target, int size, char *format)
{
	char temp[strlen(source)] = {0};
	int ret = sscanf(source, format, temp);
	if(ret == 1){
		strncpy(target, temp, size);
	}
}

void utils_CopyDigitsBuff(uint8_t *pSourceBuff, uint8_t *pTargetBuff, uint16_t size)
{
	uint16_t cnt = 0;
	while(*pSourceBuff){
		if((*pSourceBuff >= 0x30) && (*pSourceBuff <= 0x39)){
			if(cnt < size){
				target[cnt++] = *pSourceBuff;
			}
		}
		pSourceBuff++;
	}
}
