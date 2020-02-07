/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : ISCA_CAN compile-time configuration options
 * |-- Project    : CAN-bus Controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : ISCA_CAN_TEST.c
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2018-10-22
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
 * @file ISCA_CAN_CFG.h
 *
 * @brief CAN configuration definitions
 *
 * @author Othon Tomoutzoglou
 *
 * Contact: <otto_sta@hotmail.com>
 *
 * @version 1.0
 *
 */

#ifndef ISCA_CAN_CFG_H
#define ISCA_CAN_CFG_H

#include <stdarg.h>
#include <stdio.h>

/******
 * DEFINITIONS
 ******/
#define CAN_2A      (0) /*!<CAN mode 2A (standard) constant*/
#define CAN_2B      (1) /*!<CAN mode 2B (extended) constant*/
#define APP_POLL    (0) /*!<CAN apprise in polling mode constant*/
#define APP_IRQ     (1) /*!<CAN apprise in interrupt mode constant*/

/*
 * Update values accordingly, or pass
 * them as compiler options, e.g.,
 * -DCAN_MODE=CAN_2B
 */
#ifndef CAN_MODE
#define CAN_MODE       (CAN_2B)  /*!<CAN operating mode; \ref CAN_2A or \ref CAN_2B*/
#endif

#ifndef APPRISE_MODE
#define APPRISE_MODE   (APP_POLL) /*!<Apprise mode \ref APP_POLL or \ref APP_IRQ*/
#endif

/**
 * @brief Print to standard output function wrapper
 *
 * If it happens your board to support some other printf like
 * function, or you have implemented your own modify this
 * definition
 *
 * @note You can change standard output function used, by modifying
 * this definition
 * */
#define __COUT(str, ...)    (printf(str, ##__VA_ARGS__))

#endif /* ISCA_CAN_CFG_H */
