/*
 * Queue.h
 *
 *  Created on: 2014 liep. 4
 *      Author: gravi_
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

#define _Q_SUCC		 0
#define _Q_FULL		-2
#define _Q_FAIL		-1
#define _Q_EMPTY	-3

#define TESTS 0

typedef struct Queue_t Queue_t;

/**
 * Initializes queue
 * @param q 			- pointer to structure
 * @param buffer		- pointer to queue buffer
 * @param bufferSize	- determines buffer size
 * @param elemSize		- determines size of buffer element
 * @return status
 */
int8_t QUEUE_init(Queue_t **q, void *buffer, uint16_t bufferSize, uint8_t elemSize);

/**
* Destroys queue object
* @param q - pointer to pointer to queue object
* @return status
*/
int8_t QUEUE_deinit(Queue_t **q);

/**
 * Adds element to queue buffer
 * @param q 	- pointer to pointer to structure
 * @param elem  - element to be written into queue buffer
 * @return status
 */
int8_t QUEUE_addElem(Queue_t *q, void *elem);

/**
 * Gets oldest value from queue buffer
 * @param q 	- pointer to structure
 * @param elem  - pointer to variable which stores element from queue buffer
 * @return status
 */
int8_t QUEUE_getElem(Queue_t *q, void *elem);

/**
 * Returns number of available elements in queue buffer
 * @param q - pointer to structure
 * @return number of available elements in buffer
 */
uint16_t QUEUE_getElemCount(Queue_t *q);

/**
 * Clears whole buffer
 * @param q - pointer to structure
 * @return status
 */
int8_t QUEUE_flush(Queue_t *q);

/**
 * Returns error code at particular index without moving queue elements
 * @param q 	- pointer to queue
 * @param elem  - pointer to element
 * @param idx   - index in queue
 * @return element at particular index
 */
int8_t QUEUE_getElemNoMove(Queue_t *q, void *elem, uint8_t idx);

/**
* Returns number of avaialbles queues
* @return number of available queues
*/
uint8_t QUEUE_getFreeQueueCnt(void);

#if TESTS

#include <stdio.h>
/**
* Runs Unit tests
*/
void QUEUE_runTests(void);

#endif

#endif /* QUEUE_H_ */
