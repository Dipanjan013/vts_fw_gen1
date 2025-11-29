/*
 * service_queue.h
 *
 *  Created on: Nov 14, 2024
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_QUEUE_H_
#define SERVICE_SERVICE_QUEUE_H_

#include <stdint.h>
#include "utils_gps.h"

// Number of user-type data statically allocated in the Queue
#define SERVICE_QUEUE_MAX_SIZE 10

/*!
 * @enum Queue operation return status
 */
typedef enum service_queue_tagFnStatus{
	SERVICE_QUEUE_FN_STATUS_EMPTY = -2,/*!< SERVICE_QUEUE_FN_STATUS_EMPTY */
	SERVICE_QUEUE_FN_STATUS_ERR = -1,  /*!< SERVICE_QUEUE_FN_STATUS_ERR */
	SERVICE_QUEUE_FN_STATUS_OK = 0,    /*!< SERVICE_QUEUE_FN_STATUS_OK */
	SERVICE_QUEUE_FN_STATUS_FULL = 1,  /*!< SERVICE_QUEUE_FN_STATUS_FULL */
}service_queue_fnStatus_t;

/*!
 * User-type data available in the queue
 */
typedef struct{
	uint32_t unixTime;
	nmea_s gpsData;
}service_queue_sensorData_s;

/*!
 * This is the queue data type
 */
typedef struct service_queue_tagQueue{
	service_queue_sensorData_s data[SERVICE_QUEUE_MAX_SIZE + 1];
	uint16_t head;
	uint16_t tail;
	uint16_t numOfElements;
}service_queue_t;

/*!
 * @fn API to initialize the statically allocated queue
 * @param q - pointer to the queue
 * @note -  It initializes all elements with 0
 * @return service_queue_fnStatus_t
 */
service_queue_fnStatus_t service_queue_Init(service_queue_t *q);

/*!
 * @fn API to add service_queue_sensorData_s item to the queue.
 * @param q -pointer to the queue
 * @param data -
 * @note - call this multiple time if want to add multiple data
 * @return
 */
service_queue_fnStatus_t service_queue_Enqueue(service_queue_t *q, service_queue_sensorData_s *pData);
service_queue_fnStatus_t service_queue_Dequeue(service_queue_t *q, service_queue_sensorData_s  *pData);
service_queue_fnStatus_t service_queue_Peek(service_queue_t *q, service_queue_sensorData_s  *pTarget, uint16_t index);
void service_queue_DeleteItem(service_queue_t *q, uint16_t numOfItems);
uint16_t service_queue_NumOfItems(service_queue_t *q);
void service_queue_Reset(service_queue_t *q);
service_queue_fnStatus_t service_queue_PrintInfo(service_queue_t *q);


#endif /* SERVICE_SERVICE_QUEUE_H_ */
