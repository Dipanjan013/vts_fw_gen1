#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"

uint8_t MyStrCaseStr(char *pMainStr, char *pSubStr)
{
    char UpMainStr[strlen(pMainStr)+1];
    char UpSubStr[strlen(pSubStr)+1];
    memset(UpMainStr, 0, sizeof(UpMainStr));
    memset(UpSubStr, 0, sizeof(UpSubStr));
    uint16_t i=0,j=0;
    //convert pMainStr string to upper case
    while(*pMainStr){
        UpMainStr[i++] = toupper(*pMainStr);
        pMainStr++;
    }
    //convert pSubStr string to upper case
    while(*pSubStr){
        UpSubStr[j++] = toupper(*pSubStr);
        pSubStr++;
    }
    return (NULL != strstr(UpMainStr, UpSubStr));
}

void utils_SafeSscanfStr(char *source, uint16_t sourceSize, char *target, uint16_t targetSize, char *format)
{
	char temp[sourceSize + 1];
	memset(temp, 0, sizeof(temp));
	int ret = sscanf(source, format, temp);	//extracted string size will be lesser than the source buffer size.
	if(ret == 1){
		strncpy(target, temp, targetSize);	//now copy the extracted string to target buffer with the provided target size & hence buff overflow risk is handled
	}
}

void utils_CopyDigitsBuff(uint8_t *pSourceBuff, uint8_t *pTargetBuff, uint16_t size)
{
	uint16_t cnt = 0;
	while(*pSourceBuff){
		if((*pSourceBuff >= 0x30) && (*pSourceBuff <= 0x39)){
			if(cnt < size){
				pTargetBuff[cnt++] = *pSourceBuff;
			}
		}
		pSourceBuff++;
	}
}

int16_t utils_MapCSQ(uint8_t val)
{
	int16_t rssi = 0;
	switch(val){
		case 0:
			rssi = -113;
			break;
		case 1:
			rssi = -111;
			break;
		case 2:
			rssi = -109;
			break;;
		case 30:
			rssi = -53;
			break;
		case 31:
			rssi = -51;
			break;
		case 99:
			rssi = 0;
			break;
		default:{	//between 2 to 30
			rssi = - (113 - (val*2));
		}
	}
	return rssi;
}
