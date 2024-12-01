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
 * feature/powerloss.h - Resume an SD print after power-loss
 */

#include "../sd/cardreader.h"
#include "../gcode/gcode.h"

#include "../inc/MarlinConfig.h"

#if ENABLED(GCODE_REPEAT_MARKERS)
  #include "../feature/repeat.h"
#endif

#if ENABLED(MIXING_EXTRUDER)
  #include "../feature/mixing.h"
#endif

#if ENABLED(DUAL_X_CARRIAGE)
  #include "../module/motion.h"
#endif

#if !defined(POWER_LOSS_STATE) && PIN_EXISTS(POWER_LOSS)
  #define POWER_LOSS_STATE HIGH
#endif

#ifndef POWER_LOSS_ZRAISE
  #define POWER_LOSS_ZRAISE 2 // Default Z-raise on outage or resume
#endif

//#define DEBUG_POWER_LOSS_RECOVERY
//#define SAVE_EACH_CMD_MODE
//#define SAVE_INFO_INTERVAL_MS 0

#pragma pack (1)

typedef struct {
  uint8_t valid_head;

  // Machine state
  xyze_pos_t current_position;
  uint16_t feedrate;

  float zraise;

  // Repeat information
  #if ENABLED(GCODE_REPEAT_MARKERS)
    Repeat stored_repeat;
  #endif

  #if HAS_HOME_OFFSET
    xyz_pos_t home_offset;
  #endif
  #if HAS_POSITION_SHIFT
    xyz_pos_t position_shift;
  #endif
  #if HAS_MULTI_EXTRUDER
    uint8_t active_extruder;
  #endif

  #if ENABLED(DUAL_X_CARRIAGE)
    DualXMode  dual_x_mode;
    float      dual_x_offset;
    celsius_t  dual_temp_offset;
  #endif

  #if DISABLED(NO_VOLUMETRICS)
    float filament_size[EXTRUDERS];
  #endif

  #if HAS_HOTEND
    celsius_t target_temperature[HOTENDS];
  #endif
  #if HAS_HEATED_BED
    celsius_t target_temperature_bed;
  #endif
  #if HAS_FAN
    uint8_t fan_speed[FAN_COUNT];
  #endif

  #if HAS_LEVELING
    float fade;
  #endif

  #if ENABLED(FWRETRACT)
    float retract[EXTRUDERS], retract_hop;
  #endif

  // Mixing extruder and gradient
  #if ENABLED(MIXING_EXTRUDER)
    //uint_fast8_t selected_vtool;
    //mixer_comp_t color[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];
    #if ENABLED(GRADIENT_MIX)
      gradient_t gradient;
    #endif
  #endif

  // Filename and position
  #if ENABLED(SDSUPPORT)
  char sd_filename[MAXPATHNAMELENGTH];
  volatile uint32_t sdpos;
  #elif ENABLED(CANFILE)
  volatile uint32_t canfilepos;
  #endif

  // Job elapsed time
  millis_t print_job_elapsed;

  // Relative axis modes
  relative_t axis_relative;

  // Misc. Marlin flags
  struct {
    bool raised:1;                // Raised before saved
    bool dryrun:1;                // M111 S8
    bool allow_cold_extrusion:1;  // M302 P1
    #if HAS_LEVELING
      bool leveling:1;            // M420 S
    #endif
    #if DISABLED(NO_VOLUMETRICS)
      bool volumetric_enabled:1;  // M200 S D
    #endif
  } flag;

  uint8_t valid_foot;

  bool valid() { return valid_head && valid_head == valid_foot && valid_head != 0xff; }

} job_recovery_info_t;

#pragma pack ()

class PrintJobRecovery {
  public:
  #if ENABLED(SDSUPPORT)
    static const char filename[5];

    static MediaFile file;
    static uint32_t cmd_sdpos,        //!< SD position of the next command
                    sdpos[BUFSIZE];   //!< SD positions of queued commands
  #elif ENABLED(CANFILE)
    static uint32_t cmd_canfilepos,        //!< canFile position of the next command
                    canfilepos[BUFSIZE];   //!< canFile positions of queued commands
    static bool state;
  #endif

    static job_recovery_info_t info;

    static uint8_t queue_index_r;     //!< Queue index of the active command

    static bool recoveryPrepare;

    #if HAS_DWIN_E3V2_BASIC
      static bool dwin_flag;
    #endif

    static void init();

    #if ENABLED(SDSUPPORT)
      static void prepare();
    #endif

    static void setup() {
      #if PIN_EXISTS(OUTAGECON)
        OUT_WRITE(OUTAGECON_PIN, HIGH);
      #endif
      #if PIN_EXISTS(POWER_LOSS)
        #if ENABLED(POWER_LOSS_PULLUP)
          SET_INPUT_PULLUP(POWER_LOSS_PIN);
        #elif ENABLED(POWER_LOSS_PULLDOWN)
          SET_INPUT_PULLDOWN(POWER_LOSS_PIN);
        #else
          SET_INPUT(POWER_LOSS_PIN);
        #endif
      #endif
    }

    // Track each command's file offsets
    #if ENABLED(SDSUPPORT)
      static uint32_t command_sdpos() { return sdpos[queue_index_r]; }
      static void sync_sdpos() { sdpos[queue_index_r] = info.sdpos; }
      static void commit_sdpos(const uint8_t index_w) { sdpos[index_w] = cmd_sdpos; }

      static bool exists() { return card.jobRecoverFileExists(); }
      static void open(const bool read) { card.openJobRecoveryFile(read); }
      static void close() { file.close(); }
    #elif ENABLED(CANFILE)
      static uint32_t command_canfilepos() { return canfilepos[queue_index_r]; }
      static void sync_canfilepos() { canfilepos[queue_index_r] = info.canfilepos; }
      static void commit_canfilepos(const uint8_t index_w) { canfilepos[index_w] = cmd_canfilepos; }
      static void handleCanfile(const bool onoff);
    #endif 
    
    static void sync_curPos();

    static bool enabled;
    static void enable(const bool onoff);
    static void changed();

    static bool check();
    static void resume();
    static void purge();

    static void cancel() { purge(); }

    static void load();
    static void save(const bool force=ENABLED(SAVE_EACH_CMD_MODE), const float zraise=POWER_LOSS_ZRAISE, const bool raised=false);

    #if PIN_EXISTS(POWER_LOSS)
      static void outageAction();
      static void outage() {
        static constexpr uint8_t OUTAGE_THRESHOLD = 3;
        static uint8_t outage_counter = 0;
        if (enabled && READ(POWER_LOSS_PIN) == POWER_LOSS_STATE) {
          if (outage_counter < UINT8_MAX) outage_counter++;
          if (outage_counter == OUTAGE_THRESHOLD) outageAction();
        }
        else
          outage_counter = 0;
      }
    #endif

    // The referenced file exists
    #if ENABLED(SDSUPPORT)
      static bool interrupted_file_exists() { return card.fileExists(info.sd_filename); }

      #if ENABLED(RECOVERY_USE_EEPROM)
        static bool valid() { return info.valid(); }
      #else
        static bool valid() { return info.valid() && interrupted_file_exists(); }
      #endif
    #elif ENABLED(CANFILE)
      static bool valid() { return info.valid(); }
    #endif

    #if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
      static void debug(FSTR_P const prefix);
    #else
      static void debug(FSTR_P const) {}
    #endif

  private:
    static void write();

    #if ENABLED(BACKUP_POWER_SUPPLY)
      static void retract_and_lift(const_float_t zraise);
    #endif

    #if PIN_EXISTS(POWER_LOSS) || ENABLED(DEBUG_POWER_LOSS_RECOVERY)
      friend class GcodeSuite;
      static void _outage(TERN_(DEBUG_POWER_LOSS_RECOVERY, const bool simulated=false));
    #endif
};

extern PrintJobRecovery recovery;
