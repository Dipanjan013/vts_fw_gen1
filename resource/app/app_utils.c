#include "app_utils.h"
#include <string.h>
#include "service_log.h"

uint8_t app_utils_CreateTeleRaw(char *buff, uint16_t size, nmea_s *pNmea)
{
	int ret = snprintf(buff, size,
										 "{\"utc\":\"%s\",\"date\":\"%s\",\"lat\":\"%s\",\"latPole\":\"%c\",\"lon\":\"%s\",\"lonPole\":\"%c\"}",
										 pNmea->utc,
										 pNmea->date,
										 pNmea->lat,
										 pNmea->latPole,
										 pNmea->lon,
										 pNmea->lonPole);
	if(ret < 0 || ret > size){
		LOG_W("[%s] Buffer too small, required size %d", __func__, ret);
		return 0;
	}else{
		return 1;
	}
}
