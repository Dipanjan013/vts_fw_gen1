/*
 * service_queue.c
 *
 *  Created on: Nov 14, 2024
 *      Author: Dipanjan B.
 */
#include <stdio.h>
#include <string.h>

#include "service_queue.h"

static uint8_t IsFull(service_queue_t *q);
static uint8_t IsEmpty(service_queue_t *q);

static uint8_t IsFull(service_queue_t *q)
{
	return (q->numOfElements == SERVICE_QUEUE_MAX_SIZE);
}

static uint8_t IsEmpty(service_queue_t *q)
{
	return (q->numOfElements == 0);
}

service_queue_fnStatus_t service_queue_Init(service_queue_t *q)
{
	memset(q, 0, sizeof(service_queue_t));
	return SERVICE_QUEUE_FN_STATUS_OK;
}

service_queue_fnStatus_t service_queue_Enqueue(service_queue_t *q, service_queue_sensorData_s *pData)
{
	if(IsFull(q)){
//		return SERVICE_QUEUE_FN_STATUS_FULL;
		memcpy(&(q->data[q->tail]), pData, sizeof(service_queue_sensorData_s));
		q->tail = (q->tail + 1) % SERVICE_QUEUE_MAX_SIZE;
	}else{
		memcpy(&(q->data[q->tail]), pData, sizeof(service_queue_sensorData_s));
		q->tail = (q->tail + 1) % SERVICE_QUEUE_MAX_SIZE;
		++q->numOfElements;
	}
	return SERVICE_QUEUE_FN_STATUS_OK;
}

service_queue_fnStatus_t service_queue_Dequeue(service_queue_t *q, service_queue_sensorData_s  *pData)
{
	if(IsEmpty(q)){
		return SERVICE_QUEUE_FN_STATUS_EMPTY;
	}
	*pData = q->data[q->head];
	q->head = (q->head + 1) % SERVICE_QUEUE_MAX_SIZE;
	--q->numOfElements;
	return SERVICE_QUEUE_FN_STATUS_OK;
}

service_queue_fnStatus_t service_queue_Peek(service_queue_t *q, service_queue_sensorData_s  *pTarget, uint16_t index)
{
	if(IsEmpty(q)){
		return SERVICE_QUEUE_FN_STATUS_EMPTY;
	}
	if(index <= (q->numOfElements - 1)){
		*pTarget = q->data[q->head + index];
		return SERVICE_QUEUE_FN_STATUS_OK;
	}else{
		return SERVICE_QUEUE_FN_STATUS_ERR;
	}
}

void service_queue_DeleteItem(service_queue_t *q, uint16_t numOfItems)
{
	for(uint16_t i = 0; i < numOfItems; i++){
		if(q->numOfElements == 0) {
			break;
		}
		q->tail = q->tail - 1;
		--q->numOfElements;
	}
}

void service_queue_Reset(service_queue_t *q)
{
	memset(q, 0, sizeof(service_queue_t));
}

uint16_t service_queue_NumOfItems(service_queue_t *q)
{
	return q->numOfElements;
}

service_queue_fnStatus_t service_queue_PrintInfo(service_queue_t *q)
{
	if(q == NULL){
		printf("[%s] Queue not initialized\r\n", __func__);
		return SERVICE_QUEUE_FN_STATUS_ERR;
	}
	printf("No. of elements : %d\r\n", q->numOfElements);
	printf("Head : %d, Tail : %d\r\n", q->head, q->tail);
	printf("<<<<<<<<<<<<<HEX data>>>>>>>>>>>>>>>\r\n");
	uint16_t i = 0;
	while((q->head + i) < q->tail){
//			printf("0x%2X\r\n", q->data[q->head + i]);
			++i;
	}
	printf("<<<<<<<<<<<<<End of  HEX data>>>>>>>>>>>>>>>\r\n");
	return SERVICE_QUEUE_FN_STATUS_OK;
}
