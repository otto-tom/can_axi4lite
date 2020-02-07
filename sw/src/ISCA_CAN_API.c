/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA_CAN API library
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : ISCA_CAN_TEST.c
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2017-05-27
 * |-- Last update: 2019-02-22
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
 * @file ISCA_CAN_API.c
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

/******
 * HEADERS
 ******/
#include "ISCA_CAN_IRQ.h"
#include "ISCA_CAN_API.h"
#include "ISCA_QUEUE_INDEXER.h"

/******
 * FUNCTIONS DEFINITION
 ******/
/******************************************************************************//**
 * @brief		  Initialize the CAN controller
 * @param[in,out] can_ctrl CAN controller instance pointer
 * @param[in]     queue_slots Amount of slots to allocate for the SW RX queue
 * @return 		  \ref ISCA_CAN_OK
 * @return        \ref ISCA_CAN_QUEUES_OCCUPIED
 * @pre           The following \ref can_ctrl 's fields should have been initialized
 *                before calling this function
 *                @code
 *                can_ctrl.addr
 *                can_ctrl.brp
 *                can_ctrl.tseg1
 *                can_ctrl.tseg2
 *                can_ctrl.sjw
 *                can_ctrl.irq
 *                can_ctrl.mask
 *                can_ctrl.code
 *                can_ctrl.frm_md //(if CAN_MODE==CAN_2B)
 *                @endcode
 ********************************************************************************/
int lbr_isca_can_init(can_ctrl_s *can_ctrl, int queue_slots) {

	int        can_rx_queue_id;
	can_ctrl_s *can_controller_l;

	/*Point local CAN controller to the passed; cast to can_ctrl_s**/
	can_controller_l = (can_ctrl_s *)can_ctrl;

	/*Create a CAN frames queue*/
	can_rx_queue_id = isca_queue_acquire(queue_slots);

	//if ( __builtin_expect(can_rx_queue_id == DQ_OCCUPIED, 0) ) {
	if ( can_rx_queue_id == DQ_OCCUPIED ) {
		return ISCA_CAN_QUEUES_OCCUPIED;
	} /*No queue available*/

	/*Update can controller struct*/
	can_controller_l->q_ptr  = (can_frame_s *)malloc(sizeof(can_frame_s)*queue_slots);
	can_controller_l->q_id   = can_rx_queue_id;
	can_controller_l->q_size = queue_slots;

	/*Set CAN controllers default interrupt callback function*/
	if ( can_controller_l->irq == CAN_IRQ_ON ) {
		can_controller_l->InterruptHandler = &ISCA_CAN_IntrHandler;
	}

	/*Setup CAN controller and return*/
	return isca_can_init(can_ctrl);

}

/********************************************************************************
 * @brief		  Read a received or wait to receive a CAN frame
 * @param[in,out] can_ctrl CAN controller instance pointer
 * @param[out]    rx_frame CAN frame instance pointer
 * @return        \ref ISCA_CAN_RX_FIFO_EMPTY
 * @return 		  \ref ISCA_CAN_INV_IO_TYPE
 * @return		  \ref ISCA_CAN_OK
 ********************************************************************************/
int lbr_isca_can_receive_pkt(can_ctrl_s* can_ctrl, can_frame_s *rx_frame) {

	uint8_t queue_rd_index;
	int q_idx;
	int ret_val = ISCA_CAN_OK;

	if (can_ctrl->irq == CAN_IRQ_ON) {

		do {
			q_idx = isca_queue_rd_ptr(((can_ctrl_s*)can_ctrl)->q_id, &queue_rd_index);
		} while( q_idx == DQ_EMPTY ); /*Wait for the interrupt handler to push a frame*/

		/*Interrupt routines use the created during initialization queue.
		 *Copy frame from  to user*/
		memmove(rx_frame, &can_ctrl->q_ptr[queue_rd_index], sizeof(can_frame_s));
	} /*Interrupt mode*/
	else {

		/*Poll until frame is available*/
		ret_val = isca_can_receive_frame(can_ctrl, rx_frame, CAN_REQ_BLOCKING);

	} /*Polling mode*/

	return ret_val;

}

/********************************************************************************
 * @brief		Transmit a CAN frame
 * @param[in]	can_ctrl  CAN controller struct
 * @param[in]   tx_frame  CAN frame struct pointer
 * @return      \ref ISCA_CAN_BUSY
 * @return      \ref ISCA_CAN_INV_IO_TYPE
 * @return      \ref ISCA_CAN_OK
 ********************************************************************************/
int lbr_isca_can_transmit_pkt(can_ctrl_s *can_ctrl, can_frame_s *tx_frame) {

	int ret_val;

	ret_val = isca_can_transmit_frame(can_ctrl, tx_frame, CAN_REQ_BLOCKING);
	return ret_val;

}

/********************************************************************************
 * @brief		Set CAN controller reset mode off and acquire a queue
 * @param[in]   can_ctrl CAN controller struct
 * @return 		Controller's previous \ref CAN_MODE0_REG value
 * @return      \ref ISCA_CAN_INV_RST_MODE
 ********************************************************************************/
//__attribute__ ((always_inline)) inline
int lbr_isca_can_start(can_ctrl_s *can_ctrl) {

	int ret_val;

	//Switch off can controller's reset mode
	ret_val = isca_can_switch_mode(can_ctrl, ISCA_CAN_MODE_RESET_OFF);

	if ( ret_val != ISCA_CAN_INV_RST_MODE ) {
		//Acquire queue
		can_ctrl->q_id = isca_queue_acquire(can_ctrl->q_size);
	} /**< Controller switched-off*/

	return ret_val;
}

/********************************************************************************
 * @brief		Set CAN controller reset mode on and release the queue
 * @param[in]   can_ctrl CAN controller struct
 * @return 		Controller's previous \ref CAN_MODE0_REG value
 * @return      \ref ISCA_CAN_INV_RST_MODE
 ********************************************************************************/
//__attribute__ ((always_inline)) inline
int lbr_isca_can_stop(can_ctrl_s *can_ctrl) {

	int ret_val;

	//Switch on can controller's reset mode
	ret_val = isca_can_switch_mode(can_ctrl, ISCA_CAN_MODE_RESET_ON);

	if ( ret_val != ISCA_CAN_INV_RST_MODE ) {
		//Release queue
		isca_queue_release(can_ctrl->q_id);
	} /**< Controller switched-off*/

	return ret_val;

}
