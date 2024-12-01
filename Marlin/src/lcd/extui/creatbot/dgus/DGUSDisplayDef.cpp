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

/**
 * lcd/extui/creatbot/dgus/DGUSDisplayDef.cpp
 */

#include "../../../../inc/MarlinConfigPre.h"

#if ENABLED(DGUS_LCD_UI_CREATBOT)
  #include "../../../../inc/MarlinConfig.h"
  #include "DGUSDisplay.h"
  #include "DGUSDisplayDef.h"
  #include "DGUSScreenHandler.h"
  #include "DGUSVPVariable.h"

  #define VP_HELPER_CMD(VAL, RXHANDLER) VP_HELPER_MATCH(VP_CMD, VAL, RXHANDLER)

const uint16_t VPList_Boot[] PROGMEM = {
    // VP_MARLIN_VERSION,  //发送版本号
    0x0000,
};

const uint16_t VPList_Default_Idle[] PROGMEM = {
    VP_MSG_M117,  // 必要时才更新信息字符串
  #if HAS_HOTEND
    VP_T_E0_Cur,      VP_T_E0_Tar,  // 喷头一温度
  #endif
  #if HAS_MULTI_HOTEND
    VP_T_E1_Cur,      VP_T_E1_Tar,  // 喷头二温度
  #endif
  #if HOTENDS > 2
    VP_T_E2_Cur,      VP_T_E2_Tar,  // 喷头三温度
  #endif
  #if HAS_TEMP_BED
    VP_T_Bed_Cur,     VP_T_Bed_Tar,  // 热床温度
  #endif
  #if HAS_TEMP_CHAMBER
    VP_T_Chamber_Cur, VP_T_Chamber_Tar,  // 热风温度
  #endif
    0x0000,
};

const uint16_t VPList_Default_Print[] PROGMEM = {
// VP_MSG_M117_SHORT/ROLL, //必要时才更新信息字符串
  #if HAS_HOTEND
    VP_T_E0_Cur,      VP_T_E0_Tar,  // 喷头一温度
  #endif
  #if HAS_MULTI_HOTEND
    VP_T_E1_Cur,      VP_T_E1_Tar,  // 喷头二温度
  #endif
  #if HOTENDS > 2
    VP_T_E2_Cur,      VP_T_E2_Tar,  // 喷头三温度
  #endif
  #if HAS_TEMP_BED
    VP_T_Bed_Cur,     VP_T_Bed_Tar,  // 热床温度
  #endif
  #if HAS_TEMP_CHAMBER
    VP_T_Chamber_Cur, VP_T_Chamber_Tar,  // 热风温度
  #endif
    VP_PrintPercent,  // 打印百分比
    VP_PrintTime,     // 打印时间
  #if ENABLED(CreatBot_DGUS_V5)
    VP_PrintProgessCanvas,  // 打印进度条背景
    VP_PrintProgessBrush,   // 打印进度条
  #elif ENABLED(CreatBot_DGUS_V6)
    VP_PrintProgess,  // 打印进度条
  #endif    
    VP_Z_STR_ADDR,    VP_ZPos,
    VP_BabyStepZ,
    0x0000,
};

const uint16_t VPList_Default_Online[] PROGMEM = {
// VP_MSG_M117_SHORT/ROLL, //必要时才更新信息字符串
  #if HAS_HOTEND
    VP_T_E0_Cur,      VP_T_E0_Tar,  // 喷头一温度
  #endif
  #if HAS_MULTI_HOTEND
    VP_T_E1_Cur,      VP_T_E1_Tar,  // 喷头二温度
  #endif
  #if HOTENDS > 2
    VP_T_E2_Cur,      VP_T_E2_Tar,  // 喷头三温度
  #endif
  #if HAS_TEMP_BED
    VP_T_Bed_Cur,     VP_T_Bed_Tar,  // 热床温度
  #endif
  #if HAS_TEMP_CHAMBER
    VP_T_Chamber_Cur, VP_T_Chamber_Tar,  // 热风温度
  #endif
    VP_XPos,          VP_YPos,          VP_ZPos,        // X Y Z轴位置
    VP_X_STR_ADDR,    VP_Y_STR_ADDR,    VP_Z_STR_ADDR,  //
    0x0000,
};

const uint16_t VPList_ManualMove[] PROGMEM = {
    VP_XPos,      VP_YPos, VP_ZPos,  // 坐标位置
    VP_Move_Unit,                    // 移轴单位
    0x0000,
};

const uint16_t VPList_ManualExtrude[] PROGMEM = {
    VP_Filament_Length,     // 指定耗材长度
    VP_Activated_Extruder,  // 当前激活喷头
    0x0000,
};

const uint16_t VPList_SDFileList[] PROGMEM = {
    VP_SD_FileName0, VP_SD_FileName1, VP_SD_FileName2, VP_SD_FileName3, VP_SD_FileName4,
    VP_SD_FileName5, VP_SD_FileName6, VP_SD_FileName7, VP_SD_FileName8, VP_SD_FileICO_0,
    VP_SD_FileICO_1, VP_SD_FileICO_2, VP_SD_FileICO_3, VP_SD_FileICO_4, VP_SD_FileICO_5,
    VP_SD_FileICO_6, VP_SD_FileICO_7, VP_SD_FileICO_8, 0x0000,
};

const uint16_t VPList_PrintAdjust[] PROGMEM = {
    VP_Feedrate_Percentage,  // 全局打印速度
    VP_Fan0_Percentage,      // 冷却风扇速度
    VP_Flowrate_E,           // 耗材挤出倍率
    0x0000,
};

const uint16_t VPList_MotionSetting[] PROGMEM = {
    VP_X_STEP_PER_MM, VP_Y_STEP_PER_MM, VP_Z_STEP_PER_MM, VP_E_STEP_PER_MM, 0x0000,
};

const uint16_t VPList_FanSpeedSetting[] PROGMEM = {
    VP_FanT_Percentage,  // Sink_Fan 机头散热风扇
    VP_Fan0_Percentage,  // Cooling_Fan 模型冷却风扇
    VP_Fan1_Percentage,  // Air_Filter_Fan 空气过滤风扇
    0x0000,
};

const uint16_t VPList_PreheatSetting[] PROGMEM = {
  #if HAS_HOTEND
    VP_PreHeat_T_E0,  // 喷头预热温度
  #endif
  #if HAS_TEMP_BED
    VP_PreHeat_T_Bed,  // 热床预热温度
  #endif
  #if HAS_TEMP_CHAMBER
    VP_PreHeat_T_Chamber,  // 热风预热温度
  #endif
    0x0000,
};

const uint16_t VPList_Printer_Info[] PROGMEM = {
    VP_UsedTime,
    VP_FW_Ver,
    VP_UI_Ver,
    0x0000,
};

const uint16_t VPList_Print_Confirm[] PROGMEM = {
    VP_SD_Selected,
    0x0000,
};

const uint16_t VPList_Leveling_Info[] PROGMEM = {
    VP_ServoOffsetZ,
    VP_FadeHeight,
    0x0000,
};

const uint16_t VPList_Heating[] PROGMEM = {
    VP_T_ACT_Cur,
    VP_T_ACT_Tar,
    0x0000,
};

const uint16_t VPList_Shutdown_Cooling[] PROGMEM = {
    VP_T_MAX_Cur,
    0x0000,
};

const uint16_t VPList_MoreSetting_Info[] PROGMEM = {
    VP_CaseLight,     // 机箱灯光开关
    VP_AutoPowerOff,  // 自动关机开关
    VP_AutoBackMain,  // 自动返回主页面开关
    0x0000,
};

const uint16_t VPList_REG_Info[] PROGMEM = {
    VP_TrialPeriod,  // 试用期
    VP_SN_STR,       // 序列号
    VP_RegKey,       // 注册码
    VP_RegState,     // 状态图标
    0x0000,
};

const struct VPMapping VPMap[] PROGMEM = {
    // 起始页
    {DGUSLCD_SCREEN_BOOT, VPList_Boot},

    // 默认页
    {DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT, VPList_Default_Idle},
    {DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE, VPList_Default_Idle},
    {DGUSLCD_SCREEN_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT, VPList_Default_Idle},
    {DGUSLCD_SCREEN_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE, VPList_Default_Idle},
    {DGUSLCD_SCREEN_DEFAULT_PRINT_PAUSE, VPList_Default_Print},
    {DGUSLCD_SCREEN_DEFAULT_PRINT_RESUME, VPList_Default_Print},
    {DGUSLCD_SCREEN_DEFAULT_ONLINE, VPList_Default_Online},

    // 移轴和送丝页面
    {DGUSLCD_SCREEN_MOVE, VPList_ManualMove},
    {DGUSLCD_SCREEN_FILAMENT, VPList_ManualExtrude},

    // 文件目录页面
    {DGUSLCD_SCREEN_FILE_ONE, VPList_SDFileList},
    {DGUSLCD_SCREEN_FILE_NEXT, VPList_SDFileList},
    {DGUSLCD_SCREEN_FILE_LAST, VPList_SDFileList},
    {DGUSLCD_SCREEN_FILE_BOTH, VPList_SDFileList},
    {DGUSLCD_SCREEN_FILE_UP_ONE, VPList_SDFileList},
    {DGUSLCD_SCREEN_FILE_UP_NEXT, VPList_SDFileList},
    {DGUSLCD_SCREEN_FILE_UP_LAST, VPList_SDFileList},
    {DGUSLCD_SCREEN_FILE_UP_BOTH, VPList_SDFileList},

    // 打印调整页面
    {DGUSLCD_SCREEN_ADJUST, VPList_PrintAdjust},

    // 运动参数页面
    {DGUSLCD_SCREEN_MOTION_SETTING, VPList_MotionSetting},

    // 风扇参数页面
    {DGUSLCD_SCREEN_FANSPEED_SETTING, VPList_FanSpeedSetting},

    // 预热参数页面
    {DGUSLCD_SCREEN_PREHEAT_SETTING, VPList_PreheatSetting},

    // 更多设置页面
    {DGUSLCD_SCREEN_MORESETTING, VPList_MoreSetting_Info},

    // 信息页面
    {DGUSLCD_SCREEN_PRINTER_INFO, VPList_Printer_Info},

    // 打印确认页面
    {DGUSLCD_SCREEN_PRINT_CONFIRM, VPList_Print_Confirm},

    // 调平页面
    {DGUSLCD_SCREEN_LEVELING_SETTING_OFF, VPList_Leveling_Info},
    {DGUSLCD_SCREEN_LEVELING_SETTING_ON, VPList_Leveling_Info},
    {DGUSLCD_SCREEN_LEVELING_SETTING_DISABLE, VPList_Leveling_Info},

    // 加热页面
    {DGUSLCD_SCREEN_NOZZLE_HEATING, VPList_Heating},

    // 冷却页面
    {DGUSLCD_SCREEN_SHUTDOWN_HOTTEMP, VPList_Shutdown_Cooling},

    // 注册码页面
    {DGUSLCD_SCREEN_REG, VPList_REG_Info},
    // TODO CreatBot 其他页面

    {0, nullptr}  // List is terminated with an nullptr as table entry.
};

const struct DGUS_VP_Variable ListOfVP[] PROGMEM = {
    VP_HELPER_RX(VP_TOUCH, ScreenHandler.TouchHook),  // 触摸处理
    VP_HELPER_RX(VP_PAGE, ScreenHandler.PageHook),    // 页面处理

    VP_HELPER_CMD(CMD_RETURN_DEFAULT_BUTTON, ScreenHandler.HandlePageDefault),    // 返回默认页
    VP_HELPER_CMD(CMD_PAUSE_BUTTON, ScreenHandler.HandlePrintStatuSet),           // 暂停打印按钮
    VP_HELPER_CMD(CMD_REUSE_BUTTON, ScreenHandler.HandlePrintStatuSet),           // 恢复打印按钮
    VP_HELPER_CMD(CMD_CANCEL_BUTTON, ScreenHandler.HandlePrintStatuSet),          // 取消打印按钮
    VP_HELPER_CMD(CMD_PREHEAT_BUTTON, ScreenHandler.HandlePreheat),               // 预热按钮
    VP_HELPER_CMD(CMD_COOLDOWN_BUTTON, ScreenHandler.HandleCoolDown),             // 冷却按钮
    VP_HELPER_CMD(CMD_OPEN_FILAMENT_BUTTON, ScreenHandler.HandleFilament),        // 耗材按钮
    VP_HELPER_CMD(CMD_CHANGE_BUTTON, ScreenHandler.HandleFilamentChange),         // 更换耗材按钮
    VP_HELPER_CMD(CMD_Z_UP_BUTTON, ScreenHandler.HandleZAxisUp),                  // 平台上升按钮
    VP_HELPER_CMD(CMD_Z_DOWN_BUTTON, ScreenHandler.HandleZAxisDown),              // 平台下降按钮
    VP_HELPER_CMD(CMD_SETTING_BUTTON, ScreenHandler.HandleSetting),               // 设置菜单按钮
    VP_HELPER_CMD(CMD_ADJUST_BUTTON, ScreenHandler.HandleAdjust),                 // 调整菜单按钮
    VP_HELPER_CMD(CMD_REBOOT_DFU_BUTTON, ScreenHandler.HandleRebootIntoDFUMode),  // 下载模式按钮
    VP_HELPER_CMD(CMD_MOVE_HOME_ALL, ScreenHandler.HandleHome),                   // 全部回零
    VP_HELPER_CMD(CMD_MOVE_HOME_X, ScreenHandler.HandleHome),                     // X 回零
    VP_HELPER_CMD(CMD_MOVE_HOME_Y, ScreenHandler.HandleHome),                     // Y 回零
    VP_HELPER_CMD(CMD_MOVE_HOME_Z, ScreenHandler.HandleHome),                     // Z 回零
    VP_HELPER_CMD(CMD_MOVE_X_PLUS, ScreenHandler.HandleManualMove),               // X +
    VP_HELPER_CMD(CMD_MOVE_Y_PLUS, ScreenHandler.HandleManualMove),               // Y +
    VP_HELPER_CMD(CMD_MOVE_Z_PLUS, ScreenHandler.HandleManualMove),               // Z +
    VP_HELPER_CMD(CMD_MOVE_X_MINUS, ScreenHandler.HandleManualMove),              // X -
    VP_HELPER_CMD(CMD_MOVE_Y_MINUS, ScreenHandler.HandleManualMove),              // Y -
    VP_HELPER_CMD(CMD_MOVE_Z_MINUS, ScreenHandler.HandleManualMove),              // Z -
    VP_HELPER_CMD(CMD_FILAMENT_EXT, ScreenHandler.HandleManualExtrude),           // 进耗材按钮
    VP_HELPER_CMD(CMD_FILAMENT_REC, ScreenHandler.HandleManualExtrude),           // 退耗材按钮
    VP_HELPER_CMD(CMD_FILAMENT_LOAD, ScreenHandler.HandleLoadFilament),           // 加载耗材按钮
    VP_HELPER_CMD(CMD_FILAMENT_UNLOAD, ScreenHandler.HandleLoadFilament),         // 卸载耗材按钮
    VP_HELPER_CMD(CMD_BABYSTEP_UP_BUTTON, ScreenHandler.HandleBabyStepUp),        // 打印时BabyStep上调
    VP_HELPER_CMD(CMD_BABYSTEP_DOWN_BUTTON, ScreenHandler.HandleBabyStepDown),    // 打印时BabyStep下调
    VP_HELPER_CMD(CMD_STORE_SETTINGS, ScreenHandler.HandleStore),                 // 保存设置
    VP_HELPER_CMD(CMD_RESET_SETTINGS, ScreenHandler.HandleReset),                 // 恢复出厂
    VP_HELPER_CMD(CMD_JUMP_SETTING_MOTION, ScreenHandler.HandleSettingMotion),    // 步进设置
    VP_HELPER_CMD(CMD_JUMP_SETTING_FAN, ScreenHandler.HandleSettingFan),          // 风扇设置
    VP_HELPER_CMD(CMD_JUMP_SETTING_PREHEAT, ScreenHandler.HandleSettingPreheat),  // 预热设置
    VP_HELPER_CMD(CMD_JUMP_SETTING_MORE, ScreenHandler.HandleSettingMore),        // 更多设置

    VP_HELPER_CMD(CMD_SHUTTING_CANCEL, ScreenHandler.HandleCancelShutdown),  // 取消关机
    VP_HELPER_CMD(CMD_HEATING_SKIP, ScreenHandler.HandleHeatSkip),           // 跳过加热等待
    VP_HELPER_CMD(CMD_POP1_BUTTON, ScreenHandler.HandlePopButton),           // 弹窗页面1按钮
    VP_HELPER_CMD(CMD_POP2_BUTTON1, ScreenHandler.HandlePopButton),          // 弹窗页面2按钮1
    VP_HELPER_CMD(CMD_POP2_BUTTON2, ScreenHandler.HandlePopButton),          // 弹窗页面2按钮2

  #if ENABLED(SDSUPPORT)
    VP_HELPER_CMD(CMD_OPEN_FILE_BUTTON, ScreenHandler.HandleFileOpen),      // U盘按钮
    VP_HELPER_CMD(CMD_FILE_ITEM_0_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_ITEM_1_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_ITEM_2_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_ITEM_3_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_ITEM_4_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_ITEM_5_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_ITEM_6_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_ITEM_7_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_ITEM_8_ACTION, ScreenHandler.HandleFileAction),  // U盘列表项
    VP_HELPER_CMD(CMD_FILE_NEXT_PAGE, ScreenHandler.HandleFileNext),        // U盘下一页
    VP_HELPER_CMD(CMD_FILE_LAST_PAGE, ScreenHandler.HandleFileLast),        // U盘上一页
    VP_HELPER_CMD(CMD_FILE_UP_LEVEL, ScreenHandler.HandleFileUp),           // U盘上一级
    VP_HELPER_CMD(CMD_FILE_UP_LEVEL, ScreenHandler.HandleFileUp),           // U盘上一级
    VP_HELPER_CMD(CMD_FILE_UP_LEVEL, ScreenHandler.HandleFileUp),           // U盘上一级
    VP_HELPER_CMD(CMD_FILE_ACTION_PRINT, ScreenHandler.HandleFilePrint),    // U盘文件打印
    VP_HELPER_CMD(CMD_FILE_ACTION_CANCEL, ScreenHandler.HandleFileCancel),  // U盘文件取消
  #endif

  // 自动调平
  #if HAS_LEVELING
    VP_HELPER_CMD(CMD_JUMP_SETTING_LEVELING, ScreenHandler.HandleSettingLeveling),  // 调平设置
    VP_HELPER_CMD(CMD_LEVELING_SWITCH_TOGGLE, ScreenHandler.HandleLevelingSwitch),  // 调平开关
    VP_HELPER_CMD(CMD_LEVELING_PROBE, ScreenHandler.HandleLevelingCalibrate),       // 调平启动
  #endif

    // 更多设置
    VP_HELPER_CMD(CMD_CASE_LIGHT, ScreenHandler.HandleSwitchAction),      // 机箱灯开关按钮
    VP_HELPER_CMD(CMD_AUTO_POWER_OFF, ScreenHandler.HandleSwitchAction),  // 自动关机开关按钮
    VP_HELPER_CMD(CMD_AUTO_BACK_MAIN, ScreenHandler.HandleSwitchAction),  // 自动返回主页开关按钮

  // 断电续打
  #if ENABLED(POWER_LOSS_RECOVERY)
    VP_HELPER_CMD(CMD_ACCIDENT_REUSE, ScreenHandler.HandleAccidentReuse),    // 继续断电续打
    VP_HELPER_CMD(CMD_ACCIDENT_CANCEL, ScreenHandler.HandleAccidentCancel),  // 取消断电续打
  #endif

  // TODO CreatBot 换成合适的宏定义
  #if ENABLED(CREATBOT_WIFI_SUPPORT)
    VP_HELPER_CMD(CMD_JUMP_SETTING_WIFI, ScreenHandler.HandleSettingWifi),  // WIFI设置
    VP_HELPER_CMD(CMD_WIFI_CANCEL, ScreenHandler.HandleSetting),            // 返回设置页面
    VP_HELPER_CMD(CMD_WIFI_CONNECT, ScreenHandler.HandleConnectWiFi),       // 连接WIFI
    VP_HELPER_RX(VP_WIFI_SSID_SET_ADDR, ScreenHandler.HandleWiFiInfoSet),   // 接收WIFI SSID
    VP_HELPER_RX(VP_WIFI_KEY_ADDR, ScreenHandler.HandleWiFiInfoSet),        // 接收WIFI KEY

  // VP_HELPER_CMD(CMD_WIFI_ITEM_0_ACTION, nullptr),                         // WIFI列表项
  // VP_HELPER_CMD(CMD_WIFI_ITEM_1_ACTION, nullptr),                         // WIFI列表项
  // VP_HELPER_CMD(CMD_WIFI_ITEM_2_ACTION, nullptr),                         // WIFI列表项
  // VP_HELPER_CMD(CMD_WIFI_ITEM_3_ACTION, nullptr),                         // WIFI列表项
  // VP_HELPER_CMD(CMD_WIFI_ITEM_4_ACTION, nullptr),                         // WIFI列表项
  // VP_HELPER_CMD(CMD_WIFI_SWITCH_TOGGLE, nullptr),                         // WIFI开关
  // VP_HELPER_CMD(CMD_WIFI_RESET, nullptr),                                 // WIFI重置
  // VP_HELPER_CMD(CMD_WIFI_SCAN, nullptr),                                  // WIFI扫描
  // VP_HELPER_CMD(CMD_WIFI_NEXT_PAGE, nullptr),                             // WIFI下一页
  // VP_HELPER_CMD(CMD_WIFI_LAST_PAGE, nullptr),                             // WIFI上一页
  // VP_HELPER_CMD(CMD_WIFI_MODE_STA, nullptr),                              // WIFI站点模式
  // VP_HELPER_CMD(CMD_WIFI_MODE_AP, nullptr),                               // WIFI基站模式
  // VP_HELPER_CMD(CMD_WIFI_SETTING_ENTRY, nullptr),                         // WIFI设置子页
  // VP_HELPER_KEY(KEY_WIFI_CONNECT, nullptr),                               // WIFI连接确认
  // VP_HELPER_KEY(KEY_WIFI_CANCEL, nullptr),                                // WIFI连接取消
  #endif

    // 状态字符串
    VP_HELPER_STR(VP_MSG_M117, LCD_MSG_LENGTH, ScreenHandler.UpdateMsg),
  #if ENABLED(CreatBot_DGUS_V5)
    VP_HELPER_STR(VP_MSG_M117_SHORT, LCD_MSG_LENGTH, ScreenHandler.UpdateMsg),
  #elif ENABLED(CreatBot_DGUS_V6)
    VP_HELPER_STR(VP_MSG_M117_ROLL, LCD_MSG_LENGTH, ScreenHandler.UpdateMsg),
  #endif    

  #if HAS_HOTEND
    // 喷头一
    VP_HELPER_TX(VP_T_E0_Cur, ScreenHandler.UpdateTemperatureCur),
    VP_HELPER_TX(VP_T_E0_Tar, ScreenHandler.UpdateTemperatureTar),
    VP_HELPER_RX(VP_T_E0_Set, ScreenHandler.HandleTemperatureSet),
  #endif

  #if HAS_MULTI_HOTEND
    // 喷头二
    VP_HELPER_TX(VP_T_E1_Cur, ScreenHandler.UpdateTemperatureCur),
    VP_HELPER_TX(VP_T_E1_Tar, ScreenHandler.UpdateTemperatureTar),
    VP_HELPER_RX(VP_T_E1_Set, ScreenHandler.HandleTemperatureSet),
  #endif

  #if HOTENDS > 2
    // 喷头三
    VP_HELPER_TX(VP_T_E2_Cur, ScreenHandler.UpdateTemperatureCur),
    VP_HELPER_TX(VP_T_E2_Tar, ScreenHandler.UpdateTemperatureTar),
    VP_HELPER_RX(VP_T_E2_Set, ScreenHandler.HandleTemperatureSet),
  #endif

  #if HAS_HEATED_BED
    // 热床
    VP_HELPER_TX(VP_T_Bed_Cur, ScreenHandler.UpdateTemperatureCur),
    VP_HELPER_TX(VP_T_Bed_Tar, ScreenHandler.UpdateTemperatureTar),
    VP_HELPER_RX(VP_T_Bed_Set, ScreenHandler.HandleTemperatureSet),
  #endif

  #if HAS_HEATED_CHAMBER
    // 热风
    VP_HELPER_TX(VP_T_Chamber_Cur, ScreenHandler.UpdateTemperatureCur),
    VP_HELPER_TX(VP_T_Chamber_Tar, ScreenHandler.UpdateTemperatureTar),
    VP_HELPER_RX(VP_T_Chamber_Set, ScreenHandler.HandleTemperatureSet),
  #endif

    // 全局速度
    VP_HELPER_TX(VP_Feedrate_Percentage, ScreenHandler.UpdateFeedrate),
    VP_HELPER_RX(VP_Feedrate_Percentage_Set, ScreenHandler.HandleFeedrateSet),

    // 挤出倍率
    VP_HELPER_TX(VP_Flowrate_E, ScreenHandler.UpdateFlowrate),
    VP_HELPER_RX(VP_Flowrate_E_Set, ScreenHandler.HandleFlowrateSet),

    // 轴坐标
    VP_HELPER_TX_DWORD(VP_XPos, ScreenHandler.UpdateAxisPos),  //
    VP_HELPER_TX_DWORD(VP_YPos, ScreenHandler.UpdateAxisPos),  //
    VP_HELPER_TX_DWORD(VP_ZPos, ScreenHandler.UpdateAxisPos),  //
    VP_HELPER_RXTX(VP_Move_Unit, ScreenHandler.HandleAxisUnitSet, ScreenHandler.UpdateAxisUnit),

  #if ENABLED(SDSUPPORT)
    // U盘文件列表
    VP_HELPER_STR(VP_SD_FileName0, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_STR(VP_SD_FileName1, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_STR(VP_SD_FileName2, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_STR(VP_SD_FileName3, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_STR(VP_SD_FileName4, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_STR(VP_SD_FileName5, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_STR(VP_SD_FileName6, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_STR(VP_SD_FileName7, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_STR(VP_SD_FileName8, LCD_FILENAME_LENGTH, ScreenHandler.UpdateFileName),
    VP_HELPER_TX(VP_SD_FileICO_0, ScreenHandler.UpdateFileICO),
    VP_HELPER_TX(VP_SD_FileICO_1, ScreenHandler.UpdateFileICO),
    VP_HELPER_TX(VP_SD_FileICO_2, ScreenHandler.UpdateFileICO),
    VP_HELPER_TX(VP_SD_FileICO_3, ScreenHandler.UpdateFileICO),
    VP_HELPER_TX(VP_SD_FileICO_4, ScreenHandler.UpdateFileICO),
    VP_HELPER_TX(VP_SD_FileICO_5, ScreenHandler.UpdateFileICO),
    VP_HELPER_TX(VP_SD_FileICO_6, ScreenHandler.UpdateFileICO),
    VP_HELPER_TX(VP_SD_FileICO_7, ScreenHandler.UpdateFileICO),
    VP_HELPER_TX(VP_SD_FileICO_8, ScreenHandler.UpdateFileICO),
    VP_HELPER_STR(VP_SD_Selected, LCD_FILENAME_LENGTH, ScreenHandler.UpdatePrintFileName),
  #endif

// 耗材页面
  #if HAS_MULTI_HOTEND
    VP_HELPER_RXTX(VP_Activated_Extruder,  //
                   ScreenHandler.HandleActivatedExtruderSet, ScreenHandler.UpdateActivatedExtruder),
  #endif
    VP_HELPER_RXTX(VP_Filament_Length,  //
                   ScreenHandler.HandleFilamentLengthSet, ScreenHandler.UpdateFilamentLength),

    // 打印页面
    VP_HELPER_TX(VP_PrintPercent, ScreenHandler.UpdatePrintPercentInfo),
    VP_HELPER_TX(VP_PrintProgess, ScreenHandler.UpdatePrintPercentInfo),
    VP_HELPER_TX(VP_PrintProgessCanvas, ScreenHandler.UpdatePrintPercentInfo),
    VP_HELPER_TX(VP_PrintProgessBrush, ScreenHandler.UpdatePrintPercentInfo),
    VP_HELPER_TX(VP_PrintTime, ScreenHandler.UpdateTimeInfo),
    VP_HELPER_TX(VP_Z_STR_ADDR, ScreenHandler.UpdateAxisString),
    VP_HELPER_TX(VP_Y_STR_ADDR, ScreenHandler.UpdateAxisString),
    VP_HELPER_TX(VP_X_STR_ADDR, ScreenHandler.UpdateAxisString),

    // 信息页面
    VP_HELPER_TX(VP_UsedTime, ScreenHandler.UpdateTimeInfo),   //
    VP_HELPER_TX(VP_FW_Ver, ScreenHandler.UpdateVersionInfo),  //
    VP_HELPER_TX(VP_UI_Ver, ScreenHandler.UpdateVersionInfo),  //

    // 冷却页面
    VP_HELPER_TX(VP_T_MAX_Cur, ScreenHandler.UpdateTemperatureMax),

    // 加热页面
    VP_HELPER_TX(VP_T_ACT_Cur, ScreenHandler.UpdateTemperatureCur),
    VP_HELPER_TX(VP_T_ACT_Tar, ScreenHandler.UpdateTemperatureTar),

    // 步进参数
    VP_HELPER_TX_DWORD(VP_X_STEP_PER_MM, ScreenHandler.UpdateStepPerMM),
    VP_HELPER_TX_DWORD(VP_Y_STEP_PER_MM, ScreenHandler.UpdateStepPerMM),
    VP_HELPER_TX_DWORD(VP_Z_STEP_PER_MM, ScreenHandler.UpdateStepPerMM),
    VP_HELPER_TX_DWORD(VP_E_STEP_PER_MM, ScreenHandler.UpdateStepPerMM),
    VP_HELPER_RX_DWORD(VP_X_STEP_PER_MM_Set, ScreenHandler.HandleStepPerMMSet),
    VP_HELPER_RX_DWORD(VP_Y_STEP_PER_MM_Set, ScreenHandler.HandleStepPerMMSet),
    VP_HELPER_RX_DWORD(VP_Z_STEP_PER_MM_Set, ScreenHandler.HandleStepPerMMSet),
    VP_HELPER_RX_DWORD(VP_E_STEP_PER_MM_Set, ScreenHandler.HandleStepPerMMSet),

  #if HAS_FAN
    // 风扇参数
    VP_HELPER_TX(VP_FanT_Percentage, ScreenHandler.UpdateFan),
    VP_HELPER_TX(VP_Fan0_Percentage, ScreenHandler.UpdateFan),
    VP_HELPER_TX(VP_Fan1_Percentage, ScreenHandler.UpdateFan),
    VP_HELPER_RX(VP_FanT_Percentage_Set, ScreenHandler.HandleFanSet),
    VP_HELPER_RX(VP_Fan0_Percentage_Set, ScreenHandler.HandleFanSet),
    VP_HELPER_RX(VP_Fan1_Percentage_Set, ScreenHandler.HandleFanSet),
  #endif

  // 预热参数
  #if HAS_HOTEND
    VP_HELPER_TX(VP_PreHeat_T_E0, ScreenHandler.UpdatePreHeat),
    VP_HELPER_RX(VP_PreHeat_T_E0_Set, ScreenHandler.HandlePreheadSet),
  #endif
  #if HAS_MULTI_HOTEND
    VP_HELPER_TX(VP_PreHeat_T_E1, nullptr), VP_HELPER_RX(VP_PreHeat_T_E1_Set, nullptr),
  #endif
  #if HOTENDS > 2
    VP_HELPER_TX(VP_PreHeat_T_E2, nullptr), VP_HELPER_RX(VP_PreHeat_T_E2_Set, nullptr),
  #endif
  #if HAS_HEATED_BED
    VP_HELPER_TX(VP_PreHeat_T_Bed, ScreenHandler.UpdatePreHeat),
    VP_HELPER_RX(VP_PreHeat_T_Bed_Set, ScreenHandler.HandlePreheadSet),
  #endif
  #if HAS_HEATED_CHAMBER
    VP_HELPER_TX(VP_PreHeat_T_Chamber, ScreenHandler.UpdatePreHeat),
    VP_HELPER_RX(VP_PreHeat_T_Chamber_Set, ScreenHandler.HandlePreheadSet),
  #endif
  #if HAS_FAN
    VP_HELPER_TX(VP_PreHeat_Fan, nullptr), VP_HELPER_RX(VP_PreHeat_Fan_Set, nullptr),
  #endif

    // 更多设置页面
    VP_HELPER_TX(VP_CaseLight, ScreenHandler.UpdateSwitchIcon),
    VP_HELPER_TX(VP_AutoPowerOff, ScreenHandler.UpdateSwitchIcon),
    VP_HELPER_TX(VP_AutoBackMain, ScreenHandler.UpdateSwitchIcon),

    // BabyStep参数
    VP_HELPER_TX(VP_BabyStepZ, ScreenHandler.UpdateBabyStep),

  #if HAS_LEVELING
    // 自动调平参数
    VP_HELPER_TX(VP_ServoOffsetZ, ScreenHandler.UpdateProbeOffsetZ),
    VP_HELPER_RX(VP_ServoOffsetZ_Set, ScreenHandler.HandleProbeOffsetZSet),
    VP_HELPER_TX(VP_FadeHeight, ScreenHandler.UpdateFadeHeight),
    VP_HELPER_RX(VP_FadeHeight_Set, ScreenHandler.HandleFadeHeightSet),
  #endif

    // WIFI参数
    // VP_WIFI_MODE_INDEX_ADDR
    // VP_WIFI_STATE_INDEX_ADDR
    // VP_WIFI_SCAN_LAST_ICO_ADDR
    // VP_WIFI_SCAN_NEXT_ICO_ADDR
    // VP_WIFI_IP_ADDR
    // VP_WIFI_IP_ADDR
    // VP_WIFI_SSID_ADDR
    // VP_WIFI_KEY_ADDR
    // VP_WIFI_KEY_MASK_ADDR
    // VP_WIFI_UUID_ADDR
    // VP_WIFI_SSID_SET_ADDR

    // 截至项
    VP_HELPER(0, 0, 0, 0, 0)};

constexpr const char MarlinVersion[] PROGMEM = {
    // Version
    SHORT_BUILD_VERSION[0], SHORT_BUILD_VERSION[1], SHORT_BUILD_VERSION[2], '.',

    // YY year
    __DATE__[9], __DATE__[10],

    // First month letter, Oct Nov Dec = '1' otherwise '0'
    (__DATE__[0] == 'O' || __DATE__[0] == 'N' || __DATE__[0] == 'D') ? '1' : '0',

    // Second month letter
    (__DATE__[0] == 'J')   ? ((__DATE__[1] == 'a') ? '1' : ((__DATE__[2] == 'n') ? '6' : '7'))  // Jan, Jun or Jul
    : (__DATE__[0] == 'F') ? '2'                                                                // Feb
    : (__DATE__[0] == 'M') ? (__DATE__[2] == 'r') ? '3' : '5'                                   // Mar or May
    : (__DATE__[0] == 'A') ? (__DATE__[1] == 'p') ? '4' : '8'                                   // Apr or Aug
    : (__DATE__[0] == 'S') ? '9'                                                                // Sep
    : (__DATE__[0] == 'O') ? '0'                                                                // Oct
    : (__DATE__[0] == 'N') ? '1'                                                                // Nov
    : (__DATE__[0] == 'D') ? '2'                                                                // Dec
                           : 0,

    // First day letter, replace space with digit
    __DATE__[4] == ' ' ? '0' : __DATE__[4],

    // Second day letter
    __DATE__[5],

    '\0'};

#endif  // DGUS_LCD_UI_CREATBOT
