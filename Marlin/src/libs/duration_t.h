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

#include "../HAL/shared/Marduino.h"

struct duration_t {
  /**
   * @brief Duration is stored in seconds
   */
  uint32_t value;

  /**
   * @brief Constructor
   */
  duration_t()
    : duration_t(0) {};

  /**
   * @brief Constructor
   *
   * @param seconds The number of seconds
   */
  duration_t(uint32_t const &seconds) {
    this->value = seconds;
  }

  /**
   * @brief Equality comparison
   * @details Overloads the equality comparison operator
   *
   * @param value The number of seconds to compare to
   * @return True if both durations are equal
   */
  bool operator==(const uint32_t &value) const {
    return (this->value == value);
  }

  /**
   * @brief Inequality comparison
   * @details Overloads the inequality comparison operator
   *
   * @param value The number of seconds to compare to
   * @return False if both durations are equal
   */
  bool operator!=(const uint32_t &value) const {
    return ! this->operator==(value);
  }

  /**
   * @brief Format the duration as years
   * @return The number of years
   */
  inline uint8_t year() const {
    return this->day() / 365;
  }

  /**
   * @brief Format the duration as days
   * @return The number of days
   */
  inline uint16_t day() const {
    return this->hour() / 24;
  }

  /**
   * @brief Format the duration as hours
   * @return The number of hours
   */
  inline uint32_t hour() const {
    return this->minute() / 60;
  }

  /**
   * @brief Format the duration as minutes
   * @return The number of minutes
   */
  inline uint32_t minute() const {
    return this->second() / 60;
  }

  /**
   * @brief Format the duration as seconds
   * @return The number of seconds
   */
  inline uint32_t second() const {
    return this->value;
  }

  #pragma GCC diagnostic push
  #if GCC_VERSION <= 50000
    #pragma GCC diagnostic ignored "-Wformat-overflow"
  #endif

  /**
   * @brief Format the duration as a string
   * @details String will be formatted using a "full" representation of duration
   *
   * @param buffer The array pointed to must be able to accommodate 22 bytes
   *               (21 for the string, 1 more for the terminating nul)
   * @param dense Whether to skip spaces in the resulting string
   *
   * Output examples:
   *  123456789012345678901 (strlen)
   *  135y 364d 23h 59m 59s
   *  364d 23h 59m 59s
   *  23h 59m 59s
   *  59m 59s
   *  59s
   */
  char* toString(char * const buffer) const {
    const uint16_t y = this->year(),
                   d = this->day() % 365,
                   h = this->hour() % 24,
                   m = this->minute() % 60,
                   s = this->second() % 60;

         if (y) sprintf_P(buffer, PSTR("%iy %id %ih %im %is"), y, d, h, m, s);
    else if (d) sprintf_P(buffer, PSTR("%id %ih %im %is"), d, h, m, s);
    else if (h) sprintf_P(buffer, PSTR("%ih %im %is"), h, m, s);
    else if (m) sprintf_P(buffer, PSTR("%im %is"), m, s);
    else sprintf_P(buffer, PSTR("%is"), s);
    return buffer;
  }

  /**
   * @brief Format the duration as a compact string
   * @details String will be formatted using a "full" representation of duration
   *
   * @param buffer The array pointed to must be able to accommodate 18 bytes
   *               (17 for the string, 1 more for the terminating nul)
   * @param dense Whether to skip spaces in the resulting string
   *
   * Output examples:
   *  12345678901234567 (strlen)
   *  135y364d23h59m59s
   *  364d23h59m59s
   *  23h59m59s
   *  59m59s
   *  59s
   */
  char* toCompactString(char * const buffer) const {
    const uint16_t y = this->year(),
                   d = this->day() % 365,
                   h = this->hour() % 24,
                   m = this->minute() % 60,
                   s = this->second() % 60;

         if (y) sprintf_P(buffer, PSTR("%iy%id%ih%im%is"), y, d, h, m, s);
    else if (d) sprintf_P(buffer, PSTR("%id%ih%im%is"), d, h, m, s);
    else if (h) sprintf_P(buffer, PSTR("%ih%im%is"), h, m, s);
    else if (m) sprintf_P(buffer, PSTR("%im%is"), m, s);
    else sprintf_P(buffer, PSTR("%is"), s);
    return buffer;
  }

  /**
   * @brief Format the duration as a string
   * @details String will be formatted using a "digital" representation of duration
   *
   * @param buffer The array pointed to must be able to accommodate 10 bytes
   * @return length of the formatted string (without terminating nul)
   *
   * Output examples:
   *  123456789 (strlen)
   *  12'34
   *  99:59
   *  123:45
   *  1d 12:33
   *  9999d 12:33
   */
  uint8_t toDigital(char *buffer, bool with_days=false) const {
    const uint16_t h = uint16_t(this->hour()),
                   m = uint16_t(this->minute() % 60UL);
    if (with_days) {
      const uint16_t d = this->day();
      sprintf_P(buffer, PSTR("%hud %02hu:%02hu"), d, h % 24, m);  // 1d 23:45
      return strlen_P(buffer);
    }
    else if (!h) {
      const uint16_t s = uint16_t(this->second() % 60UL);
      sprintf_P(buffer, PSTR("%02hu'%02hu"), m, s);     // 12'34
      return 5;
    }
    else if (h < 100) {
      sprintf_P(buffer, PSTR("%02hu:%02hu"), h, m);     // 12:34
      return 5;
    }
    else {
      sprintf_P(buffer, PSTR("%hu:%02hu"), h, m);       // 123:45
      return 6;
    }
  }

  /**
   * @Autor CreatBot LYN
   * @param buffer The array pointed to must be able to accommodate 13 bytes
   *
   * Output examples:
   *  123456789012 (strlen)
   *  [9999:59:59]
   *  <99:59:59>
   */
  void toTimeREG(char *buffer, char prefixChar = '\0', char suffixChar = '\0') const {
    uint16_t h = this->hour() % 10000, m = this->minute() % 60, s = this->second() % 60;
    if ((prefixChar == '\0') || (suffixChar == '\0'))
      sprintf_P(buffer, PSTR("%u:%02u:%02u"), h, m, s);
    else
      sprintf_P(buffer, PSTR("%c%u:%02u:%02u%c"), prefixChar, h, m, s, suffixChar);
  }

  /**
   * @Autor CreatBot LYN
   * @param buffer The array pointed to must be able to accommodate 9 bytes
   *        Also, 11 bytes is necessary when h_len is 3.
   *
   * Output examples:
   *  1234567890 (strlen)
   *  9990995959      ->    999:59:59
   *  8120125959      ->    812:59:59
   *  99995959        ->    9999:59:59
   *  010101          ->    01:01:01
   */
  void toTimeDWIN(char *buffer, uint8_t h_len) const {
    uint16_t h = this->hour() % 10000, m = this->minute() % 60, s = this->second() % 60;

    if (h_len == 4) {
      buffer[0] = h / 1000 % 10 << 4 & 0xF0;
      buffer[0] |= h / 100 % 10 & 0x0F;
      buffer[1] = h / 10 % 10 << 4 & 0xF0;
      buffer[1] |= h % 10 & 0x0F;
      buffer[2] = (m / 10 << 4) & 0xF0;
      buffer[2] |= m % 10 & 0x0F;
      buffer[3] = (s / 10 << 4) & 0xF0;
      buffer[3] |= s % 10 & 0x0F;
    } else if (h_len == 2) {
      sprintf_P(buffer, PSTR("%02u%02u%02u"), h % 100, m, s);
    } else if (h_len == 3) {
      buffer[0] = h / 100 % 10 << 4 & 0xF0;
      buffer[0] |= h / 10 % 10 & 0x0F;
      buffer[1] = h % 10 << 4 & 0xF0;
      buffer[1] |= 0xF0;
      buffer[2] = (h % 100) / 10 << 4 & 0xF0;
      buffer[2] |= (h % 100) % 10 & 0x0F;
      buffer[3] = (m / 10 << 4) & 0xF0;
      buffer[3] |= m % 10 & 0x0F;
      buffer[4] = (s / 10 << 4) & 0xF0;
      buffer[4] |= s % 10 & 0x0F;
    } else
      sprintf_P(buffer, PSTR("%u%02u%02u"), h, m, s);
  }

  /**
   * @Autor CreatBot LYN
   * @param buffer The array pointed to must be able to accommodate 6 bytes
   *
   * Output examples:
   * 12345 (strlen)
   * 02:59
   */
  void toTimeShutDown(char *buffer) const {
    uint16_t m = this->minute() % 60, s = this->second() % 60;
    sprintf_P(buffer, PSTR("%02u:%02u"), m, s);
  }

  #pragma GCC diagnostic pop
};
