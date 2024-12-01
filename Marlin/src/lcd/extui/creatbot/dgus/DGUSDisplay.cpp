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

#include "../../../../inc/MarlinConfigPre.h"

// clang - format on
#if ENABLED(DGUS_LCD_UI_CREATBOT)

  #if HOTENDS > 2
    #warning "More than 2 hotends not implemented on DGUS Display UI."
  #endif

  #include "../../../../inc/MarlinConfig.h"
  #include "../../../../libs/hex_print.h"
  #include "DGUSDisplay.h"
  #include "DGUSDisplayDef.h"

DGUSDisplay dgusdisplay;

void DGUSDisplay::InitDisplay() {
  #ifndef LCD_BAUDRATE
    #define LCD_BAUDRATE 115200
  #endif
  LCD_SERIAL.begin(LCD_BAUDRATE);
}

void DGUSDisplay::WriteVariable(uint16_t adr, const void *values, uint8_t valueslen) {
  if (!isConnected) return;
  if (!values) return;
  const char *data = static_cast<const char *>(values);
  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  while (valueslen--) {
    LCD_SERIAL.write(*data++);
  }
}

void DGUSDisplay::WriteVariable(uint16_t adr, uint16_t value) {
  value = (value & 0xFFU) << 8U | (value >> 8U);
  WriteVariable(adr, static_cast<const void *>(&value), sizeof(uint16_t));
}

void DGUSDisplay::WriteVariable(uint16_t adr, int16_t value) {
  value = ((uint16_t)value & 0xFFU) << 8U | ((uint16_t)value >> 8U);
  WriteVariable(adr, static_cast<const void *>(&value), sizeof(uint16_t));
}

void DGUSDisplay::WriteVariable(uint16_t adr, uint8_t value) {
  WriteVariable(adr, static_cast<const void *>(&value), sizeof(uint8_t));
}

void DGUSDisplay::WriteVariable(uint16_t adr, int8_t value) {
  WriteVariable(adr, static_cast<const void *>(&value), sizeof(int8_t));
}

void DGUSDisplay::WriteVariable(uint16_t adr, long value) {
  union {
    long l;
    char lb[4];
  } endian;
  char tmp[4];
  endian.l = value;
  tmp[0]   = endian.lb[3];
  tmp[1]   = endian.lb[2];
  tmp[2]   = endian.lb[1];
  tmp[3]   = endian.lb[0];
  WriteVariable(adr, static_cast<const void *>(&tmp), sizeof(long));
}

void DGUSDisplay::WriteString(uint16_t adr, const void *values, uint8_t valueslen, bool center) {
  if (!isConnected) return;
  if (!values) return;

  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  const char *data     = static_cast<const char *>(values);

  uint8_t left_spaces  = 0;
  uint8_t right_spaces = valueslen;
  size_t  len          = strlen(data);

  if (len < valueslen) {
    if (center) {
      left_spaces = (valueslen - len) / 2;
    }
    right_spaces = valueslen - len - left_spaces;

    while (left_spaces--) {
      LCD_SERIAL.write(' ');
    }
    while (len--) {
      LCD_SERIAL.write(*data++);
    }
    while (right_spaces--) {
      LCD_SERIAL.write(0x00);
    }
  } else {
    while (valueslen--) {
      LCD_SERIAL.write(*data++);
    }
  }
}

void DGUSDisplay::WriteStringPGM(uint16_t adr, const void *values, uint8_t valueslen, bool center) {
  if (!isConnected) return;
  if (!values) return;

  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  const char *data     = static_cast<const char *>(values);

  uint8_t left_spaces  = 0;
  uint8_t right_spaces = valueslen;
  size_t  len          = strlen_P(data);

  if (len < valueslen) {
    if (center) {
      left_spaces = (valueslen - len) / 2;
    }
    right_spaces = valueslen - len - left_spaces;

    while (left_spaces--) {
      LCD_SERIAL.write(' ');
    }
    while (len--) {
      LCD_SERIAL.write(pgm_read_byte(data++));
    }
    while (right_spaces--) {
      LCD_SERIAL.write(0x00);
    }
  } else {
    while (valueslen--) {
      LCD_SERIAL.write(pgm_read_byte(data++));
    }
  }
}

void DGUSDisplay::WriteUnicodeString(uint16_t adr, const void *values, uint8_t valueslen, bool center) {
  if (!isConnected) return;
  if (!values) return;

  valueslen = (valueslen + 1) / 2 * 2;  // 取偶数
  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  const char *data     = static_cast<const char *>(values);

  uint8_t left_spaces  = 0;
  uint8_t right_spaces = valueslen / 2;
  size_t  len          = 0;
  for (len = 0; (data[len] || data[len + 1]) && (len < 0xFF); len += 2) continue;  // 取偶数

  if (len < valueslen) {
    if (center) {
      left_spaces = (valueslen - len) / 4;
    }
    right_spaces = (valueslen - len - left_spaces * 2) / 2;

    while (left_spaces--) {
      LCD_SERIAL.write(0x30);  // double space
      LCD_SERIAL.write(0x00);
    }
    while (len--) {
      LCD_SERIAL.write(*data++);
    }
    while (right_spaces--) {
      LCD_SERIAL.write(0x00);
      LCD_SERIAL.write(0x00);
    }
  } else {
    while (valueslen--) {
      LCD_SERIAL.write(*data++);
    }
  }
}

void DGUSDisplay::WriteUnicodeStringPGM(uint16_t adr, const void *values, uint8_t valueslen, bool center) {
  if (!isConnected) return;
  if (!values) return;

  valueslen = (valueslen + 1) / 2 * 2;  // 取偶数
  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  const char *data     = static_cast<const char *>(values);

  uint8_t left_spaces  = 0;
  uint8_t right_spaces = valueslen / 2;
  size_t  len          = 0;
  for (len = 0; pgm_read_word(data + len) && (len < 0xFF); len += 2) continue;  // 取偶数

  if (len < valueslen) {
    if (center) {
      left_spaces = (valueslen - len) / 4;
    }
    right_spaces = (valueslen - len - left_spaces * 2) / 2;

    while (left_spaces--) {
      LCD_SERIAL.write(0x30);  // double space
      LCD_SERIAL.write(0x00);
    }
    while (len--) {
      LCD_SERIAL.write(pgm_read_byte(data++));
    }
    while (valueslen - len) {
      LCD_SERIAL.write(0x00);  // double space
      LCD_SERIAL.write(0x00);
    }
  } else {
    while (valueslen--) {
      LCD_SERIAL.write(pgm_read_byte(data++));
    }
  }
}

void DGUSDisplay::ClearCanvas(uint16_t adr) { WriteVariable(adr, int16_t(0)); }

void DGUSDisplay::FillRectangle(uint16_t adr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
  const unsigned char data[] = {// 画一个矩形
                                0x00, 0x04, 0x00, 0x01,
                                // 矩形的左上角坐标
                                (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF), (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF),
                                // 矩形的右下角坐标
                                (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF), (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF),
                                // 画笔颜色
                                (uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
  WriteVariable(adr, data, sizeof(data));
}

void DGUSDisplay::CutRectangle(uint16_t adr, uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                               uint16_t page) {
  const unsigned char data[] = {// 剪切一个矩形
                                0x00, 0x06, 0x00, 0x01,
                                // 要剪切的页面
                                (uint8_t)(page >> 8), (uint8_t)(page & 0xFF),
                                // 被剪切矩形的左上角坐标
                                (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF), (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF),
                                // 被剪切矩形的右下角坐标
                                (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF), (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF),
                                // 粘贴矩形的左上角坐标
                                (uint8_t)(x >> 8), (uint8_t)(x & 0xFF), (uint8_t)(y >> 8), (uint8_t)(y & 0xFF)};
  WriteVariable(adr, data, sizeof(data));
}

  #if ENABLED(CREATBOT_WIFI_SUPPORT)
void DGUSDisplay::UpdateWiFiIcon(uint16_t adr, uint16_t value) { WriteVariable(adr, value); }
  #endif

void DGUSDisplay::RequestScreen(DGUSLCD_Screens screen) {
  DEBUG_ECHOLNPGM("GotoScreen ", screen);
  const unsigned char gotoscreen[] = {0x5A, 0x01, (unsigned char)(screen >> 8U), (unsigned char)(screen & 0xFFU)};
  WriteVariable(0x84, gotoscreen, sizeof(gotoscreen));
}

void DGUSDisplay::loop() {
  if (ELAPSED(millis(), nextSyncTime)) isConnected = false;
  // protect against recursion… ProcessRx() may indirectly call idle() when injecting gcode commands.
  if (!no_reentrance) {
    no_reentrance = true;
    ProcessRx();
    no_reentrance = false;
  }
}

size_t DGUSDisplay::GetFreeTxBuffer() { return LCD_SERIAL_TX_BUFFER_FREE(); }

void DGUSDisplay::WriteHeader(uint16_t adr, uint8_t cmd, uint8_t payloadlen) {
  if (!isConnected) return;
  LCD_SERIAL.write(DGUS_HEADER1);
  LCD_SERIAL.write(DGUS_HEADER2);
  LCD_SERIAL.write(payloadlen + 3);
  LCD_SERIAL.write(cmd);
  LCD_SERIAL.write(adr >> 8);
  LCD_SERIAL.write(adr & 0xFF);
}

void DGUSDisplay::WritePGM(const char str[], uint8_t len) {
  if (!isConnected) return;
  while (len--) LCD_SERIAL.write(pgm_read_byte(str++));
}

void DGUSDisplay::ProcessRx() {
  #if ENABLED(SERIAL_STATS_RX_BUFFER_OVERRUNS)
  if (!LCD_SERIAL.available() && LCD_SERIAL.buffer_overruns()) {
    // Overrun, but reset the flag only when the buffer is empty
    // We want to extract as many as valid datagrams possible...
    DEBUG_ECHOPGM("OVFL");
    rx_datagram_state = DGUS_IDLE;
    // LCD_SERIAL.reset_rx_overun();
    LCD_SERIAL.flush();
  }
  #endif

  uint8_t receivedbyte;
  while (LCD_SERIAL.available()) {
    switch (rx_datagram_state) {
      case DGUS_IDLE:  // Waiting for the first header byte
        receivedbyte = LCD_SERIAL.read();
        // DEBUG_ECHOPGM("< ",x);
        if (DGUS_HEADER1 == receivedbyte) rx_datagram_state = DGUS_HEADER1_SEEN;
        break;

      case DGUS_HEADER1_SEEN:  // Waiting for the second header byte
        receivedbyte = LCD_SERIAL.read();
        // DEBUG_ECHOPGM(" ",x);
        rx_datagram_state = (DGUS_HEADER2 == receivedbyte) ? DGUS_HEADER2_SEEN : DGUS_IDLE;
        break;

      case DGUS_HEADER2_SEEN:  // Waiting for the length byte
        rx_datagram_len = LCD_SERIAL.read();
        DEBUG_ECHOPGM(" (", rx_datagram_len, ") ");

        // Telegram min len is 3 (command and one word of payload)
        rx_datagram_state = WITHIN(rx_datagram_len, 3, DGUS_RX_BUFFER_SIZE) ? DGUS_WAIT_TELEGRAM : DGUS_IDLE;
        break;

      case DGUS_WAIT_TELEGRAM:  // wait for complete datagram to arrive.
        if (LCD_SERIAL.available() < rx_datagram_len) return;

        if (!isConnected) {
          isConnected = true;  // We've talked to it, so we defined it as initialized.
          isBooting   = true;
        }
        nextSyncTime    = millis() + DGUS_TIMEOUT;
        uint8_t command = LCD_SERIAL.read();

        DEBUG_ECHOPGM("# ", hex_byte(command));

        uint8_t        readlen = rx_datagram_len - 1;  // command is part of len.
        unsigned char  tmp[rx_datagram_len - 1];
        unsigned char *ptmp = tmp;
        while (readlen--) {
          receivedbyte = LCD_SERIAL.read();
          DEBUG_ECHOPGM(" ", hex_byte(receivedbyte));
          *ptmp++ = receivedbyte;
        }
        DEBUG_ECHOPGM(" # ");
        // mostly we'll get this: 5A A5 03 82 4F 4B -- ACK on 0x82, so discard it.
        if (command == DGUS_CMD_WRITEVAR && 'O' == tmp[0] && 'K' == tmp[1]) {
          DEBUG_ECHOLNPGM(">");
          rx_datagram_state = DGUS_IDLE;
          break;
        }

        /* AutoUpload, (and answer to) Command 0x83 :
        |      tmp[0  1  2  3  4 ... ]
        | Example 5A A5 06 83 20 01 01 78 01 ……
        |          / /  |  |   \ /   |  \     \
        |        Header |  |    |    |   \_____\_ DATA (Words!)
        |     DatagramLen  /  VPAdr  |
        |           Command          DataLen (in Words) */
        if (command == DGUS_CMD_READVAR) {
          const uint16_t vp   = tmp[0] << 8 | tmp[1];
          const uint8_t  size = tmp[2] << 1;
          const uint16_t val  = tmp[3] << 8 | tmp[4];

          DGUS_VP_Variable ramcopy;
          if (populate_VPVar(vp, val, &ramcopy)) {
            if (ramcopy.rx_handler) {
              if (ramcopy.size) {
                ramcopy.rx_handler(ramcopy, &tmp[3]);
              } else {
                ramcopy.rx_handler(ramcopy, nullptr);
              }
            } else
              DEBUG_ECHOLNPGM(" VPVar found, no handler.");
          } else
            DEBUG_ECHOLNPGM(" VPVar not found:", vp);

          rx_datagram_state = DGUS_IDLE;
          break;
        }

        // discard anything else
        rx_datagram_state = DGUS_IDLE;
    }
  }
}

uint32_t            DGUSDisplay::nextSyncTime      = 0;
rx_datagram_state_t DGUSDisplay::rx_datagram_state = DGUS_IDLE;
uint8_t             DGUSDisplay::rx_datagram_len   = 0;
bool                DGUSDisplay::isConnected       = false;
bool                DGUSDisplay::isBooting         = false;
bool                DGUSDisplay::no_reentrance     = false;

bool populate_VPVar(const uint16_t VP, const uint16_t match, DGUS_VP_Variable *const ramcopy) {
  const DGUS_VP_Variable *ret = ListOfVP;
  do {
    const uint16_t vpcheck    = pgm_read_word(&(ret->VP));
    const uint16_t sizecheck  = pgm_read_word(&(ret->size));
    const uint16_t matchcheck = pgm_read_word(&(ret->match));
    if (vpcheck == 0) break;
    if (vpcheck == VP) {
      if (sizecheck || (matchcheck == match)) {
        memcpy_P(ramcopy, ret, sizeof(*ret));
        return true;
      }
    }
  } while (++ret);

  DEBUG_ECHOLNPGM("FindVPVar NOT FOUND ", VP);
  return false;
}

#endif  // HAS_DGUS_LCD_CLASSIC
