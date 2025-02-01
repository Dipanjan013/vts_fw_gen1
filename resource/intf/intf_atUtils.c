#include "intf_atUtils.h"
#include <ctype.h>

void intf_atUtils_CopyDigitsToBuff(char *pSrc, char *pDst, uint8_t size)
{
	uint8_t i = 0;
	while(pSrc[i] && isDigit(pSrc[i]) && (i <= size)){
		pDst[i] = pSrc[i];
		i++;
	}
}
