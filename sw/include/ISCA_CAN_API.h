/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA_CAN API library
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : ISCA_CAN_TEST.h
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2017-05-27
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
 * @file ISCA_CAN_API.h
 *
 * @brief Provides an application programming interface
 *
 * @author Othon Tomoutzoglou
 *
 * Contact: <otto_sta@hotmail.com>
 *
 * @version 1.0
 *
 */

#ifndef ISCA_CAN_API_H
#define ISCA_CAN_API_H

/******
 * HEADERS
 ******/
#include "ISCA_CAN.h"
#include <string.h>

/******
 * DEFINITIONS
 ******/
#define LOCAL_NODE_ID		  (0xACU) /*!<Local node ID*/

/*-----
 * CAN CRYPTO MODE
 *----*/
#define ISCA_CAN_API_ENC      (0U) /*!<Encode constant*/
#define ISCA_CAN_API_DEC      (1U) /*!<Decode constant*/

/******
 * FUNCTIONS DECLARATION
 ******/
int lbr_isca_can_init(can_ctrl_s *can_ctrl, int queue_slots);

int lbr_isca_can_start(can_ctrl_s *can_ctrl);

int lbr_isca_can_stop(can_ctrl_s *can_ctrl);

int lbr_isca_can_receive_pkt(can_ctrl_s *can_ctrl, can_frame_s *rx_frame);

int lbr_isca_can_transmit_pkt(can_ctrl_s *can_ctrl, can_frame_s *tx_frame);

#endif /* ISCA_CAN_API_H */
