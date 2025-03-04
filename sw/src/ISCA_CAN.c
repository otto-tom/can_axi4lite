/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA_CAN HW driver
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : ISCA_CAN.c
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2017-01-27
 * |-- Last update: 2019-09-15
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
 * |-- 2018-09-15  1.0      Otto    Created
 * \#-----------------------------------------------------------------------------\
 * *******************************************************************************/

/**
 * @file ISCA_CAN.c
 *
 * @brief Provides functions to communicate with the CAN controller
 *
 * @author Othon Tomoutzoglou
 *
 * Contact: <otto_sta@hotmail.com>
 *
 * @version 1.0
 *
 * @todo Implemented filtering modes:
 * - Extended mode. ID match for standard format (11-bit ID). Using double filter.
 * - Extended mode. ID match for extended format (29-bit ID). Using double filter.
 *
 */

/******
 * HEADERS
 ******/
#include "ISCA_CAN.h"

/******
 * DEFINITIONS
 ******/

/**
 * @brief CAN controller base address fixed offset definition
 * */
#define CAN_BASE_OFFSET       (0U)

/* CAN controller registers offset */

#if !(CAN_MODE==CAN_2B)
/**
 * @brief Clock configuration register0
 * Layout (0 disable, 1 enable):
 * @code
 * CAN_MODE0_REG[7:5] -> reserved,
 * CAN_MODE0_REG[4:4] -> overrun_irq_en_basic,
 * CAN_MODE0_REG[3:3] -> error_irq_en_basic,
 * CAN_MODE0_REG[2:2] -> transmit_irq_en_basic,
 * CAN_MODE0_REG[1:1] -> receive_irq_en_basic,
 * CAN_MODE0_REG[0:0] -> reset_mode
 * @endcode
 * */
#define CAN_MODE0_REG         (CAN_BASE_OFFSET)
#else
/**
 * @brief Mode configuration register0
 *
 * Layout (0 disable, 1 enable):
 * @code
 * CAN_MODE0_REG[7:4] -> reserved,
 * CAN_MODE0_REG[3:3] -> single_acceptance_filter_mode,
 * CAN_MODE0_REG[2:2] -> self_test_mode,
 * CAN_MODE0_REG[1:1] -> listen_only_mode,
 * CAN_MODE0_REG[0:0] -> reset_mode
 * @endcode
 * */
#define CAN_MODE0_REG         (CAN_BASE_OFFSET)
#endif // !(CAN_MODE==CAN_2B)

/**
 * @brief Filter Mode enable register
 *
 * Layout (0 disable, 1 enable):
 * @code
 * CAN_FILTER_MODE_REG[7:1] -> reserved
 * CAN_FILTER_MODE_REG[0:0] -> filter_mode
 * @endcode
 * */
#define CAN_FILTER_MODE_REG   (CAN_BASE_OFFSET+12U)

#if (CAN_MODE == CAN_2B)

/**
 * @brief IRQs enable register0 (Write-only)
 *
 * Layout (0 disable, 1 enable):
 * @code
 * CAN_IRQS_EN_REG[7] -> bus_error_irq_en
 * CAN_IRQS_EN_REG[6] -> arbitration_lost_irq_en
 * CAN_IRQS_EN_REG[5] -> error_passive_irq_en
 * CAN_IRQS_EN_REG[4] -> reserved
 * CAN_IRQS_EN_REG[3] -> data_overrun_irq_en_ext
 * CAN_IRQS_EN_REG[2] -> error_warning_irq_en_ext
 * CAN_IRQS_EN_REG[1] -> transmit_irq_en_ext
 * CAN_IRQS_EN_REG[0] -> receive_irq_en_ext
 * @endcode
 * */
#define CAN_IRQS_EN_REG       (CAN_BASE_OFFSET+16U)

/**
 * @brief IRQs status register0 (Read-only)
 *
 * Layout (0 inactive, 1 active):
 * @code
 * CAN_IRQS_STATUS_REG[7] -> bus_error_irq_en
 * CAN_IRQS_STATUS_REG[6] -> arbitration_lost_irq_en
 * CAN_IRQS_STATUS_REG[5] -> error_passive_irq_en
 * CAN_IRQS_STATUS_REG[4] -> reserved
 * CAN_IRQS_STATUS_REG[3] -> data_overrun_irq_en_ext
 * CAN_IRQS_STATUS_REG[2] -> error_warning_irq_en_ext
 * CAN_IRQS_STATUS_REG[1] -> transmit_irq_en_ext
 * CAN_IRQS_STATUS_REG[0] -> receive_irq_en_ext
 * @endcode
 * */
#define CAN_IRQS_STATUS_REG   (CAN_BASE_OFFSET+12U)

#endif // (CAN_MODE == CAN_2B)

/**
 * @brief CAN clear IRQ register (Read-only)
 *
 * A read request to the CAN clear IRQ register clears the interrupt
 * at the CAN controller
 * */
//Defined at ISCA_CAN_IRQ.c
//#define CAN_IRQ_CLEAR         (CAN_BASE_ADDRESS+4U)

/**
 * @brief CAN command TX register (Write-Only)
 *
 * Layout:
 * @code
 * CAN_COMMAND_TX_REG[7:1] -> reserved
 * CAN_COMMAND_TX_REG[4]   -> trigger TX (1 enable)
 * @endcode
 * */
#define CAN_COMMAND_TX_REG    (CAN_BASE_OFFSET+4U)

/**
 * @brief CAN command RX register  (Write-Only)
 *
 * Layout:
 * @code
 * CAN_COMMAND_RX_REG[7:4] -> reserved
 * CAN_COMMAND_RX_REG[2]   -> release RX buffer, decrease RX counter (1 enable)
 * CAN_COMMAND_RX_REG[3]   -> clear RX FIFO overrun (1 enable)
 * CAN_COMMAND_RX_REG[1:0] -> reserved
 * @endcode
 * */
#define CAN_COMMAND_RX_REG    (CAN_BASE_OFFSET+8U)

/**
 * @brief CAN status register (Read-only)
 *
 * Layout:
 * @code
 * CAN_STATUS_REG[0] -> receive buffer status
 * CAN_STATUS_REG[1] -> overrun status (1:overrun)
 * CAN_STATUS_REG[2] -> transmit buffer status(1:idle, 0:busy)
 * CAN_STATUS_REG[3] -> transmission complete status
 * CAN_STATUS_REG[4] -> receive status
 * CAN_STATUS_REG[5] -> transmit status
 * CAN_STATUS_REG[6] -> error status
 * CAN_STATUS_REG[7] -> node buss off
 * @endcode
 * */
#define CAN_STATUS_REG        (CAN_BASE_OFFSET+8U)


#if !(CAN_MODE == CAN_2B)
/**
 * \if CAN_MODE == CAN_2A
 * @brief ID CODE register0
 *
 * Layout:
 * @code
 * CAN_CODE_REG0[7:0] -> CODE(ID)[10:3]
 * @endcode
 * \endif
 * */
#define CAN_CODE_REG0         (CAN_BASE_OFFSET+16U)

/**
 * @brief ID CODE register1
 *
 * Layout:
 * @code
 * CAN_CODE_REG1[2:0] -> CODE(ID)[2:0]
 * @endcode
 * */
#define CAN_CODE_REG1         (CAN_BASE_OFFSET+68U)

/**
 * @brief ID MASK register0
 *
 * Layout:
 * @code
 * CAN_MASK_REG0[7:0] -> CODE(ID)[10:3]
 * @endcode
 * */
#define CAN_MASK_REG0         (CAN_BASE_OFFSET+20U)

/**
 * @brief ID MASK register1
 *
 * Layout:
 * @code
 * CAN_MASK_REG1[2:0] -> CODE(ID)[2:0]
 * @endcode
 * */
#define CAN_MASK_REG1         (CAN_BASE_OFFSET+84U)

#else

/**
 * @brief CAN code register 0
 *
 * Layout \b extended frame single filtering:
 * @code
 * CAN_CODE_REG0[0] -> id[21]  CAN_CODE_REG0[1] -> id[22]
 * CAN_CODE_REG0[2] -> id[23]  CAN_CODE_REG0[3] -> id[24]
 * CAN_CODE_REG0[4] -> id[25]  CAN_CODE_REG0[5] -> id[26]
 * CAN_CODE_REG0[6] -> id[27]  CAN_CODE_REG0[7] -> id[28]
 * @endcode
 *
 * Layout \b standard frame, single filtering:
 * @code
 * CAN_CODE_REG0[0] -> id[3]  CAN_CODE_REG0[1] -> id[4]
 * CAN_CODE_REG0[2] -> id[5]  CAN_CODE_REG0[3] -> id[6]
 * CAN_CODE_REG0[4] -> id[7]  CAN_CODE_REG0[5] -> id[8]
 * CAN_CODE_REG0[6] -> id[9]  CAN_CODE_REG0[7] -> id[10]
 * @endcode
 *
 * @see CAN_MODE0_REG
 * */
#define CAN_CODE_REG0         (CAN_BASE_OFFSET+64U)

/**
 * @brief CAN code register 1
 *
 * Layout \b extended frame single filtering:
 * @code
 * CAN_CODE_REG1[0] -> id[13]  CAN_CODE_REG1[1] -> id[14]
 * CAN_CODE_REG1[2] -> id[15]  CAN_CODE_REG1[3] -> id[16]
 * CAN_CODE_REG1[4] -> id[17]  CAN_CODE_REG1[5] -> id[18]
 * CAN_CODE_REG1[6] -> id[19]  CAN_CODE_REG1[7] -> id[20]
 * @endcode
 *
 * Layout \b standard frame single filtering:
 * @code
 * CAN_CODE_REG1[0] -> none   CAN_CODE_REG1[1] -> none
 * CAN_CODE_REG1[2] -> none   CAN_CODE_REG1[3] -> none
 * CAN_CODE_REG1[4] -> rtr    CAN_CODE_REG1[5] -> id[0]
 * CAN_CODE_REG1[6] -> id[1]  CAN_CODE_REG1[7] -> id[2]
 * @endcode
 *
 * @see CAN_MODE0_REG
 * */
#define CAN_CODE_REG1         (CAN_BASE_OFFSET+68U)

/**
 * @brief CAN code register 2
 *
 * Layout \b extended frame single filtering:
 * @code
 * CAN_CODE_REG2[0] -> id[5]   CAN_CODE_REG2[1] -> id[6]
 * CAN_CODE_REG2[2] -> id[7]   CAN_CODE_REG2[3] -> id[8]
 * CAN_CODE_REG2[4] -> id[9]   CAN_CODE_REG2[5] -> id[10]
 * CAN_CODE_REG2[6] -> id[11]  CAN_CODE_REG2[7] -> id[12]
 * @endcode
 *
 * Layout \b standard frame single filtering:
 * @code
 * CAN_CODE_REG2[0] -> data0[0] , CAN_CODE_REG2[1] -> data0[1]
 * CAN_CODE_REG2[2] -> data0[2] , CAN_CODE_REG2[3] -> data0[3]
 * CAN_CODE_REG2[4] -> data0[4] , CAN_CODE_REG2[5] -> data0[5]
 * CAN_CODE_REG2[6] -> data0[6] , CAN_CODE_REG2[7] -> data0[7]
 * @endcode
 *
 * @see CAN_MODE0_REG
 * */
#define CAN_CODE_REG2         (CAN_BASE_OFFSET+72U)

/**
 * @brief CAN code register 3
 *
 * Layout \b extended frame single filtering:
 * @code
 * CAN_CODE_REG3[0] -> none  , CAN_CODE_REG3[1] -> none
 * CAN_CODE_REG3[2] -> rtr   , CAN_CODE_REG3[3] -> id[0]
 * CAN_CODE_REG3[4] -> id[1] , CAN_CODE_REG3[5] -> id[2]
 * CAN_CODE_REG3[6] -> id[3] , CAN_CODE_REG3[7] -> id[4]
 * @endcode
 *
 * Layout \b standard frame single filtering:
 * @code
 * CAN_CODE_REG3[0] -> data1[0]  CAN_CODE_REG3[1] -> data1[1]
 * CAN_CODE_REG3[2] -> data1[2]  CAN_CODE_REG3[3] -> data1[3]
 * CAN_CODE_REG3[4] -> data1[4]  CAN_CODE_REG3[5] -> data1[5]
 * CAN_CODE_REG3[6] -> data1[6]  CAN_CODE_REG3[7] -> data1[7]
 * @endcode
 * @see CAN_MODE0_REG
 * */
#define CAN_CODE_REG3         (CAN_BASE_OFFSET+76U)

/**
 * @brief CAN mask register 0
 *
 * Layout \b extended frame single filtering:
 * @code
 * CAN_MASK_REG0[0] -> id[21]  CAN_MASK_REG0[1] -> id[22]
 * CAN_MASK_REG0[2] -> id[23]  CAN_MASK_REG0[3] -> id[24]
 * CAN_MASK_REG0[4] -> id[25]  CAN_MASK_REG0[5] -> id[26]
 * CAN_MASK_REG0[6] -> id[27]  CAN_MASK_REG0[7] -> id[28]
 * @endcode
 *
 * Layout \b standard frame single filtering:
 * @code
 * CAN_MASK_REG0[0] -> id[3]  CAN_MASK_REG0[1] -> id[4]
 * CAN_MASK_REG0[2] -> id[5]  CAN_MASK_REG0[3] -> id[6]
 * CAN_MASK_REG0[4] -> id[7]  CAN_MASK_REG0[5] -> id[8]
 * CAN_MASK_REG0[6] -> id[9]  CAN_MASK_REG0[7] -> id[10]
 * @endcode
 * @see CAN_MODE0_REG
 * */
#define CAN_MASK_REG0         (CAN_BASE_OFFSET+80U)

/**
 * @brief CAN mask register 1
 *
 * Layout \b extended frame single filtering:
 * @code
 * CAN_MASK_REG1[0] -> id[13]  CAN_MASK_REG1[1] -> id[14]
 * CAN_MASK_REG1[2] -> id[15]  CAN_MASK_REG1[3] -> id[16]
 * CAN_MASK_REG1[4] -> id[17]  CAN_MASK_REG1[5] -> id[18]
 * CAN_MASK_REG1[6] -> id[19]  CAN_MASK_REG1[7] -> id[20]
 * @endcode
 *
 * Layout \b standard frame single filtering:
 * @code
 * CAN_MASK_REG1[0] -> none   CAN_MASK_REG1[1] -> none
 * CAN_MASK_REG1[2] -> none   CAN_MASK_REG1[3] -> none
 * CAN_MASK_REG1[4] -> rtr    CAN_MASK_REG1[5] -> id[0]
 * CAN_MASK_REG1[6] -> id[1]  CAN_MASK_REG1[7] -> id[2]
 * @endcode
 * @see CAN_MODE0_REG
 * */
#define CAN_MASK_REG1         (CAN_BASE_OFFSET+84U)

/**
 * @brief CAN mask register 2
 *
 * Layout \b extended frame single filtering:
 * @code
 * CAN_MASK_REG2[0] -> id[ 5]  CAN_MASK_REG2[1] -> id[ 6]
 * CAN_MASK_REG2[2] -> id[ 7]  CAN_MASK_REG2[3] -> id[ 8]
 * CAN_MASK_REG2[4] -> id[ 9]  CAN_MASK_REG2[5] -> id[10]
 * CAN_MASK_REG2[6] -> id[11]  CAN_MASK_REG2[7] -> id[12]
 * @endcode
 *
 * Layout \b standard frame single filtering:
 * @code
 * CAN_MASK_REG2[0] -> data0[0]  CAN_MASK_REG2[1] -> data0[1]
 * CAN_MASK_REG2[2] -> data0[2]  CAN_MASK_REG2[3] -> data0[3]
 * CAN_MASK_REG2[4] -> data0[4]  CAN_MASK_REG2[5] -> data0[5]
 * CAN_MASK_REG2[6] -> data0[6]  CAN_MASK_REG2[7] -> data0[7]
 * @endcode
 * @see CAN_MODE0_REG
 * */
#define CAN_MASK_REG2         (CAN_BASE_OFFSET+88U)

/**
 * @brief CAN mask register 3
 *
 * Layout \b extended frame single filtering:
 * @code
 * CAN_MASK_REG3[0] -> none   CAN_MASK_REG3[1] -> none
 * CAN_MASK_REG3[2] -> rtr    CAN_MASK_REG3[3] -> id[0]
 * CAN_MASK_REG3[4] -> id[1]  CAN_MASK_REG3[5] -> id[2]
 * CAN_MASK_REG3[6] -> id[3]  CAN_MASK_REG3[7] -> id[4]
 * @endcode
 *
 * Layout \b standard frame single filtering:
 * @code
 * CAN_MASK_REG3[0] -> data1[0]  CAN_MASK_REG3[1] -> data1[1]
 * CAN_MASK_REG3[2] -> data1[2]  CAN_MASK_REG3[3] -> data1[3]
 * CAN_MASK_REG3[4] -> data1[4]  CAN_MASK_REG3[5] -> data1[5]
 * CAN_MASK_REG3[6] -> data1[6]  CAN_MASK_REG3[7] -> data1[7]
 * @endcode
 * @see CAN_MODE0_REG
 * */
#define CAN_MASK_REG3         (CAN_BASE_OFFSET+92U)

#endif // !(CAN_MODE == CAN_2B)

/**
 * @brief Bus timing register 0
 *
 * Layout:
 * @code
 * CAN_BTR0_REG[7:6] -> Synch Jump Width: (value+1)
 * CAN_BTR0_REG[5:0] -> Baud rate prescaler: 2*(value+1)
 * @endcode
 * */
#define CAN_BTR0_REG          (CAN_BASE_OFFSET+24U)

/**
 * @brief Bus timing register 1
 *
 * Layout:
 * @code
 * CAN_BTR1_REG[7:7] -> Triple sampling (0 disable, 1 enable)
 * CAN_BTR1_REG[6:4] -> TSEG1: (value+1)
 * CAN_BTR1_REG[3:0] -> TSEG2: (value+1)
 * @endcode
 * */
#define CAN_BTR1_REG          (CAN_BASE_OFFSET+28U)

/**
 * @brief RX counter register
 *
 * Layout:
 * @code
 * CAN_RX_COUNTER_REG[6:0] -> unread frames
 * @endcode
 * */
#if !(CAN_MODE == CAN_2B)
#define CAN_RX_COUNTER_REG    (CAN_BASE_OFFSET+32U)
#else
#define CAN_RX_COUNTER_REG    (CAN_BASE_OFFSET+116U)
#endif // !(CAN_MODE == CAN_2B)

/**
 * @brief Acknowledge invalid RX register (Read-only)
 *
 * RX FIFO overrun occurred; reading \ref CAN_INVALID_RX_ACK_REG
 * acknowledges overrun. Layout:
 * @code
 * CAN_INVALID_RX_ACK_REG[7:1] -> reserved
 * CAN_INVALID_RX_ACK_REG[0:0] -> RX state ('1' valid)
 * @endcode
 * @note After acknowledging you should also decrease RX counter
 * */
#define CAN_INVALID_RX_ACK_REG    (CAN_BASE_OFFSET+4U)

#if !(CAN_MODE == CAN_2B)
/**
 * @brief TX buffer0 register, layout:
 * @code
 * CAN_TX0_REG[7:0] -> can_frame_s.ID[10:3]
 * @endcode
 * */
#define CAN_TX0_REG           (CAN_BASE_OFFSET+40U)

/**
 * @brief TX buffer1 register, layout:
 * @code
 * CAN_TX1_REG[7:5] -> can_frame_s.ID[2:0]
 * CAN_TX1_REG[4:4] -> can_frame_s.RTR
 * CAN_TX1_REG[3:0] -> can_frame_s.DLC
 * @endcode
 * */
#define CAN_TX1_REG           (CAN_BASE_OFFSET+44U)

/**
 * @brief TX buffer2 register, layout:
 * @code
 * CAN_TX2_REG[7:0] -> can_frame_s.DATA[0]
 * @endcode
 * */
#define CAN_TX2_REG           (CAN_BASE_OFFSET+48U)

/**
 * @brief TX buffer3 register, layout:
 * @code
 * CAN_TX3_REG[7:0] -> can_frame_s.DATA[1]
 * */
#define CAN_TX3_REG           (CAN_BASE_OFFSET+52U)

/**
 * @brief TX buffer4 register, layout:
 * @code
 * CAN_TX4_REG[7:0] -> can_frame_s.DATA[2]
 * @endcode
 * */
#define CAN_TX4_REG           (CAN_BASE_OFFSET+56U)

/**
 * @brief TX buffer5 register, layout:
 * @code
 * CAN_TX5_REG[7:0] -> can_frame_s.DATA[3]
 * @endcode
 * */
#define CAN_TX5_REG           (CAN_BASE_OFFSET+60U)

/**
 * @brief TX buffer6 register, layout:
 * @code
 * CAN_TX6_REG[7:0] -> can_frame_s.DATA[4]
 * @endcode
 * */
#define CAN_TX6_REG           (CAN_BASE_OFFSET+64U)

/**
 * @brief TX buffer7 register, layout:
 * @code
 * CAN_TX7_REG[7:0] -> can_frame_s.DATA[5]
 * @endcode
 * */
#define CAN_TX7_REG           (CAN_BASE_OFFSET+68U)

/**
 * @brief TX buffer8 register, layout:
 * @code
 * CAN_TX8_REG[7:0] -> can_frame_s.DATA[6]
 * @endcode
 * */
#define CAN_TX8_REG           (CAN_BASE_OFFSET+72U)

/**
 * @brief TX buffer9 register, layout:
 * @code
 * CAN_TX9_REG[7:0] -> can_frame_s.DATA[7]
 * @endcode
 * */
#define CAN_TX9_REG           (CAN_BASE_OFFSET+76U)

/**
 * @brief RX buffer0 register, layout:
 * @code
 * CAN_RX0_REG[7:0] -> can_frame_s.ID[10:3]
 * @endcode
 * */
#define CAN_RX0_REG           (CAN_BASE_OFFSET+80U)

/**
 * @brief RX buffer1 register, layout:
 * @code
 * CAN_RX1_REG[7:5] -> ID[2:0]
 * CAN_RX1_REG[4:4] -> RTR
 * CAN_RX1_REG[3:0] -> DLC
 * @endcode
 * */
#define CAN_RX1_REG           (CAN_BASE_OFFSET+84U)

/**
 * @brief RX buffer2 register, layout:
 * @code
 * CAN_RX2_REG[7:0] -> can_frame_s.DATA[0]
 * @endcode
 * */
#define CAN_RX2_REG           (CAN_BASE_OFFSET+88U)

/**
 * @brief RX buffer3 register, layout:
 * @code
 * CAN_RX3_REG[7:0] -> can_frame_s.DATA[1]
 * @endcode
 * */
#define CAN_RX3_REG           (CAN_BASE_OFFSET+92U)

/**
 * @brief RX buffer4 register, layout:
 * @code
 * CAN_RX4_REG[7:0] -> can_frame_s.DATA[2]
 * @endcode
 * */
#define CAN_RX4_REG           (CAN_BASE_OFFSET+96U)

/**
 * @brief RX buffer5 register, layout:
 * @code
 * CAN_RX5_REG[7:0] -> can_frame_s.DATA[3]
 * @endcode
 * */
#define CAN_RX5_REG           (CAN_BASE_OFFSET+100U)

/**
 * @brief RX buffer6 register, layout:
 * @code
 * CAN_RX6_REG[7:0] -> can_frame_s.DATA[4]
 * @endcode
 * */
#define CAN_RX6_REG           (CAN_BASE_OFFSET+104U)

/**
 * @brief RX buffer7 register, layout:
 * @code
 * CAN_RX7_REG[7:0] -> can_frame_s.DATA[5]
 * @endcode
 * */
#define CAN_RX7_REG           (CAN_BASE_OFFSET+108U)

/**
 * @brief RX buffer8 register, layout:
 * @code
 * CAN_RX8_REG[7:0] -> can_frame_s.DATA[6]
 * @endcode
 * */
#define CAN_RX8_REG           (CAN_BASE_OFFSET+112U)

/**
 * @brief RX buffer9 register, layout:
 * @code
 * CAN_RX9_REG[7:0] -> can_frame_s.DATA[7]
 * @endcode
 * */
#define CAN_RX9_REG           (CAN_BASE_OFFSET+116U)

#else

/**
 * @brief TX buffer0 register, layout:
 * @code
 * CAN_TX0_REG[7]   -> can_frame_s.IDE
 * CAN_TX0_REG[6]   -> can_frame_s.RTR
 * CAN_TX0_REG[5:4] -> Reserved
 * CAN_TX0_REG[3:0] -> can_frame_s.DLC
 * @endcode
 * */
#define CAN_TX0_REG           (CAN_BASE_OFFSET+64U)

/**
 * @brief TX buffer1 register, layout:
 * @code
 * CAN_TX1_REG[7:0] -> can_frame_s.ID[28:21]
 * @endcode
 * */
#define CAN_TX1_REG           (CAN_BASE_OFFSET+68U)

/**
 * @brief TX buffer2 register, layout:
 * @code
 * CAN_TX2_REG[7:0] -> can_frame_s.ID[20:13]
 * @endcode
 * */
#define CAN_TX2_REG           (CAN_BASE_OFFSET+72U)

/**
 * @brief TX buffer3 register, layout:
 * @code
 * CAN_TX3_REG[7:0] -> can_frame_s.ID[12:5]
 * @endcode
 * */
#define CAN_TX3_REG           (CAN_BASE_OFFSET+76U)

/**
 * @brief TX buffer4 register, layout:
 * @code
 * CAN_TX4_REG[7:3] -> can_frame_s.ID[4:0]
 * CAN_TX4_REG[2:0] -> Reserved
 * @endcode
 * */
#define CAN_TX4_REG           (CAN_BASE_OFFSET+80U)

/**
 * @brief TX buffer5 register, layout:
 * @code
 * CAN_TX5_REG[7:0] -> can_frame_s.DATA[0]
 * @endcode
 * */
#define CAN_TX5_REG           (CAN_BASE_OFFSET+84U)

/**
 * @brief TX buffer6 register, layout:
 * @code
 * CAN_TX6_REG[7:0] -> can_frame_s.DATA[1]
 * @endcode
 * */
#define CAN_TX6_REG           (CAN_BASE_OFFSET+88U)

/**
 * @brief TX buffer7 register, layout:
 * @code
 * CAN_TX7_REG[7:0] -> can_frame_s.DATA[2]
 * @endcode
 * */
#define CAN_TX7_REG           (CAN_BASE_OFFSET+92U)

/**
 * @brief TX buffer8 register, layout:
 * @code
 * CAN_TX8_REG[7:0] -> can_frame_s.DATA[3]
 * @endcode
 * */
#define CAN_TX8_REG           (CAN_BASE_OFFSET+96U)

/**
 * @brief TX buffer9 register, layout:
 * @code
 * CAN_TX9_REG[7:0] -> can_frame_s.DATA[4]
 * @endcode
 * */
#define CAN_TX9_REG           (CAN_BASE_OFFSET+100U)

/**
 * @brief TX buffer10 register, layout:
 * @code
 * CAN_TX10_REG[7:0] -> can_frame_s.DATA[5]
 * @endcode
 * */
#define CAN_TX10_REG          (CAN_BASE_OFFSET+104U)

/**
 * @brief TX buffer11 register, layout:
 * @code
 * CAN_TX11_REG[7:0] -> can_frame_s.DATA[6]
 * @endcode
 * */
#define CAN_TX11_REG          (CAN_BASE_OFFSET+108U)

/**
 * @brief TX buffer12 register, layout:
 * @code
 * CAN_TX12_REG[7:0] -> can_frame_s.DATA[7]
 * @endcode
 * */
#define CAN_TX12_REG          (CAN_BASE_OFFSET+112U)

/**
 * @brief RX buffer0 register, layout:
 * @code
 * CAN_RX0_REG[7]   -> can_frame_s.IDE
 * CAN_RX0_REG[6]   -> can_frame_s.RTR
 * CAN_RX0_REG[5:4] -> Reserved
 * CAN_RX0_REG[3:0] -> can_frame_s.DLC
 * @endcode
 * */
#define CAN_RX0_REG           (CAN_BASE_OFFSET+64U)

/**
 * @brief RX buffer1 register, layout:
 * @code
 * CAN_RX1_REG[7:0] -> can_frame_s.ID[28:21]
 * @endcode
 * */
#define CAN_RX1_REG           (CAN_BASE_OFFSET+68U)

/**
 * @brief RX buffer2 register, layout:
 * @code
 * CAN_RX2_REG[7:0] -> can_frame_s.ID[20:13]
 * @endcode
 * */
#define CAN_RX2_REG           (CAN_BASE_OFFSET+72U)

/**
 * @brief RX buffer3 register, layout:
 * @code
 * CAN_RX3_REG[7:0] -> can_frame_s.ID[12:5]
 * @endcode
 * */
#define CAN_RX3_REG           (CAN_BASE_OFFSET+76U)

/**
 * @brief RX buffer4 register, layout:
 * @code
 * CAN_RX4_REG[7:3] -> can_frame_s.ID[4:0]
 * CAN_RX4_REG[2:0] -> Reserved
 * @endcode
 * */
#define CAN_RX4_REG           (CAN_BASE_OFFSET+80U)

/**
 * @brief RX buffer5 register, layout:
 * @code
 * CAN_RX5_REG[7:0] -> can_frame_s.DATA[0]
 * @endcode
 * */
#define CAN_RX5_REG           (CAN_BASE_OFFSET+84U)

/**
 * @brief RX buffer6 register, layout:
 * @code
 * CAN_RX6_REG[7:0] -> can_frame_s.DATA[1]
 * @endcode
 * */
#define CAN_RX6_REG           (CAN_BASE_OFFSET+88U)

/**
 * @brief RX buffer7 register, layout:
 * @code
 * CAN_RX7_REG[7:0] -> can_frame_s.DATA[2]
 * @endcode
 * */
#define CAN_RX7_REG           (CAN_BASE_OFFSET+92U)

/**
 * @brief RX buffer8 register, layout:
 * @code
 * CAN_RX8_REG[7:0] -> can_frame_s.DATA[3]
 * @endcode
 * */
#define CAN_RX8_REG           (CAN_BASE_OFFSET+96U)

/**
 * @brief RX buffer9 register, layout:
 * @code
 * CAN_RX9_REG[7:0] -> can_frame_s.DATA[4]
 * @endcode
 * */
#define CAN_RX9_REG           (CAN_BASE_OFFSET+100U)

/**
 * @brief RX buffer10 register, layout:
 * @code
 * CAN_RX10_REG[7:0] -> can_frame_s.DATA[5]
 * @endcode
 * */
#define CAN_RX10_REG          (CAN_BASE_OFFSET+104U)

/**
 * @brief RX buffer11 register, layout:
 * @code
 * CAN_RX11_REG[7:0] -> can_frame_s.DATA[6]
 * @endcode
 * */
#define CAN_RX11_REG          (CAN_BASE_OFFSET+108U)

/**
 * @brief RX buffer12 register, layout:
 * @code
 * CAN_RX12_REG[7:0] -> can_frame_s.DATA[7]
 * @endcode
 * */
#define CAN_RX12_REG          (CAN_BASE_OFFSET+112U)

#endif // !(CAN_MODE == CAN_2B)

/**
 * @brief Clock divider register, layout:
 * @code
 * CAN_CDR_REG[7:7] -> extended_mode (0 disable, 1 enable)
 * CAN_CDR_REG[6:4] -> reserved
 * CAN_CDR_REG[3:3] -> clock_off (0 disable, 1 enable)
 * CAN_CDR_REG[2:0] -> clock divider (integer valid range 0 - 6)
 * @endcode
 * */
#define CAN_CDR_REG           (CAN_BASE_OFFSET+124U)

/**
 * @brief \ref Disable triple sampling
 *
 * CAN_BTR1_REG[7] default (hard-coded) value
 */
#define CAN_TRIPLE_SAM        (0x0U)

/**
 * @brief Disable output clock
 *
 * \ref CAN_CDR_REG[3] default (hard-coded) value
 */
#define CAN_CLK_O_OFF         (0x1U)

/**
 * @brief Disable triple sampling
 *
 * \ref CAN_CDR_REG[2:0] default (hard-coded) value
 */
#define CAN_CLK_DIV           (0x0U)

/**
 * @brief Trigger TX default value
 *
 * \ref Default value for \ref CAN_COMMAND_TX_REG register to trigger TX
 * @note You should initialize TX buffers first
 */
#define CAN_CMNT_TRIGGER_TX   (0x1U)

/**
 * @brief Acknowledge RX default value
 *
 * \ref Default value for \ref CAN_COMMAND_RX_REG register to acknowledge RX
 */
#define CAN_CMNT_RX_ACK       (0x4U) //Request to reduce RXed frames counter.

/**
 * @brief Clear RX FIFO overrun default value
 *
 * \ref Default value for \ref CAN_COMMAND_RX_REG register to clear RX FIFO
 * overrun
 */
#define CAN_CMNT_CLR_OVERRUN  (0x8U) //Request to clear overrun.

/**
 * @brief Query TX logic state
 *
 * Decode \ref CAN_STATUS_REG value to find out TX logic state
 * @return 0 idle
 * @return 1 active
 */
#define CAN_Q_TX_BUF_STATUS(status)     ( !((status>>2U) & 1U) )

/**
 * @brief Query RX logic state
 *
 * Decode \ref CAN_STATUS_REG value to find out RX logic state
 * @return 0 idle
 * @return 1 active
 */
#define CAN_Q_RX_STATUS(status)			( ((status)>>4U) & 1U )

/**
 * @brief Query RX buffer status
 *
 * Decode \ref CAN_STATUS_REG value to find out RX FIFO status
 * @return 0 (empty)
 * @return 1 full (frame received)
 */
#define CAN_Q_RX_BUF_STATUS(status)		( ((status)>>0U) & 1U )

/**
 * @brief Query RX FIFO overrun status
 *
 * Decode \ref CAN_STATUS_REG value to find out RX FIFO status
 * @return 0 no overrun
 * @return 1 overrun occurred
 */
#define CAN_Q_RX_OVERRUN(status)        ( ((status)>>1U) & 1U )

/******
 * PRIVATE VARIABLES
 ******/
#if !(CAN_MODE == CAN_2B)
/// TX payload buffers registers
static uint8_t const
can_tx_payload_addr[] = {
		CAN_TX2_REG, CAN_TX3_REG,
		CAN_TX4_REG, CAN_TX5_REG,
		CAN_TX6_REG, CAN_TX7_REG,
		CAN_TX8_REG, CAN_TX9_REG
};

/// RX payload buffers registers
static uint8_t const
can_rx_payload_addr[] = {
		CAN_RX2_REG, CAN_RX3_REG,
		CAN_RX4_REG, CAN_RX5_REG,
 		CAN_RX6_REG, CAN_RX7_REG,
 		CAN_RX8_REG, CAN_RX9_REG
};
#else
/// TX payload buffers registers when
/// \ref can_frame_s.IDE == \ref CAN_FRAME_EXT
static uint8_t const
can_tx_ext_payload_addr[] = {
		CAN_TX5_REG,  CAN_TX6_REG,
		CAN_TX7_REG,  CAN_TX8_REG,
		CAN_TX9_REG,  CAN_TX10_REG,
		CAN_TX11_REG, CAN_TX12_REG
};

/// RX payload buffers registers when
/// \ref can_frame_s.IDE == \ref CAN_FRAME_EXT
static uint8_t const
can_rx_ext_payload_addr[] = {
		CAN_RX5_REG,  CAN_RX6_REG,
		CAN_RX7_REG,  CAN_RX8_REG,
		CAN_RX9_REG,  CAN_RX10_REG,
		CAN_RX11_REG, CAN_RX12_REG
};

/// TX payload buffers registers when
/// \ref can_frame_s.IDE == \ref CAN_FRAME_STD
static uint8_t const
can_tx_payload_addr[] = {
		CAN_TX3_REG,  CAN_TX4_REG,
		CAN_TX5_REG,  CAN_TX6_REG,
		CAN_TX7_REG,  CAN_TX8_REG,
		CAN_TX9_REG, CAN_TX10_REG
};

/// RX payload buffers registers when
/// \ref can_frame_s.IDE == \ref CAN_FRAME_STD
static uint8_t const
can_rx_payload_addr[] = {
		CAN_RX3_REG,  CAN_RX4_REG,
		CAN_RX5_REG,  CAN_RX6_REG,
		CAN_RX7_REG,  CAN_RX8_REG,
		CAN_RX9_REG,  CAN_RX10_REG
};
#endif // !(CAN_MODE == CAN_2B)

/******
 * FUNCTIONS DEFINITION
 ******/
/*********************************************************************//**
 * @brief		CAN controller initialize function
 * @param[in]	can_ctrl CAN controller instance pointer
 * @return      \ref ISCA_CAN_OK
 * @see         CAN_MODE0_REG
 * @see         CAN_IRQS_EN_REG
 **********************************************************************/
int isca_can_init (can_ctrl_s *can_ctrl)
{

#if (CAN_MODE == CAN_2B)
	uint8_t irqs;
#endif // (CAN_MODE == CAN_2B)

	uint8_t btr0;
	uint8_t btr1;
	uint8_t cdr;
	uint8_t cfg0;
	size_t  addr = can_ctrl->addr;

	// Switch on can controller's reset mode, and clear ext_mode values
	ISCA_FPGA_Write8Bit(addr+CAN_MODE0_REG, ISCA_CAN_MODE_RESET_ON);

	/* Set bus timing register 1*/
	btr0  = 0x0U;
	btr0 |= (can_ctrl->sjw << 6U);
	btr0 |= (can_ctrl->brp & 0x3F);
	ISCA_FPGA_Write8Bit(addr+CAN_BTR0_REG, btr0);

	/* Set bus timing register 2*/
	btr1  = 0x0U;
	btr1 |= (CAN_TRIPLE_SAM << 7U);
	btr1 |= ((can_ctrl->tseg2 & 0x7U) << 4U);
	btr1 |=  (can_ctrl->tseg1 & 0xFU);

	ISCA_FPGA_Write8Bit(addr+CAN_BTR1_REG, btr1);

	/* Set bus timing register 1*/
	cdr  = 0x0U;
	cdr |= (CAN_CLK_O_OFF  << 3U);
	cdr |= (CAN_CLK_DIV    &  7U);
#if (CAN_MODE == CAN_2B)
	// Enable CAN 2B (extended) mode*/
	cdr |= (0x1U << 7U);
#endif // !(CAN_MODE == CAN_2B)

	ISCA_FPGA_Write8Bit(addr+CAN_CDR_REG, cdr);

	/* Set up can filter*/
#if !(CAN_MODE == CAN_2B)
	isca_can_set_filter(can_ctrl);
#else
	isca_can_set_filter(can_ctrl);
#endif // !(CAN_MODE == CAN_2B)

#if !(CAN_MODE == CAN_2B)
	/* Set CAN mode register*/
	cfg0  = ISCA_CAN_MODE_RESET_OFF;
	if ( can_ctrl->irq == CAN_IRQ_ON ) {
		cfg0 |= ((can_ctrl->irqs_en.err0&1U) << 0x4U); // overrun IRQ
		cfg0 |= ((can_ctrl->irqs_en.err1&1U) << 0x3U); // error IRQ
		cfg0 |= ((can_ctrl->irqs_en.tx&1U)   << 0x2U); // transmit IRQ
		cfg0 |= ((can_ctrl->irqs_en.rx&1U)   << 0x1U); // receive IRQ
	} /**>Enable interrupts iff ordered*/

	ISCA_FPGA_Write8Bit(addr+CAN_MODE0_REG, cfg0);

#else

	/* Set CAN mode register*/
	cfg0  = ISCA_CAN_MODE_RESET_OFF;
	cfg0 |= 0x8U; //dual filtering not supported in current version
	ISCA_FPGA_Write8Bit(addr+CAN_MODE0_REG, cfg0|0x8U);

	irqs = 0U;
	if ( can_ctrl->irq == CAN_IRQ_ON ) {

		/* Activate IRQs as ordered*/
		irqs |= ((can_ctrl->irqs_en.err4&1U) << 0x7U); /* bus error IRQ */
		irqs |= ((can_ctrl->irqs_en.err3&1U) << 0x6U); /* arbitration lost IRQ */
		irqs |= ((can_ctrl->irqs_en.err2&1U) << 0x5U); /* error passive IRQ */
		irqs |= ((can_ctrl->irqs_en.err1&1U) << 0x3U); /* overrun IRQ */
		irqs |= ((can_ctrl->irqs_en.err0&1U) << 0x2U); /* error IRQ */
		irqs |= ((can_ctrl->irqs_en.tx&1U)   << 0x1U); /* transmit IRQ */
		irqs |= ((can_ctrl->irqs_en.rx&1U)   << 0x0U); /* receive IRQ */

	} /**<Activate IRQs?*/

	ISCA_FPGA_Write8Bit(addr+CAN_IRQS_EN_REG, irqs);

#endif // !(CAN_MODE == CAN_2B)

	return ISCA_CAN_OK;
}

/*********************************************************************//**
 * @brief		CAN controller transmit frame
 * @pre			CAN controller initialization
 * @param[in]	can_ctrl  CAN controller struct
 * @param[in]   tx_frame  CAN frame struct pointer
 * @param[in]   req_type  /ref CAN_REF_BLOCKING or \ref CAN_REF_NONBLOCKING
 * @return      \ref ISCA_CAN_BUSY
 * @return      \ref ISCA_CAN_INV_IO_TYPE
 * @return      \ref ISCA_CAN_OK
 **********************************************************************/
int isca_can_transmit_frame(can_ctrl_s *can_ctrl, can_frame_s *tx_frame, uint8_t req_type)
{

#if !(CAN_MODE == CAN_2B)
	uint8_t tx_header[2] = {0x0U, 0x0U};
#else
	uint8_t  tx_header[5] = {0x0U, 0x0U, 0x0U, 0x0U, 0x0U};
#endif // !(CAN_MODE == CAN_2B)

	uint8_t i;
	size_t  addr = can_ctrl->addr;

	if (req_type == CAN_REQ_NONBLOCKING) {
		if (CAN_Q_TX_BUF_STATUS(ISCA_FPGA_Read8Bit(addr+CAN_STATUS_REG))) {
			return ISCA_CAN_BUSY;
		} /**<tx busy*/
	} /**<Non-blocking mode*/
	else if (req_type == CAN_REQ_BLOCKING) {
		while(CAN_Q_TX_BUF_STATUS(ISCA_FPGA_Read8Bit(addr+CAN_STATUS_REG))) {
			//TODO: sleep
			/*!<@todo Sleep while TX requested and controller is busy*/
		}; /*tx busy*/
	} /**<Blocking mode*/
	else {
		return ISCA_CAN_INV_REQ_TYPE;
	} /**<Invalid request type*/


#if !(CAN_MODE == CAN_2B)
	tx_header[0] |= (tx_frame->ID  & 0x7F8U) >> 3U; //ID[10:3]
    ISCA_FPGA_Write8Bit(addr+CAN_TX0_REG, tx_header[0]);

    tx_header[1] |= (tx_frame->ID  & 0x7U)   << 5U; //ID[2:0]
    tx_header[1] |= (tx_frame->RTR & 0x1U)   << 4U;
    tx_header[1] |= (tx_frame->DLC & 0xFU);
	ISCA_FPGA_Write8Bit(addr+CAN_TX1_REG, tx_header[1]);
#else
    //tx_header[0] |= (tx_frame->IDE & 0x4U) << 5U; // SCAN
	tx_header[0] |= (tx_frame->IDE & 0x1U) << 7U;
	tx_header[0] |= (tx_frame->RTR & 0x1U) << 6U;
    tx_header[0] |= (tx_frame->DLC & 0xFU);
	ISCA_FPGA_Write8Bit(addr+CAN_TX0_REG, tx_header[0]);

	if (tx_frame->IDE == CAN_FRAME_EXT) {
		tx_header[1] = (tx_frame->ID & 0x1FE00000U) >> 21U;
		ISCA_FPGA_Write8Bit(addr+CAN_TX1_REG, tx_header[1]);

		tx_header[2] = (tx_frame->ID & 0x1FE000U) >> 13U;
		ISCA_FPGA_Write8Bit(addr+CAN_TX2_REG, tx_header[2]);

		tx_header[3] = (tx_frame->ID & 0x1FE0U) >> 5U;
		ISCA_FPGA_Write8Bit(addr+CAN_TX3_REG, tx_header[3]);

		tx_header[4] = (tx_frame->ID & 0x1F) << 3U;
		ISCA_FPGA_Write8Bit(addr+CAN_TX4_REG, tx_header[4]);
	} /**<CAN_2B extended frame ID*/
	else {
		tx_header[1] = (tx_frame->ID & 0x7F8) >> 3U;
		ISCA_FPGA_Write8Bit(addr+CAN_TX1_REG, tx_header[1]);

		tx_header[2] = (tx_frame->ID & 0x7U) << 5U;
		ISCA_FPGA_Write8Bit(addr+CAN_TX2_REG, tx_header[2]);
	} /**<CAN_2B basic frame ID*/
#endif // !(CAN_MODE == CAN_2B)

#if !(CAN_MODE == CAN_2B)
	// Frame payload may be of variable size
	for (i = 0; i < tx_frame->DLC; i++) {
		ISCA_FPGA_Write8Bit(addr+can_tx_payload_addr[i], tx_frame->DATA[i]);
	}
#else

	if (tx_frame->IDE == CAN_FRAME_EXT) { /*CAN_2B extended frame ID*/
		/*Frame payload may be of variable size*/
		for (i = 0; i < tx_frame->DLC; i++) {
			ISCA_FPGA_Write8Bit(addr+can_tx_ext_payload_addr[i], tx_frame->DATA[i]);
		}
	} else { /*CAN_2B basic frame ID*/
		/*Frame payload may be of variable size*/
		for (i = 0; i < tx_frame->DLC; i++) {
			ISCA_FPGA_Write8Bit(addr+can_tx_payload_addr[i], tx_frame->DATA[i]);
		}
	}
#endif // !(CAN_MODE == CAN_2B)

    /*trigger TX*/
	ISCA_FPGA_Write8Bit(addr+CAN_COMMAND_TX_REG, CAN_CMNT_TRIGGER_TX);

    return ISCA_CAN_OK;
}

/*********************************************************************//**
 * @brief		CAN controller receive frame
 * @pre			CAN controller initialization
 * @param[in]	can_ctrl CAN controller struct pointer
 * @param[out]  rx_frame CAN frame struct pointer
 * @param[in]   req_type \ref CAN_IO_BLOCKING or \ref CAN_IO_NONBLOCKING
 * @return      \ref ISCA_CAN_RX_FIFO_EMPTY
 * @return 		\ref ISCA_CAN_INV_IO_TYPE
 * @return		\ref ISCA_CAN_OK
 **********************************************************************/
int isca_can_receive_frame(can_ctrl_s *can_ctrl, can_frame_s *rx_frame, uint8_t io_type)
{

#if !(CAN_MODE == CAN_2B)
	uint8_t rx_header[2] = {0x0U, 0x0U};
#else
	uint8_t rx_header[5] = {0x0U, 0x0U, 0x0U, 0x0U, 0x0U};
	uint8_t ide = 0x0U;
#endif // !(CAN_MODE == CAN_2B)

	int32_t  remain_frames = 0x0U;
	size_t   addr  = can_ctrl->addr;
    uint32_t id    = 0x0U;
    uint8_t  rtr   = 0x0U;
    uint8_t  dlc   = 0x0U;
    uint8_t  i;

    if ( CAN_Q_RX_OVERRUN(ISCA_FPGA_Read8Bit(addr+CAN_STATUS_REG)) ) {

    	while (ISCA_FPGA_Read8Bit(addr+CAN_INVALID_RX_ACK_REG) != 0) {
    		/*Acknowledge RX buff, should decrease frames counter*/
    		ISCA_FPGA_Write8Bit(addr+CAN_COMMAND_RX_REG, CAN_CMNT_RX_ACK);
    	} /**<while all overrun data is acknowledged*/

    } /**<RX FIFO overrun occurred*/

    remain_frames = ISCA_FPGA_Read8Bit(addr+CAN_RX_COUNTER_REG);

	if ( io_type == CAN_REQ_NONBLOCKING ) {

		if ( remain_frames == 0x0U ) {
			return ISCA_CAN_RX_FIFO_EMPTY;
		} /**<No frame received*/

	} /**<io_type non-blocking*/
	else if (io_type == CAN_REQ_BLOCKING) {

		while (remain_frames == 0x0U) {
			remain_frames = ISCA_FPGA_Read8Bit(addr+CAN_RX_COUNTER_REG);
			/*TODO: sleep*/
			/*!<@todo Sleep while waiting for a frame to arrive*/
		}; /*wait for a frame to arrive*/

	} /**<io_type blocking*/
	else {

		return ISCA_CAN_INV_REQ_TYPE;

	} /*Invalid io_type*/

#if !(CAN_MODE == CAN_2B)
	// read frame header
	rx_header[0] = ISCA_FPGA_Read8Bit(addr+CAN_RX0_REG);
	rx_header[1] = ISCA_FPGA_Read8Bit(addr+CAN_RX1_REG);

	/// Decode CAN frame header ID
	id  |= ( (uint32_t)rx_header[0] << 3);
	id  |= ((rx_header[1] >> 5) & 0x7U);
	rtr  = ((rx_header[1] >> 4) & 0x1U);
	dlc  = ( rx_header[1]       & 0xFU);

	// Frame payload may be of variable size
	dlc = (dlc==0x8U) ? 0x8U : (dlc & 0x7U); //SCAN
	for (i = 0; i < dlc; i++) {
		rx_frame-> DATA[i] = ISCA_FPGA_Read8Bit(addr+can_rx_payload_addr[i]);
	} /*Read frame payload*/

#else
	/*Read frame header*/
	rx_header[0] = ISCA_FPGA_Read8Bit(addr+CAN_RX0_REG);

	// Decode CAN frame header
	//ide = ((rx_header[0] & 0x80U) >> 5U);  // SCAN
	ide = ((rx_header[0] & 0x80U) >> 7U);
	rtr = ((rx_header[0] & 0x40U) >> 6U);
	dlc =  (rx_header[0] & 0x0FU);
	//dlc_l = (dlc>0x8U) ? 0x8U : dlc; // support SCAN

	rx_frame->IDE = ide;

	if (ide != can_ctrl->frm_md) {
		/* CAN frame received header with mode
		 * different than the one CAN controller
		 * is initialized to filter (basic <-> extended)
		 * */
		/**<@todo Notify user if during CAN 2B mode and filtering
		 * is set to basic, an extended frame is has been received
		 * or vice versa*/
	}

	// Read part of CAN header ID
	rx_header[1] = ISCA_FPGA_Read8Bit(addr+CAN_RX1_REG);
	rx_header[2] = ISCA_FPGA_Read8Bit(addr+CAN_RX2_REG);
	if (ide == CAN_FRAME_EXT) {
		// Read rest of CAN header ID
		rx_header[3] = ISCA_FPGA_Read8Bit(addr+CAN_RX3_REG);
		rx_header[4] = ISCA_FPGA_Read8Bit(addr+CAN_RX4_REG);

		// Decode CAN frame header ID
		id |= (rx_header[1] << 21U);
		id |= (rx_header[2] << 13U);
		id |= (rx_header[3] << 5U);
		id |= ((rx_header[4] & 0xF8U) >> 3U);

		// Frame payload may be of variable size
		for (i = 0; i < dlc; i++) {
			rx_frame-> DATA[i] = ISCA_FPGA_Read8Bit(addr+can_rx_ext_payload_addr[i]);
		}
	} /**<CAN_2B extended frame ID*/
	else {

		id |= (rx_header[1] << 3U);
		id |= ((rx_header[2] & 0xE0U) >> 5U);

		// Frame payload may be of variable size
		for (i = 0; i < dlc; i++) {
			rx_frame->DATA[i] = ISCA_FPGA_Read8Bit(addr+can_rx_payload_addr[i]);
		}

	} /**<CAN_2B basic frame ID*/
#endif // !(CAN_MODE == CAN_2B)

	rx_frame-> ID  = id;
	rx_frame-> RTR = rtr;
	rx_frame-> DLC = dlc;

	//Acknowledge RX buff read, should decrease frames counter
	ISCA_FPGA_Write8Bit(addr+CAN_COMMAND_RX_REG, CAN_CMNT_RX_ACK);

	//Return remain frames
	return (remain_frames-1);
}

/*********************************************************************//**
 * @brief		Switch can controller's reset mode on/off.
 * @param[in]   can_ctrl CAN controller struct pointer
 * @param[in]	reset mode \ref ISCA_CAN_MODE_RESET_OFF
 *              or \ref ISCA_CAN_MODE_RESET_ON
 * @return 		Controller's previous \ref CAN_MODE0_REG value
 * @return      \ref ISCA_CAN_INV_RST_MODE
 **********************************************************************/
int isca_can_switch_mode(can_ctrl_s *can_ctrl, uint8_t reset_mode)
{

	uint8_t prev_mode;
	size_t  addr = can_ctrl->addr;

	prev_mode = ISCA_FPGA_Read8Bit(addr+CAN_MODE0_REG);

	if ((prev_mode & 0x1U) != (reset_mode & 0x1U)) {
#if !(CAN_MODE == CAN_2B)
		/*mode register also includes the IRQs mode in 2A mode*/
		ISCA_FPGA_Write8Bit(addr+CAN_MODE0_REG, (prev_mode&0x01EU)|(reset_mode&0x1U));
#else
		ISCA_FPGA_Write8Bit(addr+CAN_MODE0_REG, (prev_mode&0x0EU)|(reset_mode&0x1U));
#endif // !(CAN_MODE == CAN_2B)
	} /**<Controller is in different than the ordered reset mode*/
	else {
		return ISCA_CAN_INV_RST_MODE;
	} /**<Controller is in the same with the ordered reset mode*/

	//return the CAN_MODE0_REG value before update, if any
	return prev_mode;
}

/*********************************************************************//**
 * @brief		CAN controller set filter function
 * @param[in]	can_ctrl 32bit mask
 * @return 		None
 * @info        At least can_ctrl->addr, can_ctrl->mask and
 *              can_ctrl->code should have been set
 **********************************************************************/
void isca_can_set_filter(can_ctrl_s *can_ctrl)
{

	size_t    addr = can_ctrl->addr;
	uint32_t  code = can_ctrl->code,
			  mask = can_ctrl->mask;

	// Need to enter filter mode since can version 2.2
	// Enable filter mode
	ISCA_FPGA_Write8Bit(addr+CAN_FILTER_MODE_REG, 0x1U);

	// Set mask and code registers
#if !(CAN_MODE == CAN_2B) /*Standard mode*/
	// CAN_CODE_REG0[7:0] -> CODE(ID)[10:3] */
	ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG0, (uint8_t)((code & 0x7F8U) >> 3U));

	// CAN_CODE_REG1[2:0] -> CODE(ID)[2:0] */
	ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG1, (uint8_t)(code & 0x7U));

	// CAN_MASK_REG0[7:0] -> MASK(ID)[10:3] */
	ISCA_FPGA_Write8Bit(addr+CAN_MASK_REG0, (uint8_t)((mask & 0x7F8U) >> 3U));

	// CAN_MASK_REG1[2:0] -> MASK(ID)[2:0] */
	ISCA_FPGA_Write8Bit(addr+CAN_MASK_REG1, (uint8_t)(mask & 0x7U));

#else /*Extended mode*/
	switch (can_ctrl->frm_md) {
		case CAN_FRAME_EXT /*extended ID single filtering*/ :
			//CAN_CODE_REG0[7:0] = ID[28:21]
			ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG0, (uint8_t)(code >>21U));
			//CAN_CODE_REG1[7:0] = ID[20:13]
			ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG1, (uint8_t)(code >>13U));
			//CAN_CODE_REG2[7:0] = ID[12:5]
			ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG2, (uint8_t)(code >> 5U));
			//CAN_CODE_REG3[7:3] = ID[4:0], CAN_CODE_REG3[2] = rtr2
			ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG3, (uint8_t)((code&0x1FU) << 3U));

			//CAN_MASK_REG0[7:0] = ID[28:21]
			ISCA_FPGA_Write8Bit(addr+CAN_MASK_REG0, (uint8_t)(mask >>21U));
			//CAN_MASK_REG1[7:0] = ID[20:13]
			ISCA_FPGA_Write8Bit(addr+CAN_MASK_REG1, (uint8_t)(mask >>13U));
			//CAN_MASK_REG2[7:0] = ID[12:5]
			ISCA_FPGA_Write8Bit(addr+CAN_MASK_REG2, (uint8_t)(mask >> 5U));
			//CAN_MASK_REG3[7:3] = ID[4:0], CAN_MASK_REG3[2] = rtr2 (='1' always allow)
			ISCA_FPGA_Write8Bit(addr+CAN_MASK_REG3, (uint8_t)((mask&0x1FU) << 3U)|0x4U);

			break;

		case CAN_FRAME_STD /*standard ID single filtering*/ :
			//CAN_CODE_REG0[7:0] = ID[10:3]
			ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG0, (uint8_t)(code >> 3U));
			//CAN_CODE_REG1[7:5] = ID[2:0]
			ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG1, (uint8_t)((code&0x7U) << 5U));

			//CAN_MASK_REG0[7:0] = ID[10:3]
			ISCA_FPGA_Write8Bit(addr+CAN_MASK_REG0, (uint8_t)(mask >> 3U));
			//CAN_MASK_REG1[7:5] = ID[2:0], set never check RTR (CAN_MASK_REG1[4])
			ISCA_FPGA_Write8Bit(addr+CAN_MASK_REG1, (uint8_t)(((mask&0x7U) << 5U)) | 0x10U);

			/*Never check for data fields (i.e.payload); set mask to '1'*/
			//CAN_MASK_REG2[7:0] = DATA0[7:0]
			ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG2, (uint8_t)(0xFFU));
			//CAN_MASK_REG3[7:0] = DATA1[7:0]
			ISCA_FPGA_Write8Bit(addr+CAN_CODE_REG3, (uint8_t)(0xFFU));

			break;

		default:
			break;
	}

	// Disable filter mode
	ISCA_FPGA_Write8Bit(addr+CAN_FILTER_MODE_REG, 0x0U);

#endif // !(CAN_MODE == CAN_2B)

}
