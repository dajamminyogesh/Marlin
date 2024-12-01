/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include <stdint.h>

/**
 * DGUSVPVariable.h
 *
 *  Created on: 2022/05/13
 *      Author: CreatBot LYN
 */

struct DGUS_VP_Variable {
  uint16_t VP;
  uint8_t  size;
  uint16_t match;

  void (*rx_handler)(DGUS_VP_Variable &, void *);
  void (*tx_handler)(DGUS_VP_Variable &);
};

#define VP_HELPER(ADDR, SIZE, MATCH, RXHANDLER, TXHANDLER)                                                             \
  { .VP = ADDR, .size = SIZE, .match = MATCH, .rx_handler = RXHANDLER, .tx_handler = TXHANDLER }

#define VP_HELPER_RX(ADDR, RXHANDLER)              VP_HELPER(ADDR, 2, 0, RXHANDLER, nullptr)
#define VP_HELPER_RX_DWORD(ADDR, RXHANDLER)        VP_HELPER(ADDR, 4, 0, RXHANDLER, nullptr)

#define VP_HELPER_TX(ADDR, TXHANDLER)              VP_HELPER(ADDR, 2, 0, nullptr, TXHANDLER)
#define VP_HELPER_TX_DWORD(ADDR, TXHANDLER)        VP_HELPER(ADDR, 4, 0, nullptr, TXHANDLER)

#define VP_HELPER_RXTX(ADDR, RXHANDLER, TXHANDLER) VP_HELPER(ADDR, 2, 0, RXHANDLER, TXHANDLER)
#define VP_HELPER_STR(ADDR, SIZE, TXHANDLER)       VP_HELPER(ADDR, SIZE, 0, nullptr, TXHANDLER)

#define VP_HELPER_MATCH(ADDR, VAL, RXHANDLER)      VP_HELPER(ADDR, 0, VAL, RXHANDLER, nullptr)
