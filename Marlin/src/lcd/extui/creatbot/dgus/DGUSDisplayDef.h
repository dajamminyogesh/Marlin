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

#include "DGUSDisplayMacro.h"

#ifndef DGUS_BUILD_VERSION
  #if ENABLED(CreatBot_DGUS_V5)
    #define DGUS_BUILD_VERSION "5.3.2"
  #elif ENABLED(CreatBot_DGUS_V6)
    #define DGUS_BUILD_VERSION "6.0.3"
  #else
    #error "Please configure CreatBot_DGUS version!"
  #endif
#endif

extern const char MarlinVersion[11];

enum DGUSLCD_Screens : uint8_t {
  DGUSLCD_SCREEN_BOOT                                = 0,
  DGUSLCD_SCREEN_BOOT_END                            = 39,
  DGUSLCD_SCREEN_DEFAULT_DELAY                       = 40,
  DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT  = PAGE_PARSE_DEFAULT(PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT),
  DGUSLCD_SCREEN_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT = PAGE_PARSE_DEFAULT(PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT),
  DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE  = PAGE_PARSE_DEFAULT(PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE),
  DGUSLCD_SCREEN_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE = PAGE_PARSE_DEFAULT(PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE),
  DGUSLCD_SCREEN_DEFAULT_PRINT_PAUSE                 = PAGE_PARSE_DEFAULT(PAGE_DEFAULT_PRINT_PAUSE),
  DGUSLCD_SCREEN_DEFAULT_PRINT_RESUME                = PAGE_PARSE_DEFAULT(PAGE_DEFAULT_PRINT_RESUME),
  DGUSLCD_SCREEN_DEFAULT_ONLINE                      = PAGE_PARSE_DEFAULT(PAGE_DEFAULT_ONLINE),

  DGUSLCD_SCREEN_MOVE                                = PAGE_MOVE,
  DGUSLCD_SCREEN_FILAMENT                            = PAGE_PARSE_DEFAULT(PAGE_FILAMENT),

  DGUSLCD_SCREEN_FILE_ONE                            = PAGE_FILE_ONE,
  DGUSLCD_SCREEN_FILE_NEXT                           = PAGE_FILE_NEXT,
  DGUSLCD_SCREEN_FILE_LAST                           = PAGE_FILE_LAST,
  DGUSLCD_SCREEN_FILE_BOTH                           = PAGE_FILE_BOTH,
  DGUSLCD_SCREEN_FILE_UP_ONE                         = PAGE_FILE_UP_ONE,
  DGUSLCD_SCREEN_FILE_UP_NEXT                        = PAGE_FILE_UP_NEXT,
  DGUSLCD_SCREEN_FILE_UP_LAST                        = PAGE_FILE_UP_LAST,
  DGUSLCD_SCREEN_FILE_UP_BOTH                        = PAGE_FILE_UP_BOTH,

  DGUSLCD_SCREEN_ADJUST                              = PAGE_ADJUST,
  DGUSLCD_SCREEN_SETTING                             = PAGE_PARSE_SETTING,

  DGUSLCD_SCREEN_MOTION_SETTING                      = PAGE_MOTION_SETTING,
  DGUSLCD_SCREEN_FANSPEED_SETTING                    = PAGE_PARSE_FANSPEED_SETTING,
  DGUSLCD_SCREEN_PREHEAT_SETTING                     = PAGE_PARSE_PREHEAT_SETTING,
  DGUSLCD_SCREEN_WIFI_SETTING_OFF                    = PAGE_WIFI_SETTING_OFF,
  DGUSLCD_SCREEN_WIFI_SETTING_ON                     = PAGE_WIFI_SETTING_ON,
  DGUSLCD_SCREEN_LEVELING_SETTING_OFF                = PAGE_LEVELING_SETTING_OFF,
  DGUSLCD_SCREEN_LEVELING_SETTING_ON                 = PAGE_LEVELING_SETTING_ON,
  DGUSLCD_SCREEN_LEVELING_SETTING_DISABLE            = PAGE_LEVELING_SETTING_DISABLE,
  DGUSLCD_SCREEN_MORESETTING                         = PAGE_MORE_SETTING,
  DGUSLCD_SCREEN_REG                                 = PAGE_REG,
  DGUSLCD_SCREEN_PRINTER_INFO                        = PAGE_INFO_PRINTER,
  DGUSLCD_SCREEN_WIFI_INFO_STATION                   = PAGE_WIFI_INFO_STATION,
  DGUSLCD_SCREEN_WIFI_INFO_AP                        = PAGE_WIFI_INFO_AP,
  DGUSLCD_SCREEN_PRINT_CONFIRM                       = PAGE_PRINT_CONFIRM,
  DGUSLCD_SCREEN_SHUTDOWN_HOTTEMP                    = PAGE_SHUTDOWN_HOTTEMP,
  DGUSLCD_SCREEN_NOZZLE_HEATING                      = PAGE_NOZZLE_HEATING,
  DGUSLCD_SCREEN_UNFINISH_CHOOSE                     = PAGE_UNFINISH_CHOOSE,
  DGUSLCD_SCREEN_CONNECT_WIFI_12                     = PAGE_CONNECT_WIFI_12,
  DGUSLCD_SCREEN_CONNECT_WIFI_01                     = PAGE_CONNECT_WIFI_01,

  DGUSLCD_SCREEN_POP_0                               = PAGE_POP_0,
  DGUSLCD_SCREEN_POP_1                               = PAGE_POP_1,
  DGUSLCD_SCREEN_POP_2                               = PAGE_POP_2,
};

// T5UID 变量地址 布局
// 0x0000 .. 0x0FFF -- DGUS系统保留变量
// 0x1000 .. 0x17FF -- 保留用于曲线空间0
// 0x1800 .. 0x1FFF -- 保留用于曲线空间1
// 0x2000 .. 0x27FF -- 保留用于曲线空间2
// 0x2800 .. 0x2FFF -- 保留用于曲线空间3
// 0x3000 .. 0x37FF -- 保留用于曲线空间4
// 0x3800 .. 0x3FFF -- 保留用于曲线空间5
// 0x4000 .. 0x47FF -- 保留用于曲线空间6
// 0x4800 .. 0x4FFF -- 保留用于曲线空间7
// 0x5000 .. 0x5FFF -- 一般数据变量地址
// 0x6000 .. 0x6FFF -- 文件列表项字符
// 0x7000 .. 0xEFFF -- 数据变量配置地址(暂未使用)
// 0xF000 .. 0xFFFF -- 保留区域不使用

constexpr uint16_t VP_T_E0_Tar                = 0x5000;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_E1_Tar                = 0x5001;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_E2_Tar                = 0x5002;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_E0_Cur                = 0x5003;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_E1_Cur                = 0x5004;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_E2_Cur                = 0x5005;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_Bed_Tar               = 0x5006;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_Bed_Cur               = 0x5007;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_Chamber_Tar           = 0x5008;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_Chamber_Cur           = 0x5009;  // 2 bytes  (3.0)
constexpr uint16_t VP_Feedrate_Percentage     = 0x500B;  // 2 bytes  (3.0)
constexpr uint16_t VP_Fan0_Percentage         = 0x500C;  // 2 bytes  (3.0)
constexpr uint16_t VP_Flowrate_E              = 0x500D;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_Fan             = 0x500E;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_E0            = 0x500F;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_E1            = 0x501A;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_E2            = 0x501B;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_Bed           = 0x5010;  // 2 bytes  (3.0)
constexpr uint16_t VP_X_STEP_PER_MM           = 0x5011;  // 4 bytes  (3.4)
constexpr uint16_t VP_Y_STEP_PER_MM           = 0x5013;  // 4 bytes  (3.4)
constexpr uint16_t VP_Z_STEP_PER_MM           = 0x5015;  // 4 bytes  (4.3)
constexpr uint16_t VP_E_STEP_PER_MM           = 0x5017;  // 4 bytes  (3.4)
constexpr uint16_t VP_PreHeat_T_Chamber       = 0x5019;  // 2 bytes  (3.0)
constexpr uint16_t VP_CaseLight               = 0x501C;  // 2 bytes  (1.0)  (1,2,3,4 => on, off, dis_on, dis_off)
constexpr uint16_t VP_AutoPowerOff            = 0x501D;  // 2 bytes  (1.0)  (1,2,3,4 => on, off, dis_on, dis_off)
constexpr uint16_t VP_AutoBackMain            = 0x501E;  // 2 bytes  (1.0)  (1,2,3,4 => on, off, dis_on, dis_off)
constexpr uint16_t VP_Move_Unit               = 0x5020;  // 2 bytes  (1.0)  (1,2,3,4 => 0.1mm, 1mm, 10mm, 100mm)
constexpr uint16_t VP_Activated_Extruder      = 0x5021;  // 2 bytes  (1.0)  (1=2=4 => Ext0; 3=5 => Ext1; 6 => Ext2)
constexpr uint16_t VP_Filament_Length         = 0x5022;  // 2 bytes  (3.0)
constexpr uint16_t VP_PrintPercent            = 0x5023;  // 2 bytes  (3.1)
constexpr uint16_t VP_PrintTime               = 0x5024;  // 5 bytes  (HEX)  (ddd:hh:mm)
constexpr uint16_t VP_UsedTime                = 0x5027;  // 4 bytes  (HEX)  (hhhh:mm:ss)
constexpr uint16_t VP_TrialPeriod             = 0x5029;  // 4 bytes  (HEX)  (hhhh:mm:ss)
constexpr uint16_t VP_RegKey                  = 0x502B;  // 2 bytes  (3.2)
constexpr uint16_t VP_RegState                = 0x502C;  // 2 bytes  (1.0)  (0,1 => invalid, success)

constexpr uint16_t VP_FW_Ver                  = 0x5030;  // 16 bytes =  8 words
constexpr uint16_t VP_UI_Ver                  = 0x5038;  // 16 bytes =  8 words
constexpr uint16_t VP_MSG_M117                = 0x5040;  // 32 bytes = 16 words
constexpr uint16_t VP_PrintProgessCanvas      = 0x5050;  // 16 bytes =  8 words
constexpr uint16_t VP_PrintProgessBrush       = 0x5058;  // 16 bytes =  8 words
constexpr uint16_t VP_PrintProgess            = 0x5500;  // 512 bytes = 256 words
constexpr uint16_t VP_X_STR_ADDR              = 0x5060;  // 2 bytes =  1 word
constexpr uint16_t VP_Y_STR_ADDR              = 0x5061;  // 2 bytes =  1 word
constexpr uint16_t VP_Z_STR_ADDR              = 0x5062;  // 2 bytes =  1 word
constexpr uint16_t VP_SN_STR                  = 0x5130;  // 16 bytes =  8 words
  #if ENABLED(CreatBot_DGUS_V5)
constexpr uint16_t VP_MSG_M117_SHORT          = 0x5140;  // 32 bytes = 16 words
  #elif ENABLED(CreatBot_DGUS_V6)
constexpr uint16_t VP_MSG_M117_ROLL           = 0x5143;  // 26 bytes = 13 words
  #endif
constexpr uint16_t VP_T_ACT_Tar               = 0x5150;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_ACT_Cur               = 0x5151;  // 2 bytes  (3.0)

constexpr uint16_t VP_T_MAX_Cur               = 0x5070;  // 2 bytes  (3.0)
constexpr uint16_t VP_FanT_Percentage         = 0x5071;  // 2 bytes  (3.0)
constexpr uint16_t VP_Fan1_Percentage         = 0x5072;  // 2 bytes  (3.0)
constexpr uint16_t VP_XPos                    = 0x5073;  // 4 bytes  (4.2)
constexpr uint16_t VP_YPos                    = 0x5075;  // 4 bytes  (4.2)
constexpr uint16_t VP_ZPos                    = 0x5077;  // 4 bytes  (4.2)
constexpr uint16_t VP_ServoOffsetZ            = 0x5079;  // 2 bytes  (2.2)
constexpr uint16_t VP_FadeHeight              = 0x507A;  // 2 bytes  (3.1)
constexpr uint16_t VP_WIFI_MODE_INDEX_ADDR    = 0x507B;  // 2 bytes  (1.0)  (0,1 => STA, AP)
constexpr uint16_t VP_WIFI_STATE_INDEX_ADDR   = 0x507C;  // 2 btyes  (1.0)  (0,1,2 => connected, connecting, no_connect)
constexpr uint16_t VP_WIFI_SCAN_LAST_ICO_ADDR = 0x507D;  // 2 bytes  (1.0)  (0,1 => HIDE,SHOW)
constexpr uint16_t VP_WIFI_SCAN_NEXT_ICO_ADDR = 0x507E;  // 2 bytes  (1.0)  (0,1 => HIDE,SHOW)

constexpr uint16_t VP_WIFI_IP_ADDR            = 0x5080;  // 32 bytes = 16 words
constexpr uint16_t VP_WIFI_SSID_ADDR          = 0x5090;  // 32 bytes = 16 words
constexpr uint16_t VP_WIFI_KEY_ADDR           = 0x50A0;  // 32 bytes = 16 words
constexpr uint16_t VP_WIFI_KEY_MASK_ADDR      = 0x50B0;  // 16 bytes (****************)
constexpr uint16_t VP_WIFI_UUID_ADDR          = 0x50B8;  // 16 bytes (991234xxxxxxxxx)
  #if ENABLED(CreatBot_DGUS_V5)
constexpr uint16_t VP_WIFI_SSID_SET_ADDR      = 0x50C0;  // 32 bytes = 16 words
  #elif ENABLED(CreatBot_DGUS_V6)
constexpr uint16_t VP_WIFI_SSID_SET_ADDR      = 0x50C3;  // 32 bytes = 16 words (end:0x50D3)
constexpr uint16_t VP_WIFI_QR_ADDR            = 0x50D8;  // 80 bytes = 40 words (end:0x50FF)
  #endif

constexpr uint16_t VP_T_E0_Set                = 0x5100;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_E1_Set                = 0x5101;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_E2_Set                = 0x5102;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_Bed_Set               = 0x5106;  // 2 bytes  (3.0)
constexpr uint16_t VP_T_Chamber_Set           = 0x5108;  // 2 bytes  (3.0)
constexpr uint16_t VP_Feedrate_Percentage_Set = 0x510B;  // 2 bytes  (3.0)
constexpr uint16_t VP_Fan0_Percentage_Set     = 0x510C;  // 2 bytes  (3.0)
constexpr uint16_t VP_Flowrate_E_Set          = 0x510D;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_Fan_Set         = 0x510E;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_E0_Set        = 0x510F;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_E1_Set        = 0x511A;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_E2_Set        = 0x511B;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_Bed_Set       = 0x5110;  // 2 bytes  (3.0)
constexpr uint16_t VP_PreHeat_T_Chamber_Set   = 0x5119;  // 2 bytes  (3.0)

constexpr uint16_t VP_X_STEP_PER_MM_Set       = 0x5111;  // 4 bytes  (3.4)
constexpr uint16_t VP_Y_STEP_PER_MM_Set       = 0x5113;  // 4 bytes  (3.4)
constexpr uint16_t VP_Z_STEP_PER_MM_Set       = 0x5115;  // 4 bytes  (4.3)
constexpr uint16_t VP_E_STEP_PER_MM_Set       = 0x5117;  // 4 bytes  (3.4)

constexpr uint16_t VP_RegKey_Set              = 0x512B;  // 2 bytes  (3.2)

constexpr uint16_t VP_FanT_Percentage_Set     = 0x5171;  // 2 bytes  (3.0)
constexpr uint16_t VP_Fan1_Percentage_Set     = 0x5172;  // 2 bytes  (3.0)
constexpr uint16_t VP_ServoOffsetZ_Set        = 0x5179;  // 2 bytes  (2.2)
constexpr uint16_t VP_FadeHeight_Set          = 0x517A;  // 2 bytes  (3.1)

constexpr uint16_t VP_SD_FileName0            = 0x6000;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileName1            = 0x6100;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileName2            = 0x6200;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileName3            = 0x6300;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileName4            = 0x6400;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileName5            = 0x6500;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileName6            = 0x6600;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileName7            = 0x6700;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileName8            = 0x6800;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_Selected             = 0x6F00;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileICO_0            = 0x530F;  // 2 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_SD_FileICO_1            = 0x531F;  // 2 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_SD_FileICO_2            = 0x532F;  // 2 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_SD_FileICO_3            = 0x533F;  // 2 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_SD_FileICO_4            = 0x534F;  // 2 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_SD_FileICO_5            = 0x535F;  // 2 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_SD_FileICO_6            = 0x536F;  // 2 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_SD_FileICO_7            = 0x537F;  // 2 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_SD_FileICO_8            = 0x538F;  // 2 bytes (1.0)  (0,1,2)

  #if ENABLED(CreatBot_DGUS_V5)
constexpr uint16_t VP_Home_Icon               = 0x53EF;  // 1 bytes (1.0)  (0,1,2)
  #elif ENABLED(CreatBot_DGUS_V6)
constexpr uint16_t VP_Home_WIFI_Icon          = 0x5400;  // 1 bytes (1.0)  (0,1,2,3,4)
constexpr uint16_t VP_Home_CLOUD_Icon         = 0x5401;  // 1 bytes (1.0)  (0,1,2)
constexpr uint16_t VP_Home_CAMERA_Icon        = 0x5402;  // 1 bytes (1.0)  (0,1,2)
  #endif

constexpr uint16_t VP_BabyStepZ               = 0x5410;  // 2 bytes  (2.2)

constexpr uint16_t VP_SD_FileNameGap          = 0x0100;  // 512 bytes = 256 words
constexpr uint16_t VP_SD_FileICOGap           = 0x0010;  // 32 bytes = 16 words

constexpr uint16_t VP_PAGE                    = 0x0014;  // 当前页面变量
constexpr uint16_t VP_TOUCH                   = 0x0016;  // 当前触摸状态
constexpr uint16_t VP_CMD                     = 0x5700;  // 屏幕指令变量
constexpr uint16_t VP_POP                     = 0x5603;  // 弹窗索引变量

// 弹窗图标索引
constexpr uint16_t POP0_ICO_SETTING_SAVING      = 0x0101;
constexpr uint16_t POP0_ICO_SETTING_RESTORING   = 0x0102;
constexpr uint16_t POP0_ICO_WAITING             = 0x0103;
constexpr uint16_t POP0_ICO_PARKING             = 0x0104;
constexpr uint16_t POP0_ICO_RESUMING            = 0x0105;
constexpr uint16_t POP0_ICO_INVALID_LEVLEL_DATA = 0x0106;

constexpr uint16_t POP1_ICO_EXTRUDE_FILAMENT    = 0x0107;
constexpr uint16_t POP1_ICO_RECRACT_FILAMENT    = 0x0108;
constexpr uint16_t POP1_ICO_LOAD_FILAMENT       = 0x0109;
constexpr uint16_t POP1_ICO_UNLOAD_FILAMENT     = 0x010A;
constexpr uint16_t POP1_ICO_MOVING              = 0x010B;
constexpr uint16_t POP1_ICO_HOMING              = 0x010C;
constexpr uint16_t POP1_ICO_PROBING             = 0x010D;
constexpr uint16_t POP1_ICO_PROBER_UNAVAILABLE  = 0x010E;
constexpr uint16_t POP1_ICO_TAR_TEMP_LOW        = 0x010F;
constexpr uint16_t POP1_ICO_CUR_TEMP_LOW        = 0x0110;
constexpr uint16_t POP1_ICO_HEATER_TIMEOUT      = 0x0111;

constexpr uint16_t POP2_ICO_CHANGE_CONFIRM      = 0x0112;
constexpr uint16_t POP2_ICO_RESET_CONFIRM       = 0x0113;
constexpr uint16_t POP2_ICO_SKIP_CONFIRM        = 0x0114;
constexpr uint16_t POP2_ICO_STOP_CONFIRM        = 0x0115;
constexpr uint16_t POP2_ICO_INSERT_CONTIUNE     = 0x0116;
constexpr uint16_t POP2_ICO_LOAD_OPTION         = 0x0117;

#define FILAMENT_UNLOAD_SPEED           50  // (mm/s)
#define FILAMENT_UNLOAD_EXTRUDER_LENGTH 10  // (mm)
#ifdef NEAR_FEED
  #define FILAMENT_UNLOAD_LENGTH 20
#else
  #define FILAMENT_UNLOAD_LENGTH 150
#endif

// 变量相关宏定义
#define LCD_MSG_LENGTH     32                    // 32 char = 16 word
#define LCD_ROLLMSG_LENGTH (LCD_MSG_LENGTH - 6)  // 26 char = 13 word
#define LCD_SN_CHAR_LEN    16                    // 16 char = 8 word
#define LCD_QR_LEN         80                    // 80 char = 40 word
#if ENABLED(SDSUPPORT)
  #define LCD_FILENAME_LENGTH (FILENAME_LENGTH * VFAT_ENTRIES_LIMIT + 1) * 2
  #define LCD_FILE_PER_PAGE   7  // TODO CreatBot 根据不同的屏幕指定不同的单页目录项
#endif

struct VPMapping {
  const uint8_t   screen;
  const uint16_t *VPList;  // The list is null-terminated.
};

extern const struct VPMapping VPMap[];

// List of VPs handled by Marlin / The Display.
extern const struct DGUS_VP_Variable ListOfVP[];