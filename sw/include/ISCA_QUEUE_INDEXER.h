/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA queue, i.e., ring buffer, indexing provider
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : ISCA_QUEUE_INDEXER.h
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
 * @file ISCA_QUEUE_INDEXER.h
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
 */

#ifndef ISCA_QUEUE_INDEXER_H
#define ISCA_QUEUE_INDEXER_H

/******
 * HEADERS
 ******/
#include "stdint.h"

/******
 * DEFINITIONS
 ******/
#define MAX_QUEUES   (8)
#define DQ_OK        (0)
#define DQ_AVAILABLE (1)
#define DQ_OCCUPIED  (-1)
#define DQ_FULL      (-2)
#define DQ_EMPTY     (-3)

/******
 * FUNCTIONS DECLARATION
 ******/
int isca_queue_acquire(uint8_t const q_slots);
int isca_queue_release(uint8_t queue_index);
int isca_queue_rd_ptr(uint8_t q_index, uint8_t *queue_rd_ptr);
int isca_queue_wr_ptr(uint8_t q_index, uint8_t *queue_wr_index);

#endif /* ISCA_QUEUE_INDEXER_H */
