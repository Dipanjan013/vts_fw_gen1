/*
 * atc_utils.c
 *
 *  Created on: Oct 19, 2024
 *      Author: Dipan
 */
#include "ctype.h"
#include "atc_utils.h"

uint8_t atc_utils_IsDigitBuff(uint8_t *buff, uint16_t size)
{
	for(uint16_t i = 0; i < size; i++){
		if(0 == isdigit(buff[i]))
			return 0;
	}
	return 1;
}

int16_t atc_utils_MapSQ(int val)
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
