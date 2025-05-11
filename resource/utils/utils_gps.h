/*
 * utils_gps.h
 *
 *  Created on: Mar 16, 2025
 *      Author: Dipanjan
 */

#ifndef UTILS_GPS_H_
#define UTILS_GPS_H_

#include <stdint.h>

#define NMEA_LOG 1				//for nmea parsing log print
#define UTC_LEN 6   			//hhmmss
#define DATE_LEN 6  			//ddmmyy
#define LAT_LON_LEN 20		//For latitude : xx.xxxxxxxx (using 8 decimal points for more accuracy). For longitude : xxx.xxxxxxxx

/**
 * @struct : NMEA parsed results will be stored in this structure
 * @param utc : Contains the UTC time as hhmmss (ms is ignored) during parsing
 * @param date : ddmmyy
 * @param lat : latitude as xx.12345678 (upto 8 decimal points)
 * @param latPole : N or S
 * @param lon : longitude as xxx.12345678 (upto 8 decimal points)
 * @param lonPole : E or W
 */
typedef struct{
    char utc[UTC_LEN + 1];
    char date[DATE_LEN + 1];
    char lat[LAT_LON_LEN+1];
    char latPole;
    char lon[LAT_LON_LEN+1];
    char lonPole;
}nmea_s;

/*!
 * @fn NMEA sentence parser. It only parses the RMC field and saves the data in nmea_s
 * @param buff : Source data buffer that needs to be parsed
 * @param pTarget : nmea_s type structure where parsed data will be saved
 * @return 1 if success or 0 if failure
 */
uint8_t utils_gps_NmeaParse(char *buff, nmea_s *pTarget);

/*!
 * @fn : Just to print the NMEA data
 * @param pNmea : NMEA structure used for printing
 */
void utils_gps_PrintNmea(const nmea_s *pNmea);

#endif /* UTILS_GPS_H_ */
