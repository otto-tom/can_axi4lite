/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA_CAN HW driver
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : ISCA_CAN.h
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2017-01-27
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
 * @file ISCA_CAN.h
 *
 * @brief Provides functions to communicate with the CAN controller
 *
 * @version 1.0
 *
 * @author Othon Tomoutzoglou
 *
 * Contact: <otto_sta@hotmail.com>
 *
 */

#ifndef ISCA_CAN_H
#define ISCA_CAN_H

/******
 * HEADERS
 ******/
#include "ISCA_IO.h"
#include "ISCA_CAN_CFG.h"

/******
 * DEFINITIONS
 ******/

/*-----
 * CAN FRAME STRUCT
 *----*/

#if !(CAN_MODE == CAN_2B)
/// Basic mode CAN frame description structure
typedef struct can_frame_s {
	uint16_t DLC;    /*!<Frame data length, multiple of byte*/
	uint16_t RTR;    /*!<1: Remote frame request*/
	uint8_t DATA[8]; /*!<Frame data, i.e., payload*/
	uint32_t ID;     /*!<Frame Identification*/
} can_frame_s;
#else
/// Extended mode CAN frame description structure
typedef struct can_frame_s {
	uint16_t DLC;    /*!<Frame data length, multiple of byte*/
	uint16_t RTR;    /*!<1: Remote frame request*/
	uint8_t DATA[8]; /*!<Frame data, i.e., payload*/
	uint32_t ID;     /*!<Frame Identification*/
	uint32_t IDE;    /*!<Frame format, \ref CAN_FRAME_EXT or \ref CAN_FRAME_STD)*/
} can_frame_s;
#endif

/*-----
 * CAN CONTROLLER STRUCT
 *----*/

#if !(CAN_MODE == CAN_2B)

/// CAN controller IRQ enable union
typedef union can_irq_en_u {
    uint8_t rx;       /*!<RX IRQ; 0: disable / 1: enable*/
    uint8_t tx;       /*!<TX IRQ; 0: disable / 1: enable*/
    uint8_t err0;     /*!<RX FIFO overrun IRQ; 0: disable / 1: enable*/
    uint8_t err1;     /*!<Error IRQ; 0: disable / 1: enable*/
} irq_en;

/// CAN controller description structure
typedef struct can_controller_s {
	size_t addr;          /*!<Controller's physical address*/
	uint8_t brp;          /*!<Baud rate prescalar*/
	uint8_t tseg1;        /*!<Timming segment 1*/
	uint8_t tseg2;        /*!<Timming segment 2*/
	uint8_t sjw;          /*!<Synchronization jump width*/
	uint32_t mask;        /*!<Filter mask*/
	uint32_t code;        /*!<Filter code*/
	can_frame_s* q_ptr;   /*!<RX queue pointer*/
	int8_t q_id;          /*!<RX queue identifier*/
	uint8_t q_size;       /*!<RX queue size in frames*/
	uint8_t irq;          /*!<IRQ mode, \ref CAN_IRQ_ON or \ref CAN_IRQ_OFF*/
	irq_en  irqs_en; ;    /*!<IRQs enable union*/
	void (*InterruptCallback) (void *); /*!<Interupt callback pointer*/
} can_ctrl_s;
#else
/// CAN controller IRQ enable union
typedef union can_irq_en_u {
    uint8_t rx;       /*!<RX IRQ; 0: disable / 1: enable*/
    uint8_t tx;       /*!<TX IRQ; 0: disable / 1: enable*/
    uint8_t err0;     /*!<Error IRQ; 0: disable / 1: enable*/
    uint8_t err1;     /*!<RX FIFO overrun IRQ; 0: disable / 1: enable*/
    uint8_t err2;     /*!<Passive error IRQ; 0: disable / 1: enable*/
    uint8_t err3;     /*!<Arbitration lost IRQ; 0: disable / 1: enable*/
    uint8_t err4;     /*!<Bus error IRQ; 0: disable / 1: enable*/
} irq_en;

/// CAN controller description structure
typedef struct can_controller_s {
	size_t addr;          /*!<Controller's physical address*/
	uint8_t brp;          /*!<Baud rate prescalar*/
	uint8_t tseg1;        /*!<Timming segment 1*/
	uint8_t tseg2;        /*!<Timming segment 2*/
	uint8_t sjw;          /*!<Synchronization jump width*/
	uint32_t mask;        /*!<Filter mask*/
	uint32_t code;        /*!<Filter code*/
	can_frame_s* q_ptr;   /*!<RX queue pointer*/
	uint8_t frm_md;       /*!<Frame mode: \ref CAN_FRAME_EXT / \ref CAN_FRAME_STD*/
	int8_t q_id;          /*!<RX queue identifier*/
	uint8_t q_size;       /*!<RX queue size in frames*/
	uint8_t irq;          /*!<IRQ mode, \ref CAN_IRQ_ON or \ref CAN_IRQ_OFF*/
	irq_en  irqs_en;      /*!<IRQs enable union*/
	void (*InterruptHandler) (void *); /*!<Interupt callback pointer*/
} can_ctrl_s;
#endif // !(CAN_MODE == CAN_2B)

/*-----
 * CAN FUNCT RETURN
 *----*/
#define ISCA_CAN_OK                ( 0)   /*!<CAN generic success return value*/
#define ISCA_CAN_BUSY              (-1)   /*!<CAN controller return value*/
#define ISCA_CAN_INV_REQ_TYPE      (-2)   /*!<CAN invalid request type return value*/
#define ISCA_CAN_RX_FIFO_EMPTY     (-3)   /*!<CAN RX FIFO empty return value*/
#define ISCA_CAN_INV_ID_TYPE       (-4)   /*!<CAN invalid \ref can_frame_s ID type return value*/
#define ISCA_CAN_INV_DLC           (-5)   /*!<CAN invalid \ref can_frame_s DLC type return value*/
#define ISCA_CAN_INV_RESET_MODE    (-6)   /*!<CAN invalid reset mode @see ISCA_CAN_MODE_RESET_OFF @see ISCA_CAN_MODE_RESET_ON*/
#define ISCA_CAN_QUEUES_OCCUPIED   (-7)   /*!<CAN all SW RX queues occupied*/
#define ISCA_CAN_INV_RST_MODE      (-8)   /*!<CAN invalid reset mode ordered*/
#define ISCA_CAN_ERROR             (-9)   /*!<CAN error*/

/*-----
 * CAN REQUEST TYPE
 *----*/
#define CAN_REQ_BLOCKING           (0U)   /*!<Keep polling the controller until request satisfied*/
#define CAN_REQ_NONBLOCKING        (1U)   /*!<Send request to the controller only once*/

/*-----
 * CAN SWITCH RESET MODE
 *----*/
#define ISCA_CAN_MODE_RESET_OFF    (0U)   /*!<Switch CAN SW controlled reset mode off*/
#define ISCA_CAN_MODE_RESET_ON     (1U)   /*!<Switch CAN SW controlled reset mode on*/

/*-----
 * CAN IRQ SWITCH
 *----*/
#define CAN_IRQ_OFF                (0U)  /*!<Switch CAN interrupts off*/
#define CAN_IRQ_ON                 (1U)  /*!<Switch CAN interrupts on*/


#if (CAN_MODE == CAN_2B)
/*-----
 * CAN 2B FRAME TYPE
 *----*/
#define CAN_FRAME_STD              (0U)  /*!<CAN extended mode, basic frame (11-bit header)*/
#define CAN_FRAME_EXT              (1U)  /*!<CAN extended mode, extended frame (29-bit header)*/
#endif

/******
 * CAN DRIVER FUNCTIONS
 ******/
int isca_can_init(can_ctrl_s *can_ctrl);

int isca_can_transmit_frame(can_ctrl_s *can_ctrl, can_frame_s *tx_frame, uint8_t req_type);

int isca_can_receive_frame(can_ctrl_s *can_ctrl, can_frame_s *rx_frame, uint8_t req_type);

void isca_can_set_filter(can_ctrl_s *can_ctrl);

int isca_can_switch_mode(can_ctrl_s *can_ctrl, uint8_t reset_mode);

#endif /* ISCA_CAN_H */
