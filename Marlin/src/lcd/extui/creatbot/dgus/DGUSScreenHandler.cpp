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

#if ENABLED(DGUS_LCD_UI_CREATBOT)

  #if ENABLED(CREATBOT_WIFI_SUPPORT)
    #include "../../../../feature/wifi/WiFiSocket.h"
  #endif

  #include "../../../../module/planner.h"
  #include "../../../../module/settings.h"
  #include "../../../../module/stepper.h"
  #include "../../../../module/temperature.h"
  #include "../../ui_api.h"
  #include "../creatbotFunction.h"
  #include "DGUSDisplay.h"
  #include "DGUSScreenHandler.h"
  #include "DGUSScreenHandlerHelper.h"

using namespace ExtUI;

DGUSScreenHandler ScreenHandler;

DGUSLCD_Screens DGUSScreenHandler::curScreen;
DGUSLCD_Screens DGUSScreenHandler::preScreen;
DGUSLCD_Screens DGUSScreenHandler::delayScreen;
millis_t        DGUSScreenHandler::delayJumpTime;
uint8_t         DGUSScreenHandler::updatePtr;
uint16_t        DGUSScreenHandler::skipVP;
bool            DGUSScreenHandler::ScreenComplete;
bool            DGUSScreenHandler::ChangeComplete;
bool            DGUSScreenHandler::InTouch;
uint16_t        DGUSScreenHandler::popIndex;
uint16_t        DGUSScreenHandler::delayPop;

static millis_t next_event_ms     = 0;  // 下次屏幕更新时间
static millis_t return_default_ms = 0;  // 返回默认页的时间

static DGUSSetting ScreenSetting;

static uint16_t axisMoveUnit            = MOVE_UNIT_10MM;  // 移轴单位
static uint16_t FilamentExtrudeDistance = 30;              // 耗材挤出或回抽测试的长度

// clang-format off
#if ENABLED(SDSUPPORT)
  static ExtUI::FileList filelist;
  static uint8_t page_num                   = 0;
  static uint8_t file_page[MAX_DIR_DEPTH]   = {1};
  static uint8_t file_item_selected         = -1;
  static uint8_t fileIco[LCD_FILE_PER_PAGE] = {0};

  //目录项索引刷新
  static inline void SDCardIndexRefresh() {
    filelist.refresh();
    file_item_selected = -1;
    page_num = (filelist.count() + LCD_FILE_PER_PAGE - 1) / LCD_FILE_PER_PAGE;
  }

  //目录项索引初始化
  static inline void SDCardIndexInit() {
    memset(file_page, 1, sizeof(file_page));
    SDCardIndexRefresh();
  }

  //目录项索引下级目录
  static inline void SDCardIndexCD() {
    memmove(&file_page[1], &file_page[0], sizeof(file_page) - 1);
    file_page[0]       = 1;
    file_item_selected = -1;
    page_num           = (filelist.count() + LCD_FILE_PER_PAGE - 1) / LCD_FILE_PER_PAGE;
  }
  //目录项索引上级目录
  static inline void SDCardIndexUP() {
    memmove(&file_page[0], &file_page[1], sizeof(file_page) - 1);
    file_page[sizeof(file_page) - 1] = 1;
    file_item_selected               = -1;
    page_num                         = (filelist.count() + LCD_FILE_PER_PAGE - 1) / LCD_FILE_PER_PAGE;
  }
  //目录项索引下一页
  static inline void SDCardIndexNext() {
    if (file_page[0] < page_num) file_page[0]++;
  }
  //目录项索引上一页
  static inline void SDCardIndexLast() {
    if (file_page[0] > 1) file_page[0]--;
  }
#endif
// clang-format on

// 立即停止电机动作
static inline void quickStop(bool resetCounter = true) {
  quickstop_stepper();
  if (resetCounter) planner.cleaning_buffer_counter = 0;
}

// 自动关机时间重置
static inline void powerReset() {
  #if ENABLED(AUTO_POWER_CONTROL)
  if (powerManager.psu_on) powerManager.power_on();
  #endif
}

// 处理屏幕触摸
void DGUSScreenHandler::TouchHook(DGUS_VP_Variable &var, void *val_ptr) {
  uint8_t        *tmp    = (uint8_t *)val_ptr;
  DGUSLCD_Screens target = (DGUSLCD_Screens)tmp[1];

  DEBUG_ECHOLNPGM(" TouchHook ", target);
  if ((target & 0xFD) == 0)  // 过滤 0x00 和 0x02
    InTouch = false;
  else
    InTouch = true;
  powerReset();
}

// 处理当前页面
void DGUSScreenHandler::PageHook(DGUS_VP_Variable &var, void *val_ptr) {
  uint8_t        *tmp    = (uint8_t *)val_ptr;
  DGUSLCD_Screens target = (DGUSLCD_Screens)tmp[1];

  DEBUG_ECHOLNPGM(" PageHook ", target);

  if (dgusdisplay.needBoot()) {
    if (target < DGUSLCD_SCREEN_BOOT_END) {
      TERN_(SHOW_BOOTSCREEN, dgusdisplay.setBootState(false));
    } else {
      TERN(SHOW_BOOTSCREEN, dgusdisplay.RequestScreen(DGUSLCD_SCREEN_BOOT), dgusdisplay.setBootState(false));
    }
    return;
  }

  if (target < DGUSLCD_SCREEN_BOOT_END)  // 开机动画
    return;
  else if (target == DGUSLCD_SCREEN_BOOT_END) {  // 开机画面结束
    UpdateScreenInitData();                      // 初始化屏幕关键数据
    if (popIndex)
      GotoPopup(popIndex);  // 跳转到弹窗页面
    else if (TERN1(RECOVERY_USE_EEPROM, !recovery.check()))
      GotoDefault();  // 跳转到默认页面
  } else {
    if (curScreen == target) {  // 屏幕页面与系统页面一致
      if (!ChangeComplete) ChangeComplete = true;
    } else {  // 屏幕页面跳转通知系统
      if (ChangeComplete) UpdateNewScreen(target);
    }
  }
}

// 返回默认页
void DGUSScreenHandler::HandlePageDefault(DGUS_VP_Variable &, void *) { GotoDefault(); }

// 打印状态设置
void DGUSScreenHandler::HandlePrintStatuSet(DGUS_VP_Variable &VP, void *) {
  DEBUG_ECHOLNPGM("HandlePrintStatuSet");
  switch (VP.match) {
    case CMD_PAUSE_BUTTON:
      if (ExtUI::isPrintingFromMedia() && !ExtUI::isPrintingFromMediaPaused()) ExtUI::pausePrint();
      break;
    case CMD_REUSE_BUTTON:
      if (isMediaInserted()) {
        if (TERN1(HAS_FILAMENT_SENSOR, ExtUI::getFilamentInsertState())) {
          if (ExtUI::isPrintingFromMediaPaused())
            ExtUI::resumePrint();
          else if (TERN0(HAS_RESUME_CONTINUE, wait_for_user)) {
            wait_for_user = false;
            GotoDefault();  // 跳转到默认页面
          }
        } else {
          GotoPopup(POP2_ICO_INSERT_CONTIUNE);
        }
      }
      break;
    case CMD_CANCEL_BUTTON:
      if (ExtUI::isPrintingFromMedia() || TERN0(POWER_LOSS_RECOVERY, recovery.recoveryPrepare))
        GotoPopup(POP2_ICO_STOP_CONFIRM);
      break;
    default: break;
  }
  // GotoDefault();
}

// 中止打印
void DGUSScreenHandler::HandleStopPrint(DGUS_VP_Variable &VP, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleStopPrint");
  int16_t newvalue = swap16(*(uint16_t *)val_ptr);
  ExtUI::stopPrint();
  // GotoDefault();
}

// 预热按钮
void DGUSScreenHandler::HandlePreheat(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandlePreheat");
  #if HAS_HOTEND
  ExtUI::setTargetTemp_celsius(ScreenSetting.preheat_T_temp, ExtUI::heater_t::H0);
  #endif
  #if HAS_HEATED_BED
  ExtUI::setTargetTemp_celsius(ScreenSetting.preheat_B_temp, ExtUI::heater_t::BED);
  #endif
  #if HAS_HEATED_CHAMBER
  ExtUI::setTargetTemp_celsius(ScreenSetting.preheat_C_temp, ExtUI::heater_t::CHAMBER);
  #endif
  GotoDefault();
}

// 冷却按钮
void DGUSScreenHandler::HandleCoolDown(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleCoolDown");
  ExtUI::coolDown();
  GotoDefault();
}

// 耗材按钮
void DGUSScreenHandler::HandleFilament(DGUS_VP_Variable &, void *) { GotoScreen(DGUSLCD_SCREEN_FILAMENT); }

// 送(退)丝按钮
void DGUSScreenHandler::HandleManualExtrude(DGUS_VP_Variable &var, void *) {
  DEBUG_ECHOLNPGM("HandleManualExtrude");
  extruder_t curTool = ExtUI::getActiveTool();
  if (thermalManager.targetTooColdToExtrude(curTool)) {
    GotoPopup(POP1_ICO_TAR_TEMP_LOW);
  } else if (thermalManager.tooColdToExtrude(curTool)) {
    GotoPopup(POP1_ICO_CUR_TEMP_LOW);
  } else {
    switch (var.match) {
      case CMD_FILAMENT_EXT:
        UI_INCREMENT_BY(AxisPosition_mm, FilamentExtrudeDistance, curTool);
        GotoPopup(POP1_ICO_EXTRUDE_FILAMENT);
        break;
      case CMD_FILAMENT_REC:
        UI_DECREMENT_BY(AxisPosition_mm, FilamentExtrudeDistance, curTool);
        GotoPopup(POP1_ICO_RECRACT_FILAMENT);
        break;
      default: return;
    }
  }
}

// 加(卸)载按钮
void DGUSScreenHandler::HandleLoadFilament(DGUS_VP_Variable &var, void *) {
  DEBUG_ECHOLNPGM("HandleLoadFilament");
  extruder_t curTool = ExtUI::getActiveTool();
  if (thermalManager.targetTooColdToExtrude(curTool)) {
    ExtUI::setTargetTemp_celsius(ScreenSetting.preheat_T_temp, curTool);
  }
  switch (var.match) {
    case CMD_FILAMENT_LOAD: ExtUI::injectCommands(F("M701")); break;
    case CMD_FILAMENT_UNLOAD: ExtUI::injectCommands(F("M702")); break;
  }
}

// 更换耗材按钮
void DGUSScreenHandler::HandleFilamentChange(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleFilamentChange");
  if (ExtUI::isPrintingFromMedia() && !ExtUI::isPrintingFromMediaPaused()) {
    ExtUI::injectCommands(F("M600"));
    GotoPopup(POP0_ICO_WAITING);
  }
}

// 回零按钮
void DGUSScreenHandler::HandleHome(DGUS_VP_Variable &var, void *var_ptr) {
  DEBUG_ECHOLNPGM("HandleHome");
  switch (var.match) {  // clang-format off
    case CMD_MOVE_HOME_ALL: ExtUI::injectCommands(F("G28")); break;
    case CMD_MOVE_HOME_X:   ExtUI::injectCommands(F("G28X")); break;
    case CMD_MOVE_HOME_Y:   ExtUI::injectCommands(F("G28Y")); break;
    case CMD_MOVE_HOME_Z:   ExtUI::injectCommands(F("G28Z")); break;
    default: return;
  }  // clang-format on
  ForceCompleteUpdate();
}

// 移轴按钮
void DGUSScreenHandler::HandleManualMove(DGUS_VP_Variable &var, void *var_ptr) {
  DEBUG_ECHOLNPGM("HandleManualMove");
  float_t offset;

  switch (axisMoveUnit) {  // clang-format off
    case MOVE_UNIT_01MM:    offset = 0.1;   break;
    case MOVE_UNIT_1MM:     offset = 1.0;   break;
    case MOVE_UNIT_10MM:    offset = 10.0;  break;
    case MOVE_UNIT_100MM:   offset = 100.0; break;
    default:                offset = 10.0;  break;
  } 
  switch (var.match) {
    case CMD_MOVE_X_MINUS:  offset *= -1;
    case CMD_MOVE_X_PLUS:   delayMoveAxis(offset, ExtUI::axis_t::X); break;
    case CMD_MOVE_Y_MINUS:  offset *= -1;
    case CMD_MOVE_Y_PLUS:   delayMoveAxis(offset, ExtUI::axis_t::Y); break;
    case CMD_MOVE_Z_MINUS:  offset *= -1;
    case CMD_MOVE_Z_PLUS:   delayMoveAxis(offset, ExtUI::axis_t::Z); break;
    default: return;
  }  // clang-format on
  ForceCompleteUpdate();
}

// 暂停时Z轴向上移动按钮
void DGUSScreenHandler::HandleZAxisUp(DGUS_VP_Variable &var, void *var_ptr) {
  UI_DECREMENT_BY(AxisPosition_mm, 1, axis_t::Z);
}

// 暂停时Z轴向下移动按钮
void DGUSScreenHandler::HandleZAxisDown(DGUS_VP_Variable &var, void *vat_ptr) {
  UI_INCREMENT_BY(AxisPosition_mm, 1, axis_t::Z);
}

// 打印时BabyStep Up
void DGUSScreenHandler::HandleBabyStepUp(DGUS_VP_Variable &var, void *vat_ptr) {
  int16_t steps = mmToWholeSteps(-0.02, ExtUI::Z);
  smartAdjustAxis_steps(steps, ExtUI::Z, true);
}

// 打印时BabyStep Down
void DGUSScreenHandler::HandleBabyStepDown(DGUS_VP_Variable &var, void *vat_ptr) {
  int16_t steps = mmToWholeSteps(0.02, ExtUI::Z);
  smartAdjustAxis_steps(steps, ExtUI::Z, true);
}

// 进入DFU模式按钮
void DGUSScreenHandler::HandleRebootIntoDFUMode(DGUS_VP_Variable &var, void *) { SystemResetToBootloader(); }

// 设置菜单按钮
void DGUSScreenHandler::HandleSetting(DGUS_VP_Variable &, void *) { GotoScreen(DGUSLCD_SCREEN_SETTING); }

// 调整菜单按钮
void DGUSScreenHandler::HandleAdjust(DGUS_VP_Variable &, void *) { GotoScreen(DGUSLCD_SCREEN_ADJUST); }

// 保存设置
void DGUSScreenHandler::HandleStore(DGUS_VP_Variable &, void *) {
  GotoPopup(POP0_ICO_SETTING_SAVING);
  settings.save();
}

// 恢复出厂
void DGUSScreenHandler::HandleReset(DGUS_VP_Variable &, void *) { GotoPopup(POP2_ICO_RESET_CONFIRM); }

  #if ENABLED(POWER_LOSS_RECOVERY)
// 断电续打继续按钮
void DGUSScreenHandler::HandleAccidentReuse(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleAccidentReuse");
  if (recovery.interrupted_file_exists()) {
    ExtUI::injectCommands(F("M1000"));
  }
}

// 断电续打取消按钮
void DGUSScreenHandler::HandleAccidentCancel(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleAccidentCancel");
  ExtUI::injectCommands(F("M1000 C"));
}
  #endif

// 取消关机按钮
void DGUSScreenHandler::HandleCancelShutdown(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleCancelShutdown");
  TERN_(POWER_OFF_WAIT_FOR_COOLDOWN, powerManager.cancelAutoPowerOff());
  if (TERN1(RECOVERY_USE_EEPROM, !recovery.check())) GotoDefault();
}

// 加热等待取消按钮
void DGUSScreenHandler::HandleHeatSkip(DGUS_VP_Variable &, void *) {
  wait_for_heatup = false;
  GotoScreen(preScreen);
}

// 弹窗按钮
void DGUSScreenHandler::HandlePopButton(DGUS_VP_Variable &var, void *var_ptr) {
  uint16_t keyValue = var.match;
  if (curScreen == DGUSLCD_SCREEN_POP_1 && keyValue == CMD_POP1_BUTTON) {  // 弹窗页面1触发按键
    switch (popIndex) {
      case POP1_ICO_EXTRUDE_FILAMENT:  // 挤出耗材
      case POP1_ICO_RECRACT_FILAMENT:  // 回抽退丝
      case POP1_ICO_LOAD_FILAMENT:     // 加载耗材
      case POP1_ICO_UNLOAD_FILAMENT:   // 卸载耗材
        quickStop();
        stepper.disable_e_steppers();
        break;
      case POP1_ICO_MOVING:  // 移轴
        quickStop();
        break;
      case POP1_ICO_HOMING:  // 回零
        quickStop(false);
        break;
      case POP1_ICO_PROBING:  // 测量
        quickStop(false);
        break;
      case POP1_ICO_PROBER_UNAVAILABLE:  // 探针不可用
      case POP1_ICO_TAR_TEMP_LOW:        // 目标温度过低
      case POP1_ICO_CUR_TEMP_LOW:        // 当前温度过低
        GotoScreen(preScreen);
        break;
      case POP1_ICO_HEATER_TIMEOUT:  // 加热器超时
        ExtUI::setUserConfirmed();
        break;
    }
  } else if (curScreen == DGUSLCD_SCREEN_POP_2 &&
             (keyValue == CMD_POP2_BUTTON1 || keyValue == CMD_POP2_BUTTON2)) {  // 弹窗页面2触发按键
    switch (popIndex) {
      case POP2_ICO_CHANGE_CONFIRM:
        switch (keyValue) {
          case CMD_POP2_BUTTON1: break;
          case CMD_POP2_BUTTON2: break;
        }
        break;
      case POP2_ICO_RESET_CONFIRM:
        switch (keyValue) {
          case CMD_POP2_BUTTON1:  // OK
            GotoPopup(POP0_ICO_SETTING_RESTORING);
            settings.reset();
  #if ENABLED(CREATBOT_WIFI_SUPPORT)
            myWifi.resetWifiInfo();  // wifi模块恢复出厂设置
  #endif
            break;
          case CMD_POP2_BUTTON2:  // Cancel
            GotoScreen(DGUSLCD_SCREEN_SETTING);
            break;
        }
        break;
      case POP2_ICO_SKIP_CONFIRM:
        switch (keyValue) {
          case CMD_POP2_BUTTON1:  // Skip
            TERN_(HAS_LEVELING, ExtUI::setLevelingActive(true));
            TERN_(HAS_LEVELING, HandleSettingLeveling(*(DGUS_VP_Variable *)nullptr, nullptr));
            break;
          case CMD_POP2_BUTTON2:  // Calibrate
            TERN_(HAS_LEVELING, HandleLevelingCalibrate(*(DGUS_VP_Variable *)nullptr, nullptr));
            break;
        }
        break;
      case POP2_ICO_STOP_CONFIRM:
        switch (keyValue) {
          case CMD_POP2_BUTTON1:  // Yes
            quickStop(false);
            ExtUI::stopPrint();
            break;
          case CMD_POP2_BUTTON2:  // No
            GotoScreen(preScreen);
            break;
        }
        break;
      case POP2_ICO_INSERT_CONTIUNE:
        switch (keyValue) {
          case CMD_POP2_BUTTON1:  // Continue
            if (TERN1(HAS_FILAMENT_SENSOR, ExtUI::getFilamentInsertState())) {
              if (ExtUI::isPrintingFromMedia() || ExtUI::isPrintingFromSerial())
                ExtUI::resumePrint();
              else
                wait_for_user = false;
            }
            break;
          case CMD_POP2_BUTTON2:  // Back
            if (ExtUI::isPrintingFromMedia() || ExtUI::isPrintingFromSerial())
              GotoDefault();
            else {
              wait_for_user = false;
              quickStop(false);
              GotoScreen(DGUSLCD_SCREEN_FILAMENT);
            }
            break;
        }
        break;
      case POP2_ICO_LOAD_OPTION:
        switch (keyValue) {
          case CMD_POP2_BUTTON1:  // Continue
            TERN_(M600_PURGE_MORE_RESUMABLE, ExtUI::setPauseMenuResponse(PAUSE_RESPONSE_RESUME_PRINT));
            break;
          case CMD_POP2_BUTTON2:  // Load
            TERN_(M600_PURGE_MORE_RESUMABLE, ExtUI::setPauseMenuResponse(PAUSE_RESPONSE_EXTRUDE_MORE));
            break;
        }
        break;
    }
  }
}

  #if ENABLED(SDSUPPORT)
// U盘插入
void DGUSScreenHandler::SDCardInserted() {
  switch (curScreen) {  // clang-format off
    case DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE:
    case DGUSLCD_SCREEN_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE:
      GotoDefault();
      break;
  }  // clang-format on
  SDCardIndexInit();
}

// U盘异常  //TODO CreatBot
void DGUSScreenHandler::SDCardError() {
  DGUSScreenHandler::SDCardRemoved();
  // sendinfoscreen(F("NOTICE"), nullptr, F("SD card error"), nullptr, true, true, true, true);
  // SetupConfirmAction(nullptr);
  // GotoScreen(DGUSLCD_SCREEN_POPUP);
}

// U盘移除
void DGUSScreenHandler::SDCardRemoved() {
  switch (curScreen) {  // clang-format off
    case DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT:
    case DGUSLCD_SCREEN_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT:
    case DGUSLCD_SCREEN_FILE_ONE:
    case DGUSLCD_SCREEN_FILE_NEXT:
    case DGUSLCD_SCREEN_FILE_LAST:
    case DGUSLCD_SCREEN_FILE_BOTH:
    case DGUSLCD_SCREEN_FILE_UP_ONE:
    case DGUSLCD_SCREEN_FILE_UP_NEXT:
    case DGUSLCD_SCREEN_FILE_UP_LAST:
    case DGUSLCD_SCREEN_FILE_UP_BOTH:
    case DGUSLCD_SCREEN_PRINT_CONFIRM: 
      GotoDefault(); 
      break;
    case DGUSLCD_SCREEN_DEFAULT_PRINT_PAUSE:  // TODO 打印状态拔出U盘等于暂停打印
      if (ExtUI::isPrintingFromMedia() && !ExtUI::isPrintingFromMediaPaused()) ExtUI::pausePrint();
      break;
    case DGUSLCD_SCREEN_DEFAULT_PRINT_RESUME: 
      break;
  }  // clang-format on
  SDCardIndexInit();
}

// U盘按钮
void DGUSScreenHandler::HandleFileOpen(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleFileOpen");
  uint8_t         curPage    = file_page[0];
  DGUSLCD_Screens fileScreen = DGUSLCD_SCREEN_FILE_ONE;
  SDCardIndexRefresh();
  if (filelist.isAtRootDir()) {
    if (page_num <= 1)
      fileScreen = DGUSLCD_SCREEN_FILE_ONE;
    else {
      if (curPage == 1) fileScreen = DGUSLCD_SCREEN_FILE_NEXT;
      if (curPage == page_num) fileScreen = DGUSLCD_SCREEN_FILE_LAST;
      if (curPage > 1 && curPage < page_num) fileScreen = DGUSLCD_SCREEN_FILE_BOTH;
    }
  } else {
    if (page_num <= 1)
      fileScreen = DGUSLCD_SCREEN_FILE_UP_ONE;
    else {
      if (curPage == 1) fileScreen = DGUSLCD_SCREEN_FILE_UP_NEXT;
      if (curPage == page_num) fileScreen = DGUSLCD_SCREEN_FILE_UP_LAST;
      if (curPage > 1 && curPage < page_num) fileScreen = DGUSLCD_SCREEN_FILE_UP_BOTH;
    }
  }
  GotoScreen(fileScreen);
}

// U盘列表项
void DGUSScreenHandler::HandleFileAction(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleFileAction");
  uint16_t target_line = (var.match - CMD_FILE_ITEM_0_ACTION);
  if (target_line >= LCD_FILE_PER_PAGE) return;

  uint16_t fileIndex = (file_page[0] - 1) * LCD_FILE_PER_PAGE + target_line;
  if (filelist.seek(fileIndex)) {
    if (filelist.isDir()) {
      filelist.changeDir(filelist.shortFilename());  // 打开下级目录
      SDCardIndexCD();
      HandleFileOpen(*(DGUS_VP_Variable *)nullptr, nullptr);

    #if ENABLED(CREATBOT_WIFI_SUPPORT)
      myWifi.actionSynFilesInfo();
    #endif

    } else {
      file_item_selected = fileIndex;
      GotoScreen(DGUSLCD_SCREEN_PRINT_CONFIRM);  // 打印确认屏幕
    }
  }
}

// 确认打印页面更新文件名
void DGUSScreenHandler::UpdatePrintFileName(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdatePrintFileName");
  char tmpfilename[LCD_FILENAME_LENGTH] = "";
  if (filelist.seek((uint16_t)file_item_selected)) {
    const char *filename     = filelist.filename();
    const char *longFilename = filelist.longFilename();
    char       *c            = tmpfilename;
    #if ENABLED(UTF_FILENAME_SUPPORT, UTF_IS_UTF16)
    if (longFilename[0] || longFilename[1]) {
      while (*(uint16_t *)longFilename) {  // 小端转为大端
        *(c++) = *(longFilename + 1);
        *(c++) = *(longFilename);
        longFilename += 2;
        if ((longFilename + 2 - filelist.longFilename()) >= LONG_FILENAME_LENGTH) break;
      }
    } else
    #endif
    {
      while (*filename) {
        *(c++) = 0x00;
        *(c++) = *(filename++);
      }
    }
    dgusdisplay.WriteUnicodeString(var.VP, tmpfilename, LCD_FILENAME_LENGTH);
  }
}

// U盘下一页
void DGUSScreenHandler::HandleFileNext(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleFileNext");
  SDCardIndexNext();
  HandleFileOpen(*(DGUS_VP_Variable *)nullptr, nullptr);

    #if ENABLED(CREATBOT_WIFI_SUPPORT)
  myWifi.actionSynFilesInfo();
    #endif
}

// U盘上一页
void DGUSScreenHandler::HandleFileLast(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleFileLast");
  SDCardIndexLast();
  HandleFileOpen(*(DGUS_VP_Variable *)nullptr, nullptr);

    #if ENABLED(CREATBOT_WIFI_SUPPORT)
  myWifi.actionSynFilesInfo();
    #endif
}

// U盘上一级
void DGUSScreenHandler::HandleFileUp(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleFileUp");
  filelist.upDir();
  SDCardIndexUP();
  HandleFileOpen(*(DGUS_VP_Variable *)nullptr, nullptr);

    #if ENABLED(CREATBOT_WIFI_SUPPORT)
  myWifi.actionSynFilesInfo();
    #endif
}

// U盘文件打印
void DGUSScreenHandler::HandleFilePrint(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleFilePrint");
  if (filelist.seek(file_item_selected)) {
    #if HAS_FILAMENT_SENSOR
    if (ExtUI::getFilamentRunoutState()) ExtUI::setFilamentRunoutState(false);  // 重置耗材检测触发状态
    #endif

    ExtUI::printFile(filelist.shortFilename());
    // GotoDefault();   //页面跳转被PrintTimerStartHook执行 此时跳转U盘状态不对
  }
}

// U盘文件取消
void DGUSScreenHandler::HandleFileCancel(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleFileCancel");
  file_item_selected = -1;
  HandleFileOpen(*(DGUS_VP_Variable *)nullptr, nullptr);
}

// U盘更新文件名
void DGUSScreenHandler::UpdateFileName(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateFileName");
  uint16_t target_line = (var.VP - VP_SD_FileName0) / VP_SD_FileNameGap;
  if (target_line >= LCD_FILE_PER_PAGE) return;

  fileIco[target_line]                      = 0;
  char     tmpfilename[LCD_FILENAME_LENGTH] = "";
  uint16_t fileIndex                        = (file_page[0] - 1) * LCD_FILE_PER_PAGE + target_line;

  if (filelist.seek(fileIndex)) {
    fileIco[target_line]     = filelist.isDir() ? 2 : 1;
    const char *filename     = filelist.filename();
    const char *longFilename = filelist.longFilename();
    char       *c            = tmpfilename;
    #if ENABLED(UTF_FILENAME_SUPPORT, UTF_IS_UTF16)
    if (longFilename[0] || longFilename[1]) {
      while (*(uint16_t *)longFilename) {  // 小端转为大端
        *(c++) = *(longFilename + 1);
        *(c++) = *(longFilename);
        longFilename += 2;
        if ((longFilename + 2 - filelist.longFilename()) >= LONG_FILENAME_LENGTH) break;
      }
    } else
    #endif
    {
      while (*filename) {
        *(c++) = 0x00;
        *(c++) = *(filename++);
      }
    }
  }
  dgusdisplay.WriteUnicodeString(var.VP, tmpfilename, LCD_FILENAME_LENGTH);
}

// U盘更新文件图标
void DGUSScreenHandler::UpdateFileICO(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateFileICO");
  uint16_t target_line = (var.VP - VP_SD_FileICO_0) / VP_SD_FileICOGap;
  if (target_line >= LCD_FILE_PER_PAGE) return;
  dgusdisplay.WriteVariable(var.VP, (uint16_t)fileIco[target_line]);
}
  #endif

// 更新最大温度
void DGUSScreenHandler::UpdateTemperatureMax(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateTemperatureMax");
  auto getMaxTemp = []() {
    celsius_float_t maxTemp = 0;
    HOTEND_LOOP() { maxTemp = _MAX(maxTemp, ExtUI::getActualTemp_celsius(ExtUI::extruder_t(e))); }
    return maxTemp;
  };
  UpdateValToLCD(getMaxTemp, var);
}

// 更新当前温度
void DGUSScreenHandler::UpdateTemperatureCur(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateTemperatureCur");
  UpdateValToLCD<ExtUI::heater_t, 0, int16_t>(ExtUI::getActualTemp_celsius, var);
  UpdateValToLCD<ExtUI::extruder_t, 0, int16_t>(ExtUI::getActualTemp_celsius, var);  // For Active Tool
}

// 更新目标温度
void DGUSScreenHandler::UpdateTemperatureTar(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateTemperatureTar");
  UpdateValToLCD<ExtUI::heater_t>(ExtUI::getTargetTemp_celsius, var);
  UpdateValToLCD<ExtUI::extruder_t>(ExtUI::getTargetTemp_celsius, var);  // For Active Tool
}

// 设置目标温度
void DGUSScreenHandler::HandleTemperatureSet(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleTemperatureSet");
  SetValFromLCD<ExtUI::heater_t>(ExtUI::setTargetTemp_celsius, var, val_ptr);
  GotoDefault();
}

// 更新全局速度
void DGUSScreenHandler::UpdateFeedrate(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateFeedrate");
  UpdateValToLCD(ExtUI::getFeedrate_percent, var);
}

// 设置全局速度
void DGUSScreenHandler::HandleFeedrateSet(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleFeedrateSet");
  SetValFromLCD(ExtUI::setFeedrate_percent, var, val_ptr);
}

// 更新当前活动挤出机
void DGUSScreenHandler::UpdateActivatedExtruder(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("HandleActivatedExtruder");
  auto getExtIndex = []() {
    ExtUI::extruder_t extIndex = getActiveTool();
  #if HOTENDS > 2
    return extIndex + 4;
  #elif HOTENDS > 1
    return extIndex + 2;
  #else
    return extIndex + 1;
  #endif
  };
  UpdateValToLCD(getExtIndex, var);
}

// 设置当前活动挤出机
void DGUSScreenHandler::HandleActivatedExtruderSet(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleActivatedExtruderSet");
  auto setExtIndex = [](uint16_t index) {
    switch (index) {
  #if HAS_MULTI_HOTEND
      case Active_Extruder_E0: ExtUI::setActiveTool(ExtUI::extruder_t::E0, DISABLED(DUAL_X_CARRIAGE)); break;
      case Active_Extruder_E1: ExtUI::setActiveTool(ExtUI::extruder_t::E1, DISABLED(DUAL_X_CARRIAGE)); break;
    #if HOTENDS > 2
      case Active_Extruder_E2: ExtUI::setActiveTool(ExtUI::extruder_t::E2, DISABLED(DUAL_X_CARRIAGE)); break;
    #endif
  #endif
      default: break;
    }
  };
  SetValFromLCD(setExtIndex, var, val_ptr);

  skipVP = var.VP;
}

// 更新耗材测试的长度
void DGUSScreenHandler::UpdateFilamentLength(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateFilamentLength");
  UpdateValToLCD([]() { return FilamentExtrudeDistance; }, var);
}

// 设置耗材测试的长度
void DGUSScreenHandler::HandleFilamentLengthSet(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleFilamentLengthSet");
  SetValFromLCD([](uint16_t len) { FilamentExtrudeDistance = len; }, var, val_ptr);
  skipVP = var.VP;
}

// 更新进度条打印时间百分比信息
void DGUSScreenHandler::UpdatePrintPercentInfo(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("HandlePrintingInfo");

  auto getPercentF = []() -> float {
    return TERN(HAS_PRINT_PROGRESS_PERMYRIAD, ExtUI::getProgress_permyriad() / 100.0, ExtUI::getProgress_percent());
  };

  if (var.VP == VP_PrintPercent) UpdateValToLCD<1>(getPercentF, var);
  #if ENABLED(CreatBot_DGUS_V5)
  else if (var.VP == VP_PrintProgessCanvas)
    DGUSLCD_DrawProgressBar(var.VP, COLOR_ASSIST, 1000);
  else if (var.VP == VP_PrintProgessBrush)
    DGUSLCD_DrawProgressBar(var.VP, COLOR_PRIMARY, getPercentF() * 10);
  #elif ENABLED(CreatBot_DGUS_V6)
  else if (var.VP == VP_PrintProgess)
    DGUSLCD_CutProgressBar(var.VP, getPercentF() * 10);
  #endif
}

// 更新打印机时间
void DGUSScreenHandler::UpdateTimeInfo(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateTimeInfo");
  char time_str[11] = {0};
  if (var.VP == VP_PrintTime) {
    (duration_t(ExtUI::getProgress_seconds_elapsed())).toTimeDWIN(time_str, 3);
    dgusdisplay.WriteVariable(VP_PrintTime, time_str, sizeof(time_str));
  }
  #if ENABLED(PRINTCOUNTER)
  else if (var.VP == VP_UsedTime) {
    printStatistics state   = print_job_timer.getStats();
    duration_t      elapsed = state.printTime;
    elapsed.toTimeDWIN(time_str, 4);
    dgusdisplay.WriteVariable(VP_UsedTime, time_str, sizeof(time_str));
  }
  #endif
}

// 更新X Y Z轴字符串
void DGUSScreenHandler::UpdateAxisString(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateAxisString");
  switch (var.VP) {
    case VP_X_STR_ADDR: dgusdisplay.WriteString(var.VP, "X:", 4); break;
    case VP_Y_STR_ADDR: dgusdisplay.WriteString(var.VP, "Y:", 4); break;
    case VP_Z_STR_ADDR: dgusdisplay.WriteString(var.VP, "Z:", 4); break;
    default: break;
  }
}

// 更新版本信息
void DGUSScreenHandler::UpdateVersionInfo(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateVersionInfo");
  if (var.VP == VP_FW_Ver)
    dgusdisplay.WriteString(var.VP, MarlinVersion, sizeof(MarlinVersion), true);
  else if (var.VP == VP_UI_Ver)
    dgusdisplay.WriteString(var.VP, DGUS_BUILD_VERSION, sizeof(DGUS_BUILD_VERSION), true);
}

// 更新字符串信息
void DGUSScreenHandler::UpdateMsg(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateMsg");
  if (ScreenSetting.isAutoPowerOff) {
  #if ENABLED(AUTO_POWER_CONTROL)
    millis_t aliveTime = powerManager.getAliveTime();
    if (ELAPSED(SEC_TO_MS(LCD_TIME_DISPALY), aliveTime)) {
      char info[30], time[6];
      duration_t(aliveTime / 1000).toTimeShutDown(time);
      sprintf_P(info, PSTR("Shutting down.. "));
      strcat(info, time);
      DGUSLCD_UpdateStatusMsg(info);
    } else {
      ui.reset_status();
    }
  #endif
  }
}

  #if HAS_LEVELING
// 跳转到调平页面
void DGUSScreenHandler::HandleSettingLeveling(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleSettingLeveling");
  if (ExtUI::getLevelingActive())
    if (ExtUI::isPrinting())
      GotoScreen(DGUSLCD_SCREEN_LEVELING_SETTING_DISABLE);
    else
      GotoScreen(DGUSLCD_SCREEN_LEVELING_SETTING_ON);
  else
    GotoScreen(DGUSLCD_SCREEN_LEVELING_SETTING_OFF);
}

// 自动调平开关按钮
void DGUSScreenHandler::HandleLevelingSwitch(DGUS_VP_Variable &, void *) {
  DEBUG_ECHOLNPGM("HandleLevelingSwitch");
  if (ExtUI::getLevelingActive()) {                                      // 自动调平已激活
    ExtUI::setLevelingActive(false);                                     //   直接关闭
    HandleSettingLeveling(*(DGUS_VP_Variable *)nullptr, nullptr);        //
  } else {                                                               // 自动调平未激活
    if (ExtUI::getLevelingIsValid()) {                                   //   调平数据有效
      if (ExtUI::isPrinting()) {                                         //     打印状态
        ExtUI::setLevelingActive(true);                                  //       直接开启
        HandleSettingLeveling(*(DGUS_VP_Variable *)nullptr, nullptr);    //
      } else {                                                           //     非打印状态
        GotoPopup(POP2_ICO_SKIP_CONFIRM);                                //       提示是否重新校准平台数据
      }                                                                  //
    } else {                                                             //   调平数据无效
      if (ExtUI::isPrinting()) {                                         //     打印状态
        GotoPopup(POP1_ICO_PROBER_UNAVAILABLE);                          //       提示当前不能进行校准
      } else {                                                           //     非打印状态
        GotoPopup(POP0_ICO_INVALID_LEVLEL_DATA);                         //       提示数据无效
        DelayGotoPop(POP1_ICO_HOMING, 2000);                             //       提示2秒后跳转新弹窗
        HandleLevelingCalibrate(*(DGUS_VP_Variable *)nullptr, nullptr);  //       执行调平校准
      }
    }
  }
}

// 自动调平校准按钮
void DGUSScreenHandler::HandleLevelingCalibrate(DGUS_VP_Variable &, void *) {
  // GotoPopup(POP0_ICO_WAITING);
  if (delayPop != POP1_ICO_HOMING) {
    GotoPopup(POP1_ICO_HOMING);
  }
  if (ExtUI::isMachineHomed())
    ExtUI::injectCommands(F("G29N"));
  else {
    ExtUI::injectCommands(F("G29N"));
  }
}

// 更新自动调平偏移
void DGUSScreenHandler::UpdateProbeOffsetZ(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateProbeOffsetZ");
  UpdateValToLCD<2, int16_t>(ExtUI::getZOffset_mm, var);
}

// 设置自动调平偏移
void DGUSScreenHandler::HandleProbeOffsetZSet(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleProbeOffsetZSet");
  auto adjustOffsetZ = [](float newValue) {
    setZOffset_mm(newValue);
  };
  SetValFromLCD<2, int16_t>(adjustOffsetZ, var, val_ptr);
}

// 更新调平淡化高度
void DGUSScreenHandler::UpdateFadeHeight(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateFadeHeight");
  UpdateValToLCD<1, uint16_t>([]() { return planner.z_fade_height; }, var);
}

// 设置调平淡化高度
void DGUSScreenHandler::HandleFadeHeightSet(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleFadeHeightSet");
  SetValFromLCD<1, uint16_t>(planner.set_z_fade_height, var, val_ptr);
}
  #endif  // HAS_LEVELING

// 更新挤出量
void DGUSScreenHandler::UpdateFlowrate(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateFlowrate");
  UpdateValToLCD<ExtUI::extruder_t>(ExtUI::getFlow_percent, var);
}

// 设置挤出量
void DGUSScreenHandler::HandleFlowrateSet(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleFlowrateSet");
  SetValFromLCD<ExtUI::extruder_t>(ExtUI::setFlow_percent, var, val_ptr);
}

// 更新轴坐标
void DGUSScreenHandler::UpdateAxisPos(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateAxisPos");
  UpdateValToLCD<ExtUI::axis_t, 2, int32_t>(ExtUI::getAxisPosition_mm, var);
}

// 更新轴单位
void DGUSScreenHandler::UpdateAxisUnit(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateAxisUnit");
  UpdateValToLCD([]() { return axisMoveUnit; }, var);
}

// 设置轴单位
void DGUSScreenHandler::HandleAxisUnitSet(DGUS_VP_Variable &var, void *var_ptr) {
  DEBUG_ECHOLNPGM("HandleAxisUnitSet");
  SetValFromLCD([](uint16_t axisUnit) { axisMoveUnit = axisUnit; }, var, var_ptr);
  skipVP = var.VP;
}

// 更新步进值
void DGUSScreenHandler::UpdateStepPerMM(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateStepPerMM");
  if (var.VP == VP_Z_STEP_PER_MM) {
    UpdateValToLCD<ExtUI::axis_t, 3, int32_t>(ExtUI::getAxisSteps_per_mm, var);
  } else {
    UpdateValToLCD<ExtUI::axis_t, 4, int32_t>(ExtUI::getAxisSteps_per_mm, var);
    UpdateValToLCD<ExtUI::extruder_t, 4, int32_t>(ExtUI::getAxisSteps_per_mm, var);
  }
}

// 设置步进值
void DGUSScreenHandler::HandleStepPerMMSet(DGUS_VP_Variable &var, void *var_ptr) {
  DEBUG_ECHOLNPGM("HandleStepPerMMSet");
  if (var.VP == VP_Z_STEP_PER_MM_Set) {
    SetValFromLCD<ExtUI::axis_t, 3, int32_t>(ExtUI::setAxisSteps_per_mm, var, var_ptr);
  } else {
    SetValFromLCD<ExtUI::axis_t, 4, int32_t>(ExtUI::setAxisSteps_per_mm, var, var_ptr);
    SetValFromLCD<ExtUI::extruder_t, 4, int32_t>(ExtUI::setAxisSteps_per_mm, var, var_ptr);
  }
}

// 更新风扇参数
void DGUSScreenHandler::UpdateFan(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateFan");
  if (var.VP == VP_FanT_Percentage) {
    auto getAutoFanSpeed = []() {
      return TERN(ADJUST_EXTRUDER_AUTO_FAN, ui8_to_percent(thermalManager.extruder_auto_fan_speed), 100);
    };
    UpdateValToLCD(getAutoFanSpeed, var);
  } else {
    UpdateValToLCD<ExtUI::fan_t>(ExtUI::getTargetFan_percent, var);
  }
}

// 设置风扇参数
void DGUSScreenHandler::HandleFanSet(DGUS_VP_Variable &var, void *var_ptr) {
  DEBUG_ECHOLNPGM("HandleFanSet");
  if (var.VP == VP_FanT_Percentage_Set) {
    auto getAutoFanSpeed = [](uint16_t percent) {
      TERN_(ADJUST_EXTRUDER_AUTO_FAN,
            thermalManager.extruder_auto_fan_speed = map(constrain(percent, 0, 100), 0, 100, 0, 255));
    };
    SetValFromLCD(getAutoFanSpeed, var, var_ptr);
  } else {
    SetValFromLCD<ExtUI::fan_t>(ExtUI::setTargetFan_percent, var, var_ptr);
  }
}

// 开关图标更新
void DGUSScreenHandler::UpdateSwitchIcon(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateSwitchIcon");
  auto getSwitchIcon = [&var]() {
    uint16_t index = ICO_OFF_DISABLE;
    switch (var.VP) {  // clang-format off
    #if ENABLED(CASE_LIGHT_ENABLE)
      case VP_CaseLight: index = ScreenSetting.isCaseLightOn ? ICO_ON : ICO_OFF; break;
    #else
      case VP_CaseLight: index = ScreenSetting.isCaseLightOn ? ICO_ON_DISABLE : ICO_OFF_DISABLE; break;
    #endif
      case VP_AutoPowerOff: index = ScreenSetting.isAutoPowerOff ? ICO_ON : ICO_OFF; break;
      case VP_AutoBackMain: index = ScreenSetting.isAutoBackMain ? ICO_ON : ICO_OFF; break;
      default: break;
    }  // clang-format on
    return index;
  };
  UpdateValToLCD(getSwitchIcon, var);
}

// 设置开关按钮
void DGUSScreenHandler::HandleSwitchAction(DGUS_VP_Variable &var, void *vat_ptr) {
  DEBUG_ECHOLNPGM("HandleSwitchAction");
  DGUS_VP_Variable temp_var;
  switch (var.match) {  // clang-format off
  #if ENABLED(CASE_LIGHT_ENABLE)
    case CMD_CASE_LIGHT: ScreenSetting.isCaseLightOn ^= 1; break;
  #endif
    case CMD_AUTO_BACK_MAIN: ScreenSetting.isAutoBackMain ^= 1; break;
    case CMD_AUTO_POWER_OFF: ScreenSetting.isAutoPowerOff ^= 1; break;
    default: break;
  }  // clang-format on
  ForceCompleteUpdate();
}

// 更新BabyStep
void DGUSScreenHandler::UpdateBabyStep(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdateBabyStep");
  UpdateValToLCD<ExtUI::axis_t, 2, int16_t>(ExtUI::getBabyStepAxisTotal_mm, var);
}

  #if ENABLED(CREATBOT_WIFI_SUPPORT)
// 更新WIFI信息
void DGUSScreenHandler::UpdateWiFiInfo(char *SSID, char *KEY) {
  DEBUG_ECHOLNPGM("UpdateWiFiInfo");
  char _ssid[WIFI_SSID_SIZE]   = {0};
  char _key[WIFI_KEY_SIZE]     = {0};
  char _keyShow[WIFI_KEY_SIZE] = {0};
  dgusdisplay.WriteString(VP_WIFI_KEY_MASK_ADDR, _keyShow, WIFI_TEXT_LEN);
  dgusdisplay.WriteString(VP_WIFI_SSID_SET_ADDR, _ssid, WIFI_TEXT_LEN);
  dgusdisplay.WriteString(VP_WIFI_KEY_ADDR, _key, WIFI_TEXT_LEN);
  strcpy(_ssid, SSID);
  strcpy(_key, KEY);
  memset(_keyShow, '*', strlen(_key));
  dgusdisplay.WriteString(VP_WIFI_SSID_SET_ADDR, _ssid, strlen(_ssid));
  dgusdisplay.WriteString(VP_WIFI_KEY_ADDR, _key, strlen(_key));
  if (strlen(_key) > WIFI_TEXT_LEN)
    dgusdisplay.WriteString(VP_WIFI_KEY_MASK_ADDR, _keyShow, WIFI_TEXT_LEN);
  else
    dgusdisplay.WriteString(VP_WIFI_KEY_MASK_ADDR, _keyShow, strlen(_key));
}

// 更新UUID信息
void DGUSScreenHandler::UpdateWiFiUUID(char *UUID) {
  DEBUG_ECHOLNPGM("UpdateWiFiUUID");
  char _uuid[WIFI_UUID_SIZE] = {0};
  strcpy(_uuid, UUID);
  dgusdisplay.WriteString(VP_WIFI_UUID_ADDR, _uuid, strlen(_uuid));

    #if ENABLED(CreatBot_DGUS_V6)
  char _qr[LCD_QR_LEN] = {0};
  snprintf_P(_qr, LCD_QR_LEN, PSTR("http://creatbot-%s/update?imei=%s"), &_uuid[10], _uuid);
  dgusdisplay.WriteString(VP_WIFI_QR_ADDR, _qr, LCD_QR_LEN);
    #endif
}

// 更新WIFI信号图标
void DGUSScreenHandler::UpdateWiFiHomeIcon() {
    #if ENABLED(CreatBot_DGUS_V5)
  dgusdisplay.UpdateWiFiIcon(VP_Home_Icon, myWifi.getWifiState());
    #elif ENABLED(CreatBot_DGUS_V6)
  dgusdisplay.UpdateWiFiIcon(VP_Home_WIFI_Icon, myWifi.getWifiState());
  dgusdisplay.UpdateWiFiIcon(VP_Home_CLOUD_Icon, myWifi.getCloudState());
  dgusdisplay.UpdateWiFiIcon(VP_Home_CAMERA_Icon, myWifi.getCameraState());
    #endif
}

// 更新WIFI状态图标
void DGUSScreenHandler::UpdateWiFiState() {
  uint16_t wifiState = 0;
  switch (myWifi.getWifiState()) {
    case WifiNoConnect: wifiState = 0; break;
    case WifiConnecting: wifiState = 1; break;
    case WifiRssi1:
    case WifiRssi2:
    case WifiRssi3:
    case WifiRssi4: wifiState = 2; break;
    case WifiSsidNoFind: wifiState = 4; break;
    case WifiKeyErr: wifiState = 3; break;
    default: wifiState = 0;
  }
  dgusdisplay.UpdateWiFiIcon(VP_WIFI_STATE_INDEX_ADDR, wifiState);
}

// 设置WIFI信息
void DGUSScreenHandler::HandleWiFiInfoSet(DGUS_VP_Variable &var, void *var_ptr) {
  DEBUG_ECHOLNPGM("HandleWiFiInfoSet");
  if (var.VP == VP_WIFI_SSID_SET_ADDR) {
    char *ssid = (char *)var_ptr;
    for (int i = 0; i < WIFI_SSID_SIZE; i++) {
      if (*(ssid + i) == 0xFF) {
        *(ssid + i) = 0x00;
        break;
      }
    }
    memset(&myWifi.connSSID, 0, WIFI_SSID_SIZE);
    strcpy(myWifi.connSSID, ssid);
  } else if (var.VP == VP_WIFI_KEY_ADDR) {
    char *key = (char *)var_ptr;
    for (int i = 0; i < WIFI_KEY_SIZE; i++) {
      if (*(key + i) == 0xFF) {
        *(key + i) = 0x00;
        break;
      }
    }
    memset(&myWifi.connKEY, 0, WIFI_KEY_SIZE);
    strcpy(myWifi.connKEY, key);
    UpdateWiFiInfo(myWifi.connSSID, myWifi.connKEY);
  }
}

// 连接WIFI
void DGUSScreenHandler::HandleConnectWiFi(DGUS_VP_Variable &var, void *var_ptr) {
  DEBUG_ECHOLNPGM("HandleConnectWiFi");
  myWifi.actionJoinAP();
}

// 获取文件列表信息
void DGUSScreenHandler::UpdateFilelistStatus(char *status) {
  bool lastLev  = !filelist.isAtRootDir();
  bool lastPage = false;
  bool nextPage = false;

  if (page_num > 1) {
    lastPage = nextPage = true;
    if (file_page[0] == 1) lastPage = false;
    if (file_page[0] == page_num) nextPage = false;
  }
  sprintf_P(status, "%d,%d,%d,", (int)lastLev, (int)lastPage, (int)nextPage);
}

// 远程获取文件名信息
bool DGUSScreenHandler::HandleGetFileNameInfo(uint8_t target_line, uint8_t *isdir, char *str) {
  DEBUG_ECHOLNPGM("HandleGetFileNameInfo");
  if (target_line >= LCD_FILE_PER_PAGE) return 0;
  char     tmpfilename[LCD_FILENAME_LENGTH] = "";
  uint16_t fileIndex                        = (file_page[0] - 1) * LCD_FILE_PER_PAGE + target_line;

  if (filelist.seek(fileIndex)) {
    *isdir                   = filelist.isDir() ? 2 : 1;
    const char *filename     = filelist.filename();
    const char *longFilename = filelist.longFilename();
    #if ENABLED(UTF_FILENAME_SUPPORT, UTF_IS_UTF16)
    if (longFilename[0] || longFilename[1]) {
      memcpy(str, longFilename, LONG_FILENAME_LENGTH);
      return true;
    } else
    #endif
    {
      strcpy(str, filename);
      return false;
    }
  } else {
    *isdir = 0;
    return false;
  }
}

// 远程浏览文件操作
void DGUSScreenHandler::HandleFileViewAction(uint8_t action) {
  switch (action) {
    case 1: SDCardIndexNext(); break;
    case 2: SDCardIndexLast(); break;
    case 3:
      filelist.upDir();
      SDCardIndexUP();
      break;
    default: break;
  }
}

// 远程文件操作
void DGUSScreenHandler::HandleFileItemAction(uint8_t target_line) {
  DEBUG_ECHOLNPGM("HandleFileItemAction");
  uint16_t fileIndex = (file_page[0] - 1) * LCD_FILE_PER_PAGE + target_line;
  if (filelist.seek(fileIndex)) {
    if (filelist.isDir()) {
      filelist.changeDir(filelist.shortFilename());  // 打开下级目录
      SDCardIndexCD();
      myWifi.actionSynFilesInfo();
    } else {
      if (filelist.seek(fileIndex)) {
    #if HAS_FILAMENT_SENSOR
        if (ExtUI::getFilamentRunoutState()) ExtUI::setFilamentRunoutState(false);  // 重置耗材检测触发状态
    #endif
        ExtUI::printFile(filelist.shortFilename());
      }
    }
  }
}
  #endif

// 更新预热参数
void DGUSScreenHandler::UpdatePreHeat(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("UpdatePreHeat");
  // auto getPreHeatValue = [](const celsius_t *tempVar) { return *tempVar; };
  // UpdateValToLCD<celsius_t *>(getPreHeatValue, var);

  auto getPreHeatValue = [&var]() {
    celsius_t value = 0;
    switch (var.VP) {  // clang-format off
      case VP_PreHeat_Fan:        break;
      case VP_PreHeat_T_E0:       value = ScreenSetting.preheat_T_temp; break;
      case VP_PreHeat_T_E1:       break;
      case VP_PreHeat_T_E2:       break;
      case VP_PreHeat_T_Bed:      value = ScreenSetting.preheat_B_temp; break;
      case VP_PreHeat_T_Chamber:  value = ScreenSetting.preheat_C_temp; break;
      default: break;
    }  // clang-format on
    return value;
  };
  UpdateValToLCD(getPreHeatValue, var);
}

// 设置预热参数
void DGUSScreenHandler::HandlePreheadSet(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandlePreheadSet");
  // auto setPreHeatValue = [](const_float_t newValue, celsius_t *tempVar) {  //
  //   *tempVar = newValue;
  // };
  // SetValFromLCD<celsius_t *>(setPreHeatValue, var, val_ptr);

  auto setPreHeatValue = [&var](celsius_t newValue) {
    switch (var.VP) {  // clang-format off
      case VP_PreHeat_Fan_Set:        break;
      case VP_PreHeat_T_E0_Set:       ScreenSetting.preheat_T_temp = newValue; break;
      case VP_PreHeat_T_E1_Set:       break;
      case VP_PreHeat_T_E2_Set:       break;
      case VP_PreHeat_T_Bed_Set:      ScreenSetting.preheat_B_temp = newValue; break;
      case VP_PreHeat_T_Chamber_Set:  ScreenSetting.preheat_C_temp = newValue; break;
      default: break;
    }  // clang-format on
  };
  SetValFromLCD(setPreHeatValue, var, val_ptr);
}

// 跳转到指定页面
void DGUSScreenHandler::GotoScreen(DGUSLCD_Screens screen) {
  if (dgusdisplay.needBoot()) return;
  if (delayScreen) delayScreen = (DGUSLCD_Screens)0;
  dgusdisplay.RequestScreen(screen);
  ChangeComplete = false;
  UpdateNewScreen(screen);
}

// 跳转到指定页面, 指定跳转延时
void DGUSScreenHandler::DelayGotoScreen(DGUSLCD_Screens screen, millis_t delay) {
  delayScreen   = screen;
  delayJumpTime = millis() + delay;
}

// 延时弹窗
void DGUSScreenHandler::DelayGotoPop(uint16_t Pop, millis_t delay) {
  delayPop      = Pop;
  delayJumpTime = millis() + delay;
}

// 跳转到默认页
void DGUSScreenHandler::GotoDefault() {
  ui.reset_status();
  GotoScreen(DGUSLCD_GetDefaultScreen());
}

// 跳转到弹窗页面
void DGUSScreenHandler::GotoPopup(uint16_t newPopIndex) {
  while (!ChangeComplete && dgusdisplay.isInitialized()) idle();
  ScreenComplete = true;
  dgusdisplay.WriteVariable(VP_POP, popIndex = newPopIndex);
  switch (popIndex) {  // clang-format on
    case POP0_ICO_SETTING_RESTORING:
    case POP0_ICO_SETTING_SAVING:
    case POP0_ICO_WAITING:
    case POP0_ICO_PARKING:
    case POP0_ICO_RESUMING:
    case POP0_ICO_INVALID_LEVLEL_DATA: GotoScreen(DGUSLCD_SCREEN_POP_0); break;
    case POP1_ICO_EXTRUDE_FILAMENT:
    case POP1_ICO_RECRACT_FILAMENT:
    case POP1_ICO_LOAD_FILAMENT:
    case POP1_ICO_UNLOAD_FILAMENT:
    case POP1_ICO_MOVING:
    case POP1_ICO_HOMING:
    case POP1_ICO_PROBING:
    case POP1_ICO_PROBER_UNAVAILABLE:
    case POP1_ICO_TAR_TEMP_LOW:
    case POP1_ICO_CUR_TEMP_LOW:
    case POP1_ICO_HEATER_TIMEOUT: GotoScreen(DGUSLCD_SCREEN_POP_1); break;
    case POP2_ICO_CHANGE_CONFIRM:
    case POP2_ICO_RESET_CONFIRM:
    case POP2_ICO_SKIP_CONFIRM:
    case POP2_ICO_STOP_CONFIRM:
    case POP2_ICO_INSERT_CONTIUNE:
    case POP2_ICO_LOAD_OPTION: GotoScreen(DGUSLCD_SCREEN_POP_2); break;
  }  // clang-format on
}

// 更新新的页面
void DGUSScreenHandler::UpdateNewScreen(DGUSLCD_Screens newscreen) {
  DEBUG_ECHOLNPGM("SetNewScreen: ", newscreen);
  if (!DGUSLCD_IsPopScreen(curScreen)) preScreen = curScreen;
  if (!DGUSLCD_IsPopScreen(newscreen)) popIndex = 0;
  curScreen = newscreen;
  skipVP    = 0;
  ForceCompleteUpdate();
}

// 更新初始化数据 (非自动更新)
void DGUSScreenHandler::UpdateScreenInitData() {
  DEBUG_ECHOPGM("UpdateScreenInitData");
  ScreenComplete = true;
  // ui.reset_status();  // TODO need reset here?
  // TODO 更新 状态信息
  // TODO 更新 WIFI SSID 信息

  #if ENABLED(CreatBot_DGUS_V6)
  DGUSLCD_PresetProgressBar(VP_PrintProgess);  // 更新预设进度条
  #endif
}

// 更新当前页面数据
void DGUSScreenHandler::UpdateScreenVPData() {
  DEBUG_ECHOPGM(" UpdateScreenVPData Screen: ", curScreen);

  const uint16_t *VPList = DGUSLCD_FindScreenVPMapList(curScreen);
  #if ENABLED(CREATBOT_WIFI_SUPPORT)
  if (IsDefaultScreen) UpdateWiFiHomeIcon();
  if (curScreen == DGUSLCD_SCREEN_CONNECT_WIFI_12) UpdateWiFiState();
  #endif
  if (!VPList) {
    DEBUG_ECHOLNPGM(" NO SCREEN FOR: ", curScreen);
    ScreenComplete = true;
    return;  // nothing to do, likely a bug or boring screen.
  }
  DEBUG_EOL();

  // Round-robin updating of all VPs.
  VPList += updatePtr;

  bool sent_one = false;
  do {
    uint16_t VP = pgm_read_word(VPList);
    DEBUG_ECHOPGM(" VP: ", VP, " > ");
    if (!VP) {
      updatePtr = 0;
      DEBUG_ECHOLNPGM(" UpdateScreenVPData done");
      ScreenComplete = true;
      return;  // Screen completed.
    }

    if (VP == skipVP) {
      skipVP = 0;
      DEBUG_ECHOLNPGM(" UpdateScreenVPData ignore");
      continue;
    }

    DGUS_VP_Variable rcpy;
    if (populate_VPVar(VP, 0, &rcpy)) {
      uint8_t expected_tx = 6 + rcpy.size;  // expected overhead is 6 bytes + payload.
      // Send the VP to the display, but try to avoid overrunning the Tx Buffer.
      // But send at least one VP, to avoid getting stalled.
      if (rcpy.tx_handler && (!sent_one || expected_tx <= dgusdisplay.GetFreeTxBuffer())) {
        // DEBUG_ECHOPGM(" calling handler for ", rcpy.VP);
        sent_one = true;
        rcpy.tx_handler(rcpy);
      } else {
        if (rcpy.tx_handler) {
          // auto x = dgusdisplay.GetFreeTxBuffer();
          // DEBUG_ECHOLNPGM(" tx almost full: ", x);
          DEBUG_ECHOLNPGM(" UpdateScreenVPData break");
          ScreenComplete = false;
          return;  // please call again!
        } else {
          DEBUG_ECHOLNPGM(" UpdateScreenVPData nullptr");
          continue;
        }
      }
    }
  } while (++updatePtr, ++VPList, true);
}

// 是否是默认页面
bool DGUSScreenHandler::IsDefaultScreen() {
  return WITHIN(curScreen, DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT, DGUSLCD_SCREEN_DEFAULT_ONLINE);
}

// 是否是挂起页面
bool DGUSScreenHandler::IsHoldOnScreen() {
  if (curScreen == DGUSLCD_SCREEN_POP_0 || curScreen == DGUSLCD_SCREEN_POP_1 || curScreen == DGUSLCD_SCREEN_POP_2 ||
      curScreen == DGUSLCD_SCREEN_UNFINISH_CHOOSE || curScreen == DGUSLCD_SCREEN_NOZZLE_HEATING ||
      curScreen == DGUSLCD_SCREEN_SHUTDOWN_HOTTEMP || curScreen == DGUSLCD_SCREEN_REG) {
    return true;
  } else
    return false;
}

void DGUSScreenHandler::init() {
  creatbotInit();
  dgusdisplay.InitDisplay();
  return_default_ms = millis() + LCD_PAGE_TIMEOUT;

  TERN_(SHOW_BOOTSCREEN, dgusdisplay.RequestScreen(DGUSLCD_SCREEN_BOOT));
}

bool DGUSScreenHandler::loop() {
  if (!ScreenSetting.isAutoPowerOff) powerReset();
  dgusdisplay.loop();
  if (!dgusdisplay.isInitialized()) return false;

  const millis_t ms = millis();  // 当前时间

  // 手动移轴
  creatbotLoop(axisMoveHook);

  // 更新回默认页的时间
  if (!ScreenSetting.isAutoBackMain || IsInTouch() || IsHoldOnScreen()) {
    return_default_ms = ms + LCD_PAGE_TIMEOUT;
  }

  // 返回默认页
  if (!IsDefaultScreen() && ELAPSED(ms, return_default_ms)) {
    return_default_ms = ms + LCD_PAGE_TIMEOUT;
    GotoDefault();
  }

  // 延时跳转页面
  if (delayScreen && ELAPSED(ms, delayJumpTime)) {
    if (delayScreen == DGUSLCD_SCREEN_DEFAULT_DELAY) {
      GotoDefault();
    } else {
      GotoScreen(delayScreen);
    }
    delayScreen = (DGUSLCD_Screens)0;
  }

  // 延时弹窗
  if (delayPop && ELAPSED(ms, delayJumpTime)) {
    if (delayPop) {
      GotoPopup(delayPop);
      delayPop = (uint16_t)0;
    }
  }

  // 移轴,送丝完成
  if (!ExtUI::isMoving() && !planner.cleaning_buffer_counter) {
    if (popIndex == POP1_ICO_MOVING) {  // 移轴弹窗
      GotoScreen(DGUSLCD_SCREEN_MOVE);
    } else if (popIndex == POP1_ICO_EXTRUDE_FILAMENT ||  // 送丝弹窗
               popIndex == POP1_ICO_RECRACT_FILAMENT ||  // 退丝弹窗
               popIndex == POP1_ICO_LOAD_FILAMENT ||     // 加载弹窗
               popIndex == POP1_ICO_UNLOAD_FILAMENT      // 卸载弹窗
    ) {
      GotoScreen(DGUSLCD_SCREEN_FILAMENT);
    }
  }

  // 更新屏幕数据
  if (!IsScreenComplete() || ELAPSED(ms, next_event_ms)) {
    next_event_ms = ms + DGUS_UPDATE_INTERVAL_MS;  // 屏幕更新数据周期时间500ms
    UpdateScreenVPData();                          // 更新屏幕数据
  }

  return IsScreenComplete();
}

// 关机状态显示
void DGUSScreenHandler::PrinterPoweroffHook() {
  #if ENABLED(POWER_OFF_WAIT_FOR_COOLDOWN)
  HOTEND_LOOP() {
    if (thermalManager.degHotend(e) >= (AUTO_POWER_E_TEMP)) {
      GotoScreen(DGUSLCD_SCREEN_SHUTDOWN_HOTTEMP);
      break;
    }
  }
  #endif
}

// 异常状态显示
void DGUSScreenHandler::PrinterKilledHook(FSTR_P const error, FSTR_P const component) {
  char killMsg[strlen_P(FTOP(error)) + strlen_P(FTOP(component)) + 2] = {0};
  strcpy_P(killMsg, FTOP(error));
  strcat_P(killMsg, PSTR(" "));
  strcat_P(killMsg, FTOP(component));

  GotoDefault();
  DGUSLCD_UpdateStatusMsg(killMsg);

  // TODO 禁用页面触控
  // KILLED触发之后会进入循环等待重启,页面数据不更新
  // 此时应强制完成更新,显示错误温度和信息
  while (!IsScreenComplete()) UpdateScreenVPData();

  // 等待用户按下电源按钮
  while (TERN0(HAS_KILL, !kill_state())) hal.watchdog_refresh();
}

void DGUSScreenHandler::PrintTimerStartHook() {
  GotoDefault();
  while (!IsScreenComplete()) UpdateScreenVPData();
}

void DGUSScreenHandler::PrintTimerPauseHook() {}

void DGUSScreenHandler::PrintTimerStopHook() { DelayGotoScreen(DGUSLCD_SCREEN_DEFAULT_DELAY, 10); }

void DGUSScreenHandler::PrintDoneHook() { GotoDefault(); }

void DGUSScreenHandler::FactoryResetHook() {
  ScreenSetting.preheat_T_temp = PREHEAT_1_TEMP_HOTEND;
  ScreenSetting.preheat_B_temp = PREHEAT_1_TEMP_BED;
  ScreenSetting.preheat_C_temp = PREHEAT_1_TEMP_CHAMBER;
  ScreenSetting.isCaseLightOn  = TERN1(CASE_LIGHT_ENABLE, CASE_LIGHT_DEFAULT_ON ?: 0);
  ScreenSetting.isAutoPowerOff = false;  // 默认关闭自动关机功能
  ScreenSetting.isAutoBackMain = true;   // 默认开启自动返回主页

  if (popIndex == POP0_ICO_SETTING_RESTORING) {
    DelayGotoScreen(preScreen, 500);
  }
  settings.save();
}

void DGUSScreenHandler::StoreSettingsHook(char *buff) {
  static_assert(sizeof(ScreenSetting) <= eeprom_data_size);
  memcpy(buff, &ScreenSetting, sizeof(ScreenSetting));
}

void DGUSScreenHandler::LoadSettingsHook(const char *buff) {
  static_assert(sizeof(ScreenSetting) <= eeprom_data_size);
  memcpy(&ScreenSetting, buff, sizeof(ScreenSetting));
}

void DGUSScreenHandler::SettingsStoredHook() {
  if (popIndex == POP0_ICO_SETTING_SAVING) {
    DelayGotoScreen(preScreen, 500);
  }
}

void DGUSScreenHandler::HomingStartHook() {
  if (curScreen == DGUSLCD_SCREEN_MOVE) {
    GotoPopup(POP1_ICO_HOMING);
  }
}

void DGUSScreenHandler::HomingDoneHook() {
  if (popIndex == POP1_ICO_HOMING) {
    if (preScreen == DGUSLCD_SCREEN_LEVELING_SETTING_ON)
      TERN_(HAS_LEVELING, HandleSettingLeveling(*(DGUS_VP_Variable *)nullptr, nullptr));
    else
      GotoScreen(preScreen);
  }
}

void DGUSScreenHandler::FilamentRunoutHook() { GotoPopup(POP0_ICO_WAITING); }

  #if ENABLED(POWER_LOSS_RECOVERY)
void DGUSScreenHandler::PowerLossResumeHook() { GotoScreen(DGUSLCD_SCREEN_UNFINISH_CHOOSE); }
  #endif

  #if HAS_MESH
void DGUSScreenHandler::LevelingStartHook() {
  if (curScreen == DGUSLCD_SCREEN_LEVELING_SETTING_OFF ||  //
      curScreen == DGUSLCD_SCREEN_LEVELING_SETTING_ON ||   //
      popIndex == POP0_ICO_WAITING ||                      //
      popIndex == POP1_ICO_HOMING) {
    GotoPopup(POP1_ICO_PROBING);
  }
}

void DGUSScreenHandler::LevelingDoneHook() {
  if (popIndex == POP1_ICO_PROBING) {
    HandleSettingLeveling(*(DGUS_VP_Variable *)nullptr, nullptr);
  }
  if (ExtUI::getLevelingIsValid()) {
    settings.save();
  }
}
  #endif  // HAS_MESH

void DGUSScreenHandler::StatusChangedHook(const char *const msg) { DGUSLCD_UpdateStatusMsg(msg); }

void DGUSScreenHandler::UserConfirmRequiredHook(const char *const msg) {
  // MSG_PAUSE_PRINT_SYNCING              //M125                                      //状态等待      缓冲区执行中 ...
  // MSG_PAUSE_PRINT_PARKING              //M125                                      //状态等待      喷头停靠中 ...
  // MSG_ADVANCED_PAUSE_WAITING           //M125                                      //操作等待      按下继续
  // MSG_FILAMENT_CHANGE_INIT             //M600                                      //状态等待      更换耗材 ...
  // MSG_FILAMENT_CHANGE_INSERT           //M600 load_filament                        //操作等待      插入耗材,按下继续
  // MSG_FILAMENT_CHANGE_LOAD             //M701 load_filament                        //状态等待/执行 耗材装载中 ...
  // MSG_FILAMENT_CHANGE_DELAY            //M702 unload_filament                      //状态等待/执行 耗材冷却中 ...
  // MSG_FILAMENT_CHANGE_UNLOAD           //M702 unload_filament                      //状态等待/执行 耗材卸载中 ...
  // MSG_FILAMENT_CHANGE_PURGE            //load_filament                             //状态执行      出丝测试中 ...
  // MSG_FILAMENT_CHANGE_CONT_PURGE       //load_filament                             //操作等待      出丝完成,继续打印
  // MSG_FILAMENT_CHANGE_OPTION_HEADER    //load_filament                             //操作等待      挤出更多|继续打印
  // MSG_FILAMENT_CHANGE_RESUME           //resume_print                              //状态执行      打印恢复中 ...
  // MSG_FILAMENT_CHANGE_HEAT             //wait_for_confirmation (M125,M600)         //操作等待 喷头已经冷却,按下加热
  // MSG_FILAMENT_CHANGE_HEATING          //wait_for_confirmation (un)load_filament   //状态等待      加热中 ...
  // MSG_REHEATDONE                       //wait_for_confirmation                     //状态指示      加热完成.
  // MSG_NOZZLE_PARKED                    //wait_for_confirmation                     //操作等待 喷头停靠完成,按下继续
  // ""                                   //M701 load_filament                        //状态指示      加载耗材完成
  // ""                                   //M702 unload_filament                      //状态指示      卸载耗材完成
  // ""                                   //resume_print                              //状态指示      恢复打印完成
  // MSG_USERWAIT                         //等同于 MSG_ADVANCED_PAUSE_WAITING
  // MSG_HEATER_TIMEOUT                   //等同于 MSG_FILAMENT_CHANGE_HEAT

  #if ENABLED(MARLIN_DEV_MODE)
  DGUSLCD_UpdateStatusMsg(msg);  // 更新状态字符串
  #endif

  if (!strcmp_P(msg, GET_TEXT(MSG_ADVANCED_PAUSE_WAITING)) ||         //
      !strcmp_P(msg, GET_TEXT(MSG_USERWAIT))) {                       // 操作等待: 按下继续
    GotoDefault();                                                    //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_NOZZLE_PARKED))) {           // 操作等待: 喷头停靠完成,按下继续
    NOOP;                                                             //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_HEAT)) ||    //
             !strcmp_P(msg, GET_TEXT(MSG_HEATER_TIMEOUT))) {          // 操作等待: 喷头已经冷却,按下加热
    GotoPopup(POP1_ICO_HEATER_TIMEOUT);                               //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_INSERT))) {  // 操作等待: 插入耗材,按下继续
    GotoPopup(POP2_ICO_INSERT_CONTIUNE);                              //
  }

  else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_CONT_PURGE))) {       // 操作等待: 出丝完成,继续打印
    GotoPopup(POP1_ICO_EXTRUDE_FILAMENT);                                    //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_OPTION_HEADER))) {  // 操作等待: 挤出更多|继续打印
    GotoPopup(POP2_ICO_LOAD_OPTION);                                         //
  }

  else if (!strcmp_P(msg, GET_TEXT(MSG_PAUSE_PRINT_SYNCING))) {        // 状态等待      缓冲区执行中 ...
    GotoPopup(POP0_ICO_WAITING);                                       //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_PAUSE_PRINT_PARKING))) {      // 状态等待      喷头停靠中 ...
    GotoPopup(POP0_ICO_PARKING);                                       //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_INIT))) {     // 状态等待      更换耗材 ...
    GotoPopup(POP0_ICO_WAITING);                                       //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_LOAD))) {     // 状态等待/执行 耗材装载中 ...
    GotoPopup(POP1_ICO_LOAD_FILAMENT);                                 //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_DELAY))) {    // 状态等待/执行 耗材冷却中 ...
    GotoPopup(POP0_ICO_WAITING);                                       //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_UNLOAD))) {   // 状态等待/执行 耗材卸载中 ...
    GotoPopup(POP1_ICO_UNLOAD_FILAMENT);                               //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_PURGE))) {    // 状态执行      出丝测试中 ...
    GotoPopup(POP1_ICO_EXTRUDE_FILAMENT);                              //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_RESUME))) {   // 状态执行      打印恢复中 ...
    GotoPopup(POP0_ICO_RESUMING);                                      //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_HEATING))) {  // 状态等待      加热中 ...
    GotoScreen(DGUSLCD_SCREEN_NOZZLE_HEATING);                         //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_REHEATDONE))) {               // 状态指示      加热完成.
    NOOP;                                                              //
  } else if (!strcmp_P(msg, "")) {                                     // 状态指示      操作完成.
    NOOP;                                                              //
  } else {                                                             // 其他未知消息
    ;                                                                  //
  }
}

#endif  // DGUS_LCD_UI_CREATBOT
