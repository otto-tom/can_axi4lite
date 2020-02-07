/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA_CAN interrupt callback
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : ISCA_CAN_IRQ.c
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2017-09-09
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
 * |-- 2018-10-22  1.0      Otoo    Created
 * \#-----------------------------------------------------------------------------\
 * *******************************************************************************/

/**
 * @file ISCA_CAN_IRQ.c
 *
 * @brief CAN interrupt routines and callback
 *
 * @author Othon Tomoutzoglou
 *
 * Contact: <otto_sta@hotmail.com>
 *
 * @version 1.0
 *
 * @pre Fill the "USER CODE" sections with device
 *  specific code, as described
 */

/******
 * HEADERS
 ******/
#include "ISCA_CAN_API.h"
#include "ISCA_QUEUE_INDEXER.h"
#include <stdio.h>

/******
 * DEFINITIONS
 ******/
#define CAN_IRQS_STATUS_REG   (12U) /*!<IRQs status register offset*/
#define CAN_IRQ_CLEAR         (04U) /*!<IRQs clear register offset*/

/*
 * IRQ type <-> register active bit
 */
#if (CAN_MODE == CAN_2B)
#define BUS_ERROR        (0x80U)  /*!<CAN_IRQS_STATUS_REG[7] -> bus_error_irq_en*/
#define ARB_LOST         (0x40U)  /*!<CAN_IRQS_STATUS_REG[6] -> arbitration_lost_irq_en*/
#define ERR_P_IRQ        (0x20U)  /*!<CAN_IRQS_STATUS_REG[5] -> error_passive_irq_en*/
#endif // (CAN_MODE == CAN_2B)
#define DATA_OVRRUN      (0x08U)  /*!<CAN_IRQS_STATUS_REG[3] -> data_overrun_irq_en_ext*/
#define ERR_WARN         (0x04U)  /*!<CAN_IRQS_STATUS_REG[2] -> error_warning_irq_en_ext*/
#define TX_OK            (0x02U)  /*!<CAN_IRQS_STATUS_REG[1] -> transmit_irq_en_ext*/
#define RX_OK            (0x01U)  /*!<CAN_IRQS_STATUS_REG[0] -> receive_irq_en_ext*/

/******
 * PRIVATE FUNCTIONS DECLARATION
 ******/
int  isca_can_receive_pkt_irq(can_ctrl_s *can_ctrl);
inline uint8_t isca_can_ack_irq_generic(can_ctrl_s *can_ctrl) __attribute__ ((always_inline));
inline void isca_can_ack_irq_reboot(can_ctrl_s *can_ctrl) __attribute__ ((always_inline));

/******
 * FUNCTIONS DEFINITION
 ******/
/**
 * @pre        Fill the "USER CODE" sections with device
 *             specific code, as described
 * @brief      CAN controller interrupt callback
 * @param[in]  can_ctrl CAN controllers instance pointer
 * @return     None
 * */
void ISCA_CAN_IntrHandler(void *can_ctrl) {

	uint8_t irq_rd;
	can_ctrl_s *can_ctrl_l;

	/* USER CODE
	 * Place code to apply the proper negotiations with the Interrupt controller
	 * CAN interrupt is connected with, e.g. disable IRQ's or acknowledge it
	 */

	/* END USER_CODE*/

	// Cast & link can controller
	can_ctrl_l = (can_ctrl_s *) can_ctrl;
	/* Reading the IRQ status register*/
	irq_rd = ISCA_FPGA_Read8Bit(can_ctrl_l->addr+CAN_IRQS_STATUS_REG);

	/*Check read register's every bit*/
	if ( (irq_rd&RX_OK) == RX_OK ) {
		isca_can_receive_pkt_irq(can_ctrl_l);
	} /*IRQ: RX*/
	else if ( (irq_rd&TX_OK) == TX_OK ) {
		isca_can_ack_irq_generic(can_ctrl_l);
	} /*IRQ: TX*/
	else if ( (irq_rd&ERR_WARN) == ERR_WARN ) {
		__COUT("\n\rError IRQ!\n\r");
		isca_can_ack_irq_reboot(can_ctrl_l);
	} /*IRQ: Error*/
	else if ( (irq_rd&DATA_OVRRUN) == DATA_OVRRUN ) {
		__COUT("\n\rRX FIFO overrun IRQ!\n\r");
		/*Maybe start reading some frames out of RX FIFO*/
		isca_can_ack_irq_generic(can_ctrl_l);
	} /*IRQ: RX FIFO IRQ*/
#if (CAN_MODE == CAN_2B)
	else if ( (irq_rd&ERR_P_IRQ) == ERR_P_IRQ ) {
		__COUT("\n\rError passive IRQ!\n\r");
		isca_can_ack_irq_reboot(can_ctrl_l);
	} /*IRQ: Error passive IRQ*/
	else if ( (irq_rd&ARB_LOST) == ARB_LOST ) {
		__COUT("\n\rArbitration lost IRQ!\n\r");
		isca_can_ack_irq_reboot(can_ctrl_l);
	} /*IRQ: Arbitration lost*/
	else if ( (irq_rd&BUS_ERROR) == BUS_ERROR ) {
		__COUT("\n\rBus error IRQ!\n\r");
		isca_can_ack_irq_reboot(can_ctrl_l);
	} /*IRQ: Bus error*/
#endif // !(CAN_MODE == CAN_2A)

	/* USER CODE
	 * Place code to apply the proper negotiations with the Interrupt controller
	 * CAN interrupt is connected with, e.g. re-enable IRQ's
	 */

	/* END USER_CODE*/

}

/******
 * PRIVATE FUNCTIONS IMPLEMENTATION
 ******/

/*RX interrupt routine*/
int isca_can_receive_pkt_irq(can_ctrl_s *can_ctrl) {

	volatile uint8_t _ack;
	uint8_t _cpy;
	int remain_packets;
	can_frame_s rx_frame;
	uint8_t queue_wr_index;

	int q_status;

	/*Receive frame, this also releases the RX buffer*/
	remain_packets = isca_can_receive_frame(can_ctrl, &rx_frame, CAN_REQ_NONBLOCKING);

	/*Acknowledge the interrupt*/
	_ack = ISCA_FPGA_Read8Bit(can_ctrl->addr+CAN_IRQ_CLEAR);
	/*prevent the compiler from removing the above line; the dummy way*/
	memmove(&_cpy, (void *)&_ack, sizeof(uint8_t));

	if (remain_packets==ISCA_CAN_RX_FIFO_EMPTY) return 0;

	/*Ask the queue for write pointer*/
	q_status = isca_queue_wr_ptr(can_ctrl->q_id, &queue_wr_index);

	//if ( __builtin_expect(q_status==DQ_FULL, 0) ) {
	if ( q_status==DQ_FULL ) {
		__COUT("\n\rCan queue full, frame rejected");
		return remain_packets;
	} /*Queue is full*/

	/*Write frame to the queue*/
	memmove(&can_ctrl->q_ptr[queue_wr_index], &rx_frame, sizeof(can_frame_s));

	return remain_packets;

}

/*Generic interrupt routine*/
uint8_t isca_can_ack_irq_generic(can_ctrl_s *can_ctrl) {

	volatile uint8_t _tmp;

	// acknowledge IRQ.
	_tmp = ISCA_FPGA_Read8Bit(can_ctrl->addr+CAN_IRQ_CLEAR);

	return _tmp;

}

/*Errors interrupt routine*/
/*!<@pre Fill the "USER CODE" sections with device
 * specific code, as described
 */
void isca_can_ack_irq_reboot(can_ctrl_s *can_ctrl) {

	// acknowledge IRQ.
	isca_can_ack_irq_generic(can_ctrl);

	// Soft reset CAN controller
	isca_can_switch_mode(can_ctrl, ISCA_CAN_MODE_RESET_ON);
	isca_can_switch_mode(can_ctrl, ISCA_CAN_MODE_RESET_OFF);

	/*
	 * Place code to reset the FPGA or the entire system
	 * If such functionality not available you can:
	 * A. call exit(0) from stdlib.h
	 * B. Soft reset the can controller with
	 *    isca_can_switch_mode(ISCA_CAN_MODE_RESET_ON);
	 *    isca_can_switch_mode(ISCA_CAN_MODE_RESET_OFF);
	 */

	/*USER CODE*/
	/*END USER CODE*/

}
