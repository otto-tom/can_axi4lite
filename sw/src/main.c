/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA CAN applicatio example
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : main.c
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2018-10-22
 * |-- Last update: 2019-05-10
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
 * |-- Description: Before using define the in file CAN0_ROLE, CAN0_BASEADDR,
 * |                RX_QUEUE_SIZE, alongside APPRISE_MODE and CAN_MODE in
 * |                ISCA_CAN_CFG.h. The later can optionally be defined as
 * |                compiler options, e.g., $gcc ... -DCAN_MODE=CAN_2A.
 * |                Available Options List:
 * |                 - APPRISE_MODE: APP_POLL(0) / APP_IRQ(1), poll or irq mode
 * |                 - CAN_MODE: CAN_2A(0) / CAN_2B(1), basic or extended mode
 * |                 - RX_QUEUE_SIZE: Integer, SW RX CAN frames queue size
 * |                                  attached to the controller instance
 * |                 - CAN0_BASEADDR: Integer, CAN controller's physical address
 * |                 - CAN0_ROLE: TX(0) / RX(1), main TXes or RXes a CAN frame
 * |#----------------------------------------------------------------------------#
 * |-- Copyright (c) 2018
 * |#----------------------------------------------------------------------------#
 * |-- Revisions  :
 * |-- Date        Version  Author  Description
 * |-- 2018-10-22  1.0      OtTo    Created
 * \#-----------------------------------------------------------------------------\
 * *******************************************************************************/

/** \mainpage CAN controller documentation
 *
 * Use the menus or the search field to navigate and get informed about the provided
 * software.
 *
 * @file main.c
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

/******
 * HEADERS
 ******/
#include "ISCA_CAN_API.h"


/******
 * DEFINITIONS
 ******/
#define CAN_TX           (0U)
#define CAN_RX           (1U)
#define CAN0_ROLE        CAN_TX   // 0:TX, 1:RX
#define CAN0_BASEADDR    (0xA0000000U)
#define RX_QUEUE_SIZE    (15U)


/******
 * VARIABLES
 ******/
static can_ctrl_s can_ctrl;
static can_frame_s can_frame;


/******
 * FUNCTIONS DECLARATION
 ******/
int config_can(can_ctrl_s *CanInstancePtr);
void can_example(can_ctrl_s *CanInstancePtr, can_frame_s *CanFrm);

/******
 * MAIN
 ******/
int main(void) {

	int status;

	/*
	 * configure CAN controller
	 */
	status = config_can(&can_ctrl);
	if ( status != ISCA_CAN_OK) {
		return ISCA_CAN_ERROR;
	}

	/*
	 * Run application
	 */
	can_example(&can_ctrl, &can_frame);

	return 0;
}

/******
 * FUNCTIONS DEFINITION
 ******/
int config_can(can_ctrl_s *CanInstancePtr) {
	/**
	 *  Initialize can structure
	 *  - Operate at 1000kbp (if input clock 100MHz)
	 *  - Drop any frame if header MSB is equal to '0'
	 *  - Enable interrupts
	 *  - Enable extended frame format (29-bit header) if CAN_MODE=CAN_2B
	 **/
	CanInstancePtr->addr          = CAN0_BASEADDR;
	CanInstancePtr->brp           = 9U;
	CanInstancePtr->tseg1         = 1U;
	CanInstancePtr->tseg2         = 1U;
	CanInstancePtr->sjw           = 1U;
	CanInstancePtr->irqs_en.rx    = CAN_IRQ_ON;  /// Enable RX interrupt
	CanInstancePtr->irqs_en.tx    = CAN_IRQ_OFF; /// Disable TX interrupt
#if !(CAN_MODE == CAN_2B)
	CanInstancePtr->mask          = 0x3FF;
	CanInstancePtr->code          = 0x400;
	CanInstancePtr->irqs_en.err0  = IRQ_ON; /// Enable RX FIFO overrun interrupt
	CanInstancePtr->irqs_en.err1  = IRQ_ON; /// Enable Error interrupt
#else
	CanInstancePtr->frm_md        = CAN_FRAME_EXT;
	CanInstancePtr->mask          = 0x0FFFFFFF;  // check only ID[28]
	CanInstancePtr->code          = 0x10000000;  // check if ID[28]='1'
	CanInstancePtr->irqs_en.err0  = CAN_IRQ_ON; /// Enable Error warning interrupt
	CanInstancePtr->irqs_en.err1  = CAN_IRQ_ON; /// Enable RX FIFO overrun interrupt
	CanInstancePtr->irqs_en.err2  = CAN_IRQ_ON; /// Enable Error passive IRQ interrupt
	CanInstancePtr->irqs_en.err3  = CAN_IRQ_ON; /// Enable Arbitration lost interrupt
	CanInstancePtr->irqs_en.err4  = CAN_IRQ_ON; /// Enable Bus error interrupt
#endif // !(CAN_MODE == CAN_2B)

#if !(APPRISE_MODE==APP_IRQ)
	CanInstancePtr->irq         = CAN_IRQ_OFF;
#else
	CanInstancePtr->irq         = CAN_IRQ_ON;
#endif // !(APPRISE_MODE==APP_IRQ)


	/*Initialize the controller and exit*/
	return lbr_isca_can_init(CanInstancePtr, RX_QUEUE_SIZE);
}

void can_example(can_ctrl_s *CanInstancePtr, can_frame_s *CanFrm) {
#if !(CAN0_ROLE==CAN_RX)
	/*TX*/
	CanFrm->ID      = 0x10AB0003;
#if (CAN_MODE == CAN_2B)
	CanFrm->IDE     = CAN_FRAME_EXT;
#endif // (CAN_MODE == CAN_2B)
	CanFrm->DLC     = 2U;
	CanFrm->DATA[0] = 0xAA;
	CanFrm->DATA[1] = 0xBB;
	lbr_isca_can_transmit_pkt(CanInstancePtr, CanFrm);

#else
	/*RX*/
	lbr_isca_can_receive_pkt(CanInstancePtr, CanFrm);
#endif // !(CAN0_ROLE==RX)
}
