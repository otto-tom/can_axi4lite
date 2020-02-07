/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA queue, i.e., ring buffer, indexing provider
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : ISCA_QUEUE_INDEXER.c
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2017-09-24
 * |-- Last update: 2018-10-22
 * |-- License    :
 * |--   This program is free software: you can redistribute it and/or modify
 * |--   it under the terms of the GNU General Public License as published by
 * |--   the Free Software Foundation, either version 3 of the License, or
 * |--   (at your option) any later version.
 * |--
 * |--   This program is distributed in the hope that it will be useful,
 * |--   but WITHOUT ANY WARRANTY; without even the implied warranty of
 * |--   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * |--   GNU General Public License for more details.
 * |--
 * |--   You should have received a copy of the GNU General Public License
 * |--   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * |-- 
 * |-- Platform   :
 * |-- Standard   :
 * |#----------------------------------------------------------------------------#
 * |-- Description:
 * |#----------------------------------------------------------------------------#
 * |-- Copyright (c) 2018
 * |#----------------------------------------------------------------------------#
 * |-- Revisions  :
 * |-- Date        Version  Author  Description
 * |-- 2018-10-22  1.0      Otto    Created
 * \#-----------------------------------------------------------------------------\
 * *******************************************************************************/

/**
 * @file ISCA_QUEUE_INDEXER.c
 *
 * @brief Provides tools to initialize and use a preallocated
 * circular queue pointers
 *
 * @author Othon Tomoutzoglou
 *
 * Contact: <otto_sta@hotmail.com>
 *
 * @version 1.0
 *
 * @todo Implement lock mechanisms for multi-threaded environments
 *
 */

/******
 * HEADERS
 ******/
#include "ISCA_QUEUE_INDEXER.h"

/******
 * DEFINITIONS
 ******/
typedef struct {
	uint8_t rd_ptr;
	uint8_t wr_ptr;
} isca_data_queue_indexer;

/******
 * VARIABLES
 ******/
//TODO: implement lock for multi-threaded environment
static volatile uint32_t available_queues = 0U;
//TODO: use it as mutex to access&modify available_queues
//static volatile uint8_t queues_lock;
static volatile uint8_t queue_slots[MAX_QUEUES];
static volatile isca_data_queue_indexer queue[MAX_QUEUES];

/******
 * LOCAL FUNCTIONS DECLARATION
 ******/
void queue_lock_acquire();
void queue_lock_release();

/******
 * FUNCTIONS DEFINITION
 ******/
/**
 * @brief      Generic queue (ring buffer) indexer
 * @note       The queue to store data is not allocated here; only queue indexing is provided
 * @param[in]  q_slots the amount of slots the new queue shall have
 * @retval     Acquired queue index
 */
int isca_queue_acquire(uint8_t const q_slots) {

	int i;

	//find an available queue indexer
	for ( i = 0; i < MAX_QUEUES; i++ ) {
		if ( ( ( available_queues >> i ) & 1U ) == 0U ) {
			available_queues |= ( 1U << i ); //reserve the queue
			/*Update acquired queue info*/
			queue_slots[i]    = q_slots;
			queue[i].rd_ptr   = 0U;
			queue[i].wr_ptr   = 0U;
			return i;
		} /*queue occupied*/
	}

	//TODO: return a hash of the queue and match it internally with the queue's index
	//      in this way only
	/*return the acquired queue*/
	return DQ_OCCUPIED;
}

int isca_queue_release(uint8_t queue_index) {

	if ( ( available_queues >> queue_index ) & 1U ) {
		queue_slots[queue_index] = 0U;
		queue[queue_index].rd_ptr = 0U;
		queue[queue_index].wr_ptr = 0U;
		available_queues ^= ((uint32_t) 1U << queue_index); //release queue
	} /*queue occupied*/
	else {
		return DQ_AVAILABLE;
	} /*queue is not occupied*/

	return DQ_OK;
}

/**
 * <@bug When asking from RX queue with \ref isca_queue_rd_ptr the library advances
 * the read pointer; it is possible that the slot (read pointer) might be overwritten
 * by a newly arrived frame.
 * */
int isca_queue_rd_ptr(uint8_t q_index, uint8_t *queue_rd_ptr) {

	if ( queue[q_index].rd_ptr == queue[q_index].wr_ptr ) {
		return DQ_EMPTY;
	} /*Queue empty*/

	*queue_rd_ptr = (uint8_t) queue[q_index].rd_ptr;
	queue[q_index].rd_ptr = (queue[q_index].rd_ptr + 1) % queue_slots[q_index];

	return DQ_OK;
}

int isca_queue_wr_ptr(uint8_t q_index, uint8_t *queue_wr_index) {

	uint8_t next_wr_index;

	next_wr_index = (queue[q_index].wr_ptr + 1) % queue_slots[q_index];

	if ( next_wr_index == queue[q_index].rd_ptr ) {
		return DQ_FULL;
	} /*Queue full*/

	*queue_wr_index = (uint8_t) (queue[q_index].wr_ptr);
	queue[q_index].wr_ptr = next_wr_index;

	return DQ_OK;
}

/******
 * LOCAL FUNCTIONS DEFINITION
 ******/
void queue_lock_acquire() {
}

void queue_lock_release() {
}
