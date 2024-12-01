/**
 * DGUSDisplayMacro.h
 *
 *  Created on: 2022/05/11
 *      Author: CreatBot LYN
 *
 *  For CreatBot, Some Marco
 */
#pragma once

#include "../../../../inc/MarlinConfigPre.h"

// 所有页面定义
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_1          41
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_2          42
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_3          43
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_1_CHAMBER  44
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_2_CHAMBER  45
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_1         46
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_2         47
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_3         48
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_1_CHAMBER 49
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_2_CHAMBER 50
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_1          51
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_2          52
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_3          53
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_1_CHAMBER  54
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_2_CHAMBER  55
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_1         56
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_2         57
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_3         58
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_1_CHAMBER 59
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_2_CHAMBER 60
#define PAGE_DEFAULT_PRINT_PAUSE_1                         61
#define PAGE_DEFAULT_PRINT_PAUSE_2                         62
#define PAGE_DEFAULT_PRINT_PAUSE_3                         63
#define PAGE_DEFAULT_PRINT_PAUSE_1_CHAMBER                 64
#define PAGE_DEFAULT_PRINT_PAUSE_2_CHAMBER                 65
#define PAGE_DEFAULT_PRINT_RESUME_1                        66
#define PAGE_DEFAULT_PRINT_RESUME_2                        67
#define PAGE_DEFAULT_PRINT_RESUME_3                        68
#define PAGE_DEFAULT_PRINT_RESUME_1_CHAMBER                69
#define PAGE_DEFAULT_PRINT_RESUME_2_CHAMBER                70
#define PAGE_DEFAULT_ONLINE_1                              71
#define PAGE_DEFAULT_ONLINE_2                              72
#define PAGE_DEFAULT_ONLINE_3                              73
#define PAGE_DEFAULT_ONLINE_1_CHAMBER                      74
#define PAGE_DEFAULT_ONLINE_2_CHAMBER                      75

#define PAGE_MOVE                                          81
#define PAGE_FILE_ONE                                      83
#define PAGE_FILE_NEXT                                     84
#define PAGE_FILE_LAST                                     85
#define PAGE_FILE_BOTH                                     86
#define PAGE_FILE_UP_ONE                                   87
#define PAGE_FILE_UP_NEXT                                  88
#define PAGE_FILE_UP_LAST                                  89
#define PAGE_FILE_UP_BOTH                                  90
#define PAGE_FILAMENT_1                                    92
#define PAGE_FILAMENT_2                                    93
#define PAGE_FILAMENT_3                                    94
#define PAGE_FILAMENT_1_CHAMBER                            92
#define PAGE_FILAMENT_2_CHAMBER                            93

#define PAGE_ADJUST                                        96
#define PAGE_SETTING                                       99
#define PAGE_SETTING_WIFI                                  100
#define PAGE_SETTING_LEVELING                              101
#define PAGE_SETTING_WIFI_LEVELING                         102
#define PAGE_MOTION_SETTING                                105
#define PAGE_FANSPEED_SETTING                              106
#define PAGE_FANSPEED_SETTING_FILTER                       107
#define PAGE_PREHEAT_SETTING                               108
#define PAGE_PREHEAT_SETTING_CHAMBER                       109
#define PAGE_WIFI_SETTING_OFF                              110
#define PAGE_WIFI_SETTING_ON                               111
#define PAGE_LEVELING_SETTING_OFF                          112
#define PAGE_LEVELING_SETTING_ON                           113
#define PAGE_LEVELING_SETTING_DISABLE                      114
#define PAGE_MORE_SETTING                                  115
#define PAGE_REG                                           116
#define PAGE_INFO_PRINTER                                  117
#define PAGE_WIFI_INFO_STATION                             119
#define PAGE_WIFI_INFO_AP                                  121
#define PAGE_PRINT_CONFIRM                                 123
#define PAGE_SHUTDOWN_HOTTEMP                              125
#define PAGE_UNFINISH_CHOOSE                               127

#define PAGE_CONNECT_WIFI_12                               129
#define PAGE_CONNECT_WIFI_01                               130

#define PAGE_POP_0                                         144
#define PAGE_POP_1                                         145
#define PAGE_POP_2                                         146

#define PAGE_NOZZLE_HEATING                                147

// 按键指令返回
#define CMD_RETURN_UP_LEVEL_BUTTON 0x0700  // unused
#define CMD_RETURN_DEFAULT_BUTTON  0x0701

#define CMD_PAUSE_BUTTON           0x0702
#define CMD_REUSE_BUTTON           0x0703
#define CMD_CANCEL_BUTTON          0x0704
#define CMD_PREHEAT_BUTTON         0x0705
#define CMD_COOLDOWN_BUTTON        0x0706
#define CMD_OPEN_FILE_BUTTON       0x0707
#define CMD_OPEN_FILAMENT_BUTTON   0x0708
#define CMD_CHANGE_BUTTON          0x0709
#define CMD_Z_UP_BUTTON            0x070A
#define CMD_Z_DOWN_BUTTON          0x070B
#define CMD_SETTING_BUTTON         0x070C
#define CMD_ADJUST_BUTTON          0x070D
#define CMD_REG_INFO_BUTTON        0x070E
#define CMD_REBOOT_DFU_BUTTON      0x070F

#define CMD_MOVE_HOME_ALL          0x0710
#define CMD_MOVE_HOME_X            0x0711
#define CMD_MOVE_HOME_Y            0x0712
#define CMD_MOVE_HOME_Z            0x0713
#define CMD_MOVE_X_PLUS            0x0714
#define CMD_MOVE_Y_PLUS            0x0715
#define CMD_MOVE_Z_PLUS            0x0716
#define CMD_MOVE_X_MINUS           0x0717
#define CMD_MOVE_Y_MINUS           0x0718
#define CMD_MOVE_Z_MINUS           0x0719

#define CMD_FILAMENT_EXT           0x0720
#define CMD_FILAMENT_REC           0x0721
#define CMD_FILAMENT_UNLOAD        0x0722
#define CMD_FILAMENT_LOAD          0x0723

#define CMD_BABYSTEP_UP_BUTTON     0x0724
#define CMD_BABYSTEP_DOWN_BUTTON   0x0725

#define CMD_STORE_SETTINGS         0x0730
#define CMD_RESET_SETTINGS         0x0731
#define CMD_JUMP_SETTING_MORE      0x0733
#define CMD_JUMP_SETTING_MOTION    0x0734
#define CMD_JUMP_SETTING_FAN       0x0735
#define CMD_JUMP_SETTING_PREHEAT   0x0736
#define CMD_JUMP_SETTING_WIFI      0x0737
#define CMD_JUMP_SETTING_LEVELING  0x0738

#define CMD_FILE_NEXT_PAGE         0x0740
#define CMD_FILE_LAST_PAGE         0x0741
#define CMD_FILE_UP_LEVEL          0x0742

#define CMD_WIFI_SWITCH_TOGGLE     0x075F
#define CMD_WIFI_SWITCH_MODE       0x0750  // unused
#define CMD_WIFI_RESET             0x0751
#define CMD_WIFI_SCAN              0x0752
#define CMD_WIFI_NEXT_PAGE         0x0753
#define CMD_WIFI_LAST_PAGE         0x0754
#define CMD_WIFI_MODE_STA          0x075C
#define CMD_WIFI_MODE_AP           0x075D
#define CMD_WIFI_SETTING_ENTRY     0x075E

#define CMD_LEVELING_SWITCH_TOGGLE 0x076F
#define CMD_LEVELING_PROBE         0x0760

#define CMD_CASE_LIGHT             0x0770
#define CMD_AUTO_POWER_OFF         0x0771
#define CMD_AUTO_BACK_MAIN         0x0772

#define CMD_FILE_ITEM_0_ACTION     0x07A0
#define CMD_FILE_ITEM_1_ACTION     0x07A1
#define CMD_FILE_ITEM_2_ACTION     0x07A2
#define CMD_FILE_ITEM_3_ACTION     0x07A3
#define CMD_FILE_ITEM_4_ACTION     0x07A4
#define CMD_FILE_ITEM_5_ACTION     0x07A5
#define CMD_FILE_ITEM_6_ACTION     0x07A6
#define CMD_FILE_ITEM_7_ACTION     0x07A7
#define CMD_FILE_ITEM_8_ACTION     0x07A8

#define CMD_WIFI_ITEM_0_ACTION     0x07A9
#define CMD_WIFI_ITEM_1_ACTION     0x07AA
#define CMD_WIFI_ITEM_2_ACTION     0x07AB
#define CMD_WIFI_ITEM_3_ACTION     0x07AC
#define CMD_WIFI_ITEM_4_ACTION     0x07AD

#define CMD_FILE_ACTION_OPEN       0x00B0  // unused
#define CMD_FILE_ACTION_PRINT      0x00B1
#define CMD_FILE_ACTION_CANCEL     0x00B2
#define CMD_SHUTTING_CANCEL        0x00B3
#define CMD_ACCIDENT_REUSE         0x00B4
#define CMD_ACCIDENT_CANCEL        0x00B5
#define CMD_CHANGE_DONE            0x00B6  // unused
#define CMD_ONLINE_RETURN          0x00B7  // unused
#define CMD_WIFI_CONNECT           0x00B8
#define CMD_WIFI_CANCEL            0x00B9
#define CMD_HEATING_SKIP           0x00BA

#define CMD_POP1_BUTTON            0x00BF
#define CMD_POP2_BUTTON1           0x00BE
#define CMD_POP2_BUTTON2           0x00BD

// 开关图标索引
constexpr uint16_t ICO_ON          = 0x0001;
constexpr uint16_t ICO_OFF         = 0x0002;
constexpr uint16_t ICO_ON_DISABLE  = 0x0003;
constexpr uint16_t ICO_OFF_DISABLE = 0x0004;

// 移轴单位
constexpr uint16_t MOVE_UNIT_01MM  = 0x0001;
constexpr uint16_t MOVE_UNIT_1MM   = 0x0002;
constexpr uint16_t MOVE_UNIT_10MM  = 0x0003;
constexpr uint16_t MOVE_UNIT_100MM = 0x0004;

// 进度条填充坐标与颜色
  #if ENABLED(CreatBot_DGUS_V5)
#define PROGRESS_BAR_X0 0
#define PROGRESS_BAR_Y0 134
#define PROGRESS_BAR_X1 480
#define PROGRESS_BAR_Y1 174
#define COLOR_PRIMARY   0xEB65  // YELLOW
#define COLOR_ASSIST    0x632C  // GRAY
  #elif ENABLED(CreatBot_DGUS_V6)
    #if ENABLED(DGUS_1024768)
#define PROGRESS_BAR_X0   20    // 左上角 X
#define PROGRESS_BAR_Y0   438   // 左上角 Y
#define PROGRESS_BAR_X1   1004  // 右下角 X
#define PROGRESS_BAR_Y1   528   // 右下角 Y
#define PROGRESS_BAR_X2   704   // 渐变坐标 X
#define PROGRESS_BAR_R    10    // 进度条圆角半径
#define PROGRESS_BAR_PAGE 80    // 进度条样式页面
    #else
#define PROGRESS_BAR_X0   29    // 左上角 X
#define PROGRESS_BAR_Y0   383   // 左上角 Y
#define PROGRESS_BAR_X1   1013  // 右下角 X
#define PROGRESS_BAR_Y1   443   // 右下角 Y
#define PROGRESS_BAR_X2   713   // 渐变坐标 X
#define PROGRESS_BAR_R    10    // 进度条圆角半径
#define PROGRESS_BAR_PAGE 80    // 进度条样式页面
    #endif
  #endif

// 图片剪切的数据块长度
#define CUT_DATA_LENGTH 14

// WIFI密码显示文本长度
#define WIFI_TEXT_LEN 16

// 喷头索引
#if HOTENDS > 2
  #define Active_Extruder_E0 0x0004
  #define Active_Extruder_E1 0x0005
  #define Active_Extruder_E2 0x0006
#elif HAS_MULTI_HOTEND
  #define Active_Extruder_E0 0x0002
  #define Active_Extruder_E1 0x0003
#endif

// 宏选择
#if HOTENDS > 2
  #if HAS_TEMP_CHAMBER
    #error "DWIN_V5 don't support 3 EXTRUDER with CHAMBER"
  #else
    #define PAGE_PARSE_DEFAULT(x) (x##_3)
  #endif
#elif HAS_MULTI_HOTEND
  #if HAS_TEMP_CHAMBER
    #define PAGE_PARSE_DEFAULT(x) (x##_2_CHAMBER)
  #else
    #define PAGE_PARSE_DEFAULT(x) (x##_2)
  #endif
#elif HAS_HOTEND
  #if HAS_TEMP_CHAMBER
    #define PAGE_PARSE_DEFAULT(x) (x##_1_CHAMBER)
  #else
    #define PAGE_PARSE_DEFAULT(x) (x##_1)
  #endif
#endif

#if BOTH(CREATBOT_WIFI_SUPPORT, HAS_LEVELING)
  #define PAGE_PARSE_SETTING PAGE_SETTING_WIFI_LEVELING
#elif ENABLED(CREATBOT_WIFI_SUPPORT)
  #define PAGE_PARSE_SETTING PAGE_SETTING_WIFI
#elif HAS_LEVELING
  #define PAGE_PARSE_SETTING PAGE_SETTING_LEVELING
#else
  #define PAGE_PARSE_SETTING PAGE_SETTING
#endif

#if ENABLED(FILTER_FAN_SUPPORT)
  #define PAGE_PARSE_FANSPEED_SETTING PAGE_FANSPEED_SETTING_FILTER
#else
  #define PAGE_PARSE_FANSPEED_SETTING PAGE_FANSPEED_SETTING
#endif

#if HAS_TEMP_CHAMBER
  #define PAGE_PARSE_PREHEAT_SETTING PAGE_PREHEAT_SETTING_CHAMBER
#else
  #define PAGE_PARSE_PREHEAT_SETTING PAGE_PREHEAT_SETTING
#endif
