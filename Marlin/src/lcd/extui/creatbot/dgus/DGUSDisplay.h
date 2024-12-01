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

/**
 * lcd/extui/creatbot/dgus/DGUSDisplay.h
 */

#include <stdlib.h>  // size_t

#include "../../../../inc/MarlinConfigPre.h"
#include "DGUSVPVariable.h"

enum DGUSLCD_Screens : uint8_t;

// #define DEBUG_DGUSLCD
#define DEBUG_OUT ENABLED(DEBUG_DGUSLCD)
#include "../../../../core/debug_out.h"

// Preamble... 2 Bytes, usually 0x5A 0xA5, but configurable
constexpr uint8_t DGUS_HEADER1      = 0x5A;
constexpr uint8_t DGUS_HEADER2      = 0xA5;

constexpr uint8_t DGUS_CMD_WRITEVAR = 0x82;
constexpr uint8_t DGUS_CMD_READVAR  = 0x83;

constexpr uint32_t DGUS_TIMEOUT = 8000;  // 连接超时时间5秒(屏幕页面同步时间为1秒,考虑safe_delay造成的阻塞)

typedef enum : uint8_t {
  DGUS_IDLE,           //< waiting for DGUS_HEADER1.
  DGUS_HEADER1_SEEN,   //< DGUS_HEADER1 received
  DGUS_HEADER2_SEEN,   //< DGUS_HEADER2 received
  DGUS_WAIT_TELEGRAM,  //< LEN received, Waiting for to receive all bytes.
} rx_datagram_state_t;

// Low-Level access to the display.
class DGUSDisplay {
 public:
  DGUSDisplay() = default;

  static void InitDisplay();

  // Variable access.
  static void WriteVariable(uint16_t adr, const void *values, uint8_t valueslen);
  static void WriteVariable(uint16_t adr, int16_t value);
  static void WriteVariable(uint16_t adr, uint16_t value);
  static void WriteVariable(uint16_t adr, uint8_t value);
  static void WriteVariable(uint16_t adr, int8_t value);
  static void WriteVariable(uint16_t adr, long value);
  static void WriteString(uint16_t adr, const void *values, uint8_t valueslen, bool center = false);
  static void WriteStringPGM(uint16_t adr, const void *values, uint8_t valueslen, bool center = false);
  static void WriteUnicodeString(uint16_t adr, const void *values, uint8_t valueslen, bool center = false);
  static void WriteUnicodeStringPGM(uint16_t adr, const void *values, uint8_t valueslen, bool center = false);
  static void FillRectangle(uint16_t adr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
  static void CutRectangle(uint16_t adr, uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                           uint16_t page);
  static void ClearCanvas(uint16_t adr);
#if ENABLED(CREATBOT_WIFI_SUPPORT)
  static void UpdateWiFiIcon(uint16_t adr, uint16_t value);
#endif

  // Until now I did not need to actively read from the display. That's why there is no ReadVariable
  // (I extensively use the auto upload of the display)

  // Force display into another screen.
  // (And trigger update of containing VPs)
  // (to implement a pop up message, which may not be nested)
  static void RequestScreen(DGUSLCD_Screens screen);

  // Periodic tasks, eg. Rx-Queue handling.
  static void loop();

 public:
  // Helper for users of this class to estimate if an interaction would be blocking.
  static size_t GetFreeTxBuffer();

  // Checks two things: Can we confirm the presence of the display and has we initialized it.
  // (both boils down that the display answered to our chatting)
  static bool isInitialized() { return isConnected; }
  static void setConnectState(bool state) { isConnected = state; }

  // 连接断开重连,用于通知上层应用进行初始化操作,初始化状态需要上层应用取消
  static bool needBoot() { return isBooting; }
  static void setBootState(bool state) { isBooting = state; }

 private:
  static void WriteHeader(uint16_t adr, uint8_t cmd, uint8_t payloadlen);
  static void WritePGM(const char str[], uint8_t len);
  static void ProcessRx();

  static uint32_t            nextSyncTime;
  static rx_datagram_state_t rx_datagram_state;
  static uint8_t             rx_datagram_len;
  static bool                isConnected, isBooting, no_reentrance;
};

extern DGUSDisplay dgusdisplay;

/// Helper to populate a DGUS_VP_Variable for a given VP. Return false if not found.
bool populate_VPVar(const uint16_t VP, const uint16_t match, DGUS_VP_Variable *const ramcopy);
