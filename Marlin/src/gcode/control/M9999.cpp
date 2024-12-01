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

#include "../gcode.h"

#include "../../MarlinCore.h"   // for SystemResetToBootloader
#include "../../lcd/marlinui.h" // for ui and buzzer
#if ENABLED(CREATBOT_WIFI_SUPPORT)
  #include "../../feature/wifi/WiFiSocket.h"
#endif

/**
 * M9999: Reboot into DFU mode
 *
 * Reboot into DFU mode and Update the program
 *
 *
 */
void GcodeSuite::M9999() {
  ui.init();

#if ENABLED(CREATBOT_WIFI_SUPPORT)
  myWifi.dealySYNC();
#endif

#if HAS_SOUND
  BUZZ(300, 659);  // mi
  BUZZ(100, 0);
  BUZZ(100, 698);  // fa
  BUZZDONE(1000);
#endif
  SystemResetToBootloader();
}
