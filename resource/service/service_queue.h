/*
 * service_queue.h
 *
 *  Created on: Nov 14, 2024
 *      Author: Dipan
 */

#ifndef SERVICE_SERVICE_QUEUE_H_
#define SERVICE_SERVICE_QUEUE_H_

#include <stdint.h>

#define SERVICE_QUEUE_MAX_SIZE 1024

typedef enum service_queue_tagFnStatus{
	SERVICE_QUEUE_FN_STATUS_EMPTY = -2,
	SERVICE_QUEUE_FN_STATUS_ERR = -1,
	SERVICE_QUEUE_FN_STATUS_OK = 0,
	SERVICE_QUEUE_FN_STATUS_FULL = 1,
}service_queue_fnStatus_t;

typedef struct service_queue_tagCQueue{
	uint8_t data[SERVICE_QUEUE_MAX_SIZE + 1];
	uint16_t head;
	uint16_t tail;
	uint16_t numOfElements;
}service_queue_t;

service_queue_fnStatus_t service_queue_Init(service_queue_t *q);
service_queue_fnStatus_t service_queue_Enqueue(service_queue_t *q, uint8_t val);
service_queue_fnStatus_t service_queue_Dequeue(service_queue_t *q, uint8_t  *val);
uint16_t service_queue_NumOfItems(service_queue_t *q);
void service_queue_Reset(service_queue_t *q);
service_queue_fnStatus_t service_queue_PrintInfo(service_queue_t *q);


#endif /* SERVICE_SERVICE_QUEUE_H_ */
