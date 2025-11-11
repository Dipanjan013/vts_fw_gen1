#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils_gps.h"
#include "service_log.h"

uint8_t utils_gps_NmeaParse(char *buff, nmea_s *pTarget) {
  if (!buff || !pTarget) {
    LOG_D("[line : %d]Invalid arg\r\n", __LINE__);
    return 0;
  }
  char *pStart = strstr(buff, "RMC,");
  uint8_t ret = 0;
  do {
    // Check RMC in the nmea sentence
    if (!pStart) {
			LOG_D("[line : %d]RMC not found!\r\n", __LINE__);
      break;
    }
    // Move pointer to the first data field (skip "RMC,")
    buff = pStart + 4;
    // Extract UTC time
    char *tok = strchr(buff, ',');
    if (!tok) {
			LOG_D("[line : %d]UTC field not found!\r\n", __LINE__);
      break;
    }
    strncpy(pTarget->utc, buff, (tok - buff) < UTC_LEN ? (tok - buff) : UTC_LEN);
    // Check fix data character
    buff = tok + 1;
    if (*buff != 'A') {
			LOG_D("[line : %d]No Fix\r\n", __LINE__);
      break;
    }
    // Extract latitude
    buff += 2;  // buff check?
    tok = strchr(buff, ',');
    if (!tok) {
			LOG_D("[line : %d]Latitute field not found\r\n", __LINE__);
      break;
    }
    if ((tok - buff) < 2) {
			LOG_D("[line : %d]Latitute data error\r\n", __LINE__);
      break;
    }
    // Copy degrees (first two digits)
    memcpy(pTarget->lat, buff, 2);
    // Convert minutes to decimal and append
    double dms = 0;
    dms = atof(buff + 2) / 60.0;
    uint32_t val = 0;
    val = (uint32_t)(dms * 100000000);
    snprintf(pTarget->lat + 2, LAT_LON_LEN - 2, ".%08lu", val);

    // Extract latitude pole (N/S)
    buff = tok + 1;
    if (*buff == 'N' || *buff == 'S') {
      pTarget->latPole = *buff;
    } else {
			LOG_D("[line : %d]Latitude pole incorrect\r\n", __LINE__);
      break;
    }

    // Extract longitude
    buff += 2;  // buff check?
    tok = strchr(buff, ',');
    if (!tok) {
			LOG_D("[line : %d]Longitude field not found\r\n", __LINE__);
      break;
    }
    if ((tok - buff) < 3) {
			LOG_D("[line : %d]Longitude data error\r\n", __LINE__);
      break;
    }
    // Copy degrees (first two digits)
    memcpy(pTarget->lon, buff, 3);
    // Convert minutes to decimal and append
    dms = atof(buff + 3) / 60.0;
    val = 0;
    val = (uint32_t)(dms * 100000000);
    snprintf(pTarget->lon + 3, LAT_LON_LEN - 2, ".%08lu", val);

    // Extract longitude pole (E/W)
    buff = tok + 1;
    if (*buff == 'E' || *buff == 'W') {
      pTarget->lonPole = *buff;
    } else {
			LOG_D("[line : %d]Longitude pole incorrect\r\n", __LINE__);
      break;
    }

    // Extract Date
    buff += 2;                // buff check?
    tok = strchr(buff, ',');  // skip 1 comma
    if (!tok) return 0;
    buff = tok + 1;
    tok = strchr(buff, ',');  // skip 1 comma
    if (!tok) return 0;
    buff = tok + 1;

    // Parse Date
    tok = strchr(buff, ',');
    if (!tok) {
			LOG_D("[line : %d]Date field error\r\n", __LINE__);
      break;
    }
    strncpy(pTarget->date, buff, (tok - buff) < DATE_LEN ? (tok - buff) : DATE_LEN);
    ret = 1;
  } while (0);
  return ret;
}

void utils_gps_PrintNmea(const nmea_s *pNmea)
{
	LOG_D("[%s]\r\n", __func__);
	LOG_D("UTC: %s\n", pNmea->utc);
	LOG_D("Date: %s\n", pNmea->date);
	LOG_D("Latitude: %s\n", pNmea->lat);
	LOG_D("Latitude Pole: %c\n", pNmea->latPole);
	LOG_D("Longitude: %s\n", pNmea->lon);
	LOG_D("Longitude Pole: %c\n", pNmea->lonPole);
}
