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

service_queue_fnStatus_t service_queue_Enqueue(service_queue_t *q, uint8_t val)
{
	if(IsFull(q)){
//		return SERVICE_QUEUE_FN_STATUS_FULL;
		q->data[q->tail] = val;
		q->tail = (q->tail + 1) % SERVICE_QUEUE_MAX_SIZE;
	}else{
		q->data[q->tail] = val;
		q->tail = (q->tail + 1) % SERVICE_QUEUE_MAX_SIZE;
		++q->numOfElements;
	}
	return SERVICE_QUEUE_FN_STATUS_OK;
}

service_queue_fnStatus_t service_queue_Dequeue(service_queue_t *q, uint8_t  *val)
{
	if(IsEmpty(q)){
		return SERVICE_QUEUE_FN_STATUS_EMPTY;
	}
	*val = q->data[q->head];
	q->head = (q->head + 1) % SERVICE_QUEUE_MAX_SIZE;
	--q->numOfElements;
	return SERVICE_QUEUE_FN_STATUS_OK;
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
	for(uint16_t i=0; i < q->numOfElements; i++){
		printf("0x%2X\r\n", q->data[i]);
	}
	printf("<<<<<<<<<<<<<End of  HEX data>>>>>>>>>>>>>>>\r\n");
	return SERVICE_QUEUE_FN_STATUS_OK;
}
