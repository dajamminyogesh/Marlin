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

#include "../../inc/MarlinConfig.h"

#if ENABLED(CANFILE)

  #include "../../canOpen/canFile/canFile.h"
  #include "../../module/planner.h"
  #include "../../module/printcounter.h"
  #include "../../module/temperature.h"
  #include "../gcode.h"

  #if ENABLED(SET_PROGRESS_MANUALLY)
    #include "../../lcd/marlinui.h"
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    #include "../../feature/powerloss.h"
  #endif

  #if ENABLED(EXTENSIBLE_UI)
    #include "../../lcd/extui/ui_api.h"
  #endif

  /**
   * 打印完成提示音
   */
  #if HAS_SOUND
inline void finishTone() {
  uint8_t i = 5;
  do {
    BUZZ(300, 659);  // mi
    BUZZ(200, 0);
    BUZZ(300, 698);  // fa
    BUZZ(200, 0);
  } while (--i);
}
  #endif

/**
 * M1001: Execute actions for SD print completion
 */
void GcodeSuite::M1001() {
  planner.synchronize();
  canFile.taskStop();

  TERN_(HAS_SOUND, finishTone());

  // Purge the recovery file...
  TERN_(POWER_LOSS_RECOVERY, recovery.purge());

  TERN_(DUAL_X_CARRIAGE, reset_idex_mode());
  TERN_(DUAL_X_CARRIAGE, process_subcommands_now(F("G28X")));

  // Report total print time
  const bool long_print = print_job_timer.duration() > 60;
  if (long_print) process_subcommands_now(F("M31"));

  // Stop the print job timer
  print_job_timer.stop();

  // Set the progress bar "done" state
  TERN_(SET_PROGRESS_PERCENT, ui.set_progress_done());

  // 
  planner.finish_and_disable();

  // Announce SD file completion
  {
    PORT_REDIRECT(SerialMask::All);
    SERIAL_ECHOLNPGM(STR_FILE_PRINTED);
  }

  TERN_(EXTENSIBLE_UI, ExtUI::onPrintDone());
}

#endif  // CANFILE
