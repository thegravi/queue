/*
 * Queue.c
 *
 *  Created on: 2014 liep. 4
 *      Author: gravi_
 */
#include <string.h>

#include "Queue.h"

#define MAX_QUEUE (uint8_t)10

struct Queue_t {
    void *buffer;
    uint16_t bufferSize;
    uint16_t elemCnt;
    uint16_t headIdx;
    uint16_t tailIdx;
    uint8_t elemSize;
};

static uint8_t qUsedCnt = 0;
static uint32_t qBitList = 0;
static Queue_t q[MAX_QUEUE];

static inline void setQueueBit(uint8_t qIdx)
{
	if (qIdx >= MAX_QUEUE)
		return;

	qBitList |= (1<<qIdx);
}

static inline void clearQueueBit(uint8_t qIdx)
{
	if (qIdx >= MAX_QUEUE)
		return;

	qBitList &= ~(1<<qIdx);
}

static inline uint8_t isSetQueueBit(uint8_t qIdx)
{
	return (qBitList & (1 << qIdx)) ? 1 : 0;
}

static Queue_t *createQueueObject()
{
    if (qUsedCnt >= MAX_QUEUE)
        return NULL;

	/* Looking for empty queue object */
    uint8_t qIdx;
    for (qIdx = 0; qIdx < MAX_QUEUE; qIdx++) {
    	if (!isSetQueueBit(qIdx))
    		break;
    }

    if (qIdx == MAX_QUEUE)
    	return NULL;

    setQueueBit(qIdx);
    qUsedCnt++;
    return &q[qIdx];
}

static int8_t destroyQueueObject(Queue_t **queue)
{
	if (qUsedCnt == 0)
		return _Q_EMPTY;

	/* Looking for particular queue object */
	uint8_t qIdx;
	for (qIdx = 0; qIdx < MAX_QUEUE; qIdx++) {
		if (*queue == &q[qIdx]) {
			*queue = NULL;
			clearQueueBit(qIdx);
			qUsedCnt--;
			return _Q_SUCC;
		}
	}

	return _Q_FAIL;
}

int8_t QUEUE_init(Queue_t **queue, void *buffer, uint16_t bufferSize, uint8_t elemSize)
{
    *queue = createQueueObject();
    if (*queue == NULL)
        return _Q_FULL;

	memset((void*)*queue, 0, sizeof(**queue));

	(*queue)->buffer     = buffer;
	(*queue)->bufferSize = bufferSize;
	(*queue)->elemSize   = elemSize;

	return _Q_SUCC;
}

int8_t QUEUE_deinit(Queue_t **queue)
{
	memset(*queue, 0, sizeof(**queue));
	return destroyQueueObject(queue);
}

int8_t QUEUE_addElem(Queue_t *queue, void *elem)
{
	if (queue == NULL)
	    return _Q_FAIL;

	if (queue->elemCnt + 1 > queue->bufferSize)
	    return _Q_FULL;

	// copy data to buffer at particular index
	memcpy((void*)(queue->buffer + (queue->headIdx * queue->elemSize)), (void*)elem, queue->elemSize);
	queue->headIdx = (queue->headIdx + 1) % queue->bufferSize;
	queue->elemCnt++;

	return _Q_SUCC;
}

int8_t QUEUE_getElem(Queue_t *queue, void *elem)
{
	if (queue == NULL)
	    return _Q_FAIL;

	if (queue->elemCnt == 0)
	    return _Q_EMPTY;

	// get data from buffer to user variable
	memcpy((void*)elem, (void*)(queue->buffer + (queue->tailIdx * queue->elemSize)), queue->elemSize);
	memset((void*)(queue->buffer + (queue->tailIdx * queue->elemSize)), 0, queue->elemSize);
	queue->tailIdx = (queue->tailIdx + 1) % queue->bufferSize;
	queue->elemCnt--;

	return _Q_SUCC;
}

uint16_t QUEUE_getElemCount(Queue_t *queue)
{
	return queue == NULL ? 0 : queue->elemCnt;
}

int8_t QUEUE_flush(Queue_t *queue)
{
	if (queue == NULL)
	    return _Q_FAIL;

	queue->elemCnt = 0;
	queue->headIdx = 0;
	queue->tailIdx = 0;

	return _Q_SUCC;
}

int8_t QUEUE_getElemNoMove(Queue_t *queue, void *elem, uint8_t idx)
{
	if (queue == NULL)
	    return _Q_FAIL;

	if (queue->elemCnt <= 0)
	    return _Q_EMPTY;

	memcpy((void*)elem, (void*)(queue->buffer + ((queue->tailIdx + idx) * queue->elemSize)), queue->elemSize);

	return _Q_SUCC;
}

uint8_t QUEUE_getFreeQueueCnt(void)
{
	return MAX_QUEUE - qUsedCnt;
}

#if TESTS
void QUEUE_runTests(void)
{
	Queue_t *qu[MAX_QUEUE+1];
	int buffer[MAX_QUEUE+1][10];
	int max_queues = MAX_QUEUE;
	int i;

/* Testing set queue bit list */
	for (i = 0; i < MAX_QUEUE; i++) {
		setQueueBit(i);
		if (!isSetQueueBit(i)) {
			printf("%s %d| Cannot set queue list bit of index %d\n\r", __FILE__, __LINE__, i);
			return;
		}
	}

	setQueueBit(i+1);
	if (isSetQueueBit(i)) {
		printf("%s %d| Bit should not be set outsided max bounds\r\n", __FILE__, __LINE__);
		return;
	}

/* Testing clear queue bit list */
	for (i = 0; i < MAX_QUEUE; i++) {
		clearQueueBit(i);
		if (isSetQueueBit(i)) {
			printf("%s %d| Cannot clear queue list bit of index %d\n\r", __FILE__, __LINE__, i);
			return;
		}
	}

	qBitList |= (1<<MAX_QUEUE);
	clearQueueBit(i);
	if (!isSetQueueBit(i)) {
		printf("%s %d| Bit should not be cleared outsided max bounds\r\n", __FILE__, __LINE__);
		return;
	}
	qBitList = 0;


/* Creating Queue Objects directly */
	Queue_t *q_cmp[MAX_QUEUE+1];
	for (i = 0; i < max_queues; i++)
		q_cmp[i] = qu[i];

	for (i = 0; i < max_queues; i++) {
		qu[i] = createQueueObject();
		if (qu[i] == NULL || qu[i] == q_cmp[i]) {
			printf("%s %d| Cannot create queue object\n\r", __FILE__, __LINE__);
			return;
		}
	}

	if (QUEUE_getFreeQueueCnt() > 0) {
		printf("%s %d| There must not be any available queue, but there is %d available\n\r", __FILE__, __LINE__, QUEUE_getFreeQueueCnt());
		return;
	}

/* Detroying Queue Objects */
	for (i = 0; i < max_queues; i++) {
		if (destroyQueueObject(&qu[i]) != _Q_SUCC) {
			printf("%s %d| Cannot destroy queue object\n\r", __FILE__, __LINE__);
			return;
		}
	}

	if (QUEUE_getFreeQueueCnt() != MAX_QUEUE) {
		printf("%s %d| There must be %d available queue, but there is %d used up\n\r", __FILE__, __LINE__, MAX_QUEUE, MAX_QUEUE - QUEUE_getFreeQueueCnt());
		return;
	}

/* Creating Queue Objects through init function */
	for (i = 0; i < max_queues; i++)
		q_cmp[i] = qu[i];

	for (i = 0; i < max_queues; i++) {
		if ((QUEUE_init(&qu[i], *(buffer+i), sizeof(buffer[0])/sizeof(buffer[0][0]), sizeof(buffer[0][0])) != _Q_SUCC)
			|| (qu[i] == q_cmp[i])) {
			printf("%s %d| Cannot initialize queue of index %d\r\n", __FILE__, __LINE__, i);
			return;
		}
	}

	if (QUEUE_getFreeQueueCnt() > 0) {
		printf("%s %d| There must not be any available queue, but there is %d available\n\r", __FILE__, __LINE__, QUEUE_getFreeQueueCnt());
		return;
	}

	if (QUEUE_init(qu+i, *(buffer+i), sizeof(buffer[0])/sizeof(buffer[0][0]), sizeof(buffer[0][0])) != _Q_FULL) {
		printf("%s %d| Allocated size for queues is used up, but status returned is not _Q_FULL\n\r", __FILE__, __LINE__);
		return;
	}

	if (QUEUE_getFreeQueueCnt() > 0) {
		printf("%s %d| There must not be any available queue, but there is %d available\n\r", __FILE__, __LINE__, QUEUE_getFreeQueueCnt());
		return;
	}

/* Detroying Queue Objects */
	for (i = 0; i < max_queues; i++) {
		if (destroyQueueObject(&qu[i]) != _Q_SUCC) {
			printf("%s %d| Cannot destroy queue object\n\r", __FILE__, __LINE__);
			return;
		}
	}

	if (destroyQueueObject(NULL) != _Q_EMPTY) {
		printf("%s %d| Cannot destroy object, because queue is already empty\n\r", __FILE__, __LINE__);
		return;
	}

	if (QUEUE_getFreeQueueCnt() != MAX_QUEUE) {
		printf("%s %d| There must be %d available queue, but there is %d used up\n\r", __FILE__, __LINE__, MAX_QUEUE, MAX_QUEUE - QUEUE_getFreeQueueCnt());
		return;
	}

/* Adding elements to queue */
	Queue_t *q1;
	uint32_t b1[200];
	if (QUEUE_init(&q1, b1, sizeof(b1)/sizeof(b1[0]), sizeof(b1[0])) != _Q_SUCC) {
		printf("%s %d| Cannot initialize queue\r\n", __FILE__, __LINE__);
		return;
	}

	if ((MAX_QUEUE - QUEUE_getFreeQueueCnt()) != 1) {
		printf("%s %d| Should only 1 queue be initialized\n\r", __FILE__, __LINE__);
		return;
	}

	int itemsCnt = 200;
	uint32_t item;
	for (i = 0; i < itemsCnt; i++) {
		item = i;
		if (QUEUE_addElem(q1, &item) != _Q_SUCC) {
			printf("%s %d| Cannot add element to queue\n\r", __FILE__, __LINE__);
			return;
		}
	}

	if (QUEUE_getElemCount(q1) != itemsCnt) {
		printf("%s %d| Wrong number of elements store in queue. Must be %d, but we have %d\n\r", __FILE__, __LINE__, itemsCnt, QUEUE_getElemCount(q1));
		return;
	}

	if (QUEUE_addElem(q1, &item) != _Q_FULL) {
		printf("%s %d| Item should not be added to full queue\n\r", __FILE__, __LINE__);
		return;
	}

	if (QUEUE_getElemCount(q1) != itemsCnt) {
		printf("%s %d| Wrong number of elements store in queue. Must be %d, but we have %d\n\r", __FILE__, __LINE__, itemsCnt, QUEUE_getElemCount(q1));
		return;
	}

	for (i = 0; i < itemsCnt; i++) {
		if (QUEUE_getElemNoMove(q1, &item, i) != _Q_SUCC) {
			printf("%s %d| Cannot get item from queue\n\r", __FILE__, __LINE__);
			return;
		}

		if (item != i) {
			printf("%s %d| Retrieved item from queue does not match added on (%d != %d)\r\n", __FILE__, __LINE__, item, i);
			return;
		}
	}

	if (QUEUE_getElemCount(q1) != itemsCnt) {
		printf("%s %d| Wrong number of elements store in queue. Must be %d, but we have %d\n\r", __FILE__, __LINE__, itemsCnt, QUEUE_getElemCount(q1));
		return;
	}

	for (i = 0; i < itemsCnt; i++) {
		if (QUEUE_getElem(q1, &item) != _Q_SUCC) {
			printf("%s %d| Cannot get element from queue at index %d\n\r", __FILE__, __LINE__, i);
			return;
		}

		if (item != (uint32_t)i) {
			printf("%s %d| Retrieved item does not match added one (%d != %d)\n\r", __FILE__, __LINE__, item, i);
			return;
		}
	}

	if (QUEUE_getElemCount(q1) != 0) {
		printf("%s %d| Wrong number of elements store in queue. Must be %d, but we have %d\n\r", __FILE__, __LINE__, itemsCnt, QUEUE_getElemCount(q1));
		return;
	}

/* Testing queue flush*/
	for (i = 0; i < itemsCnt; i++) {
		QUEUE_addElem(q1, &i);
	}

	if (QUEUE_flush(q1) != _Q_SUCC) {
		printf("%s %d| Cannot flush queue\n\r", __FILE__, __LINE__);
		return;
	}

	printf("All Unit tests passed!\n\r");
}
#endif
