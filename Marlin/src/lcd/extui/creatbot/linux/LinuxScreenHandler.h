/**
 *  LinuxScreenHandler.h
 *
 *  Created on: 2023/07/27
 *      Author: CreatBot LYN
 *
 *  handler for linux lcd.
 */

#pragma once

#include "../../../../inc/MarlinConfigPre.h"
#include "../../ui_api.h"
using namespace ExtUI;

class LinuxScreenHandler {
 public:
  LinuxScreenHandler() = default;

  static void init();
  static void loop();

  static void PrinterPoweroffHook();
  static void PrinterKilledHook(FSTR_P const error, FSTR_P const component);
  static void PrintTimerStartHook();
  static void PrintTimerPauseHook();
  static void PrintTimerStopHook();
  static void PrintDoneHook();
  static void FactoryResetHook();
  static void StoreSettingsHook(char *);
  static void LoadSettingsHook(const char *);
  static void SettingsStoredHook();
  static void HomingStartHook();
  static void HomingDoneHook();
  static void FilamentRunoutHook();
  static void PowerOffSafetyHook();
#if HAS_LEVELING
  static void LevelingStartHook();
  static void LevelingDoneHook();
#endif
#if ENABLED(POWER_LOSS_RECOVERY)
  static void PowerLossResumeHook();
#endif
  static void StatusChangedHook(const char *const msg);
  static void UserConfirmRequiredHook(const char *const msg);
  static void axisMoveCallback() {};

#if ENABLED(SDSUPPORT)
  static void SDCardInserted();  // U盘插入
  static void SDCardError();     // U盘异常
  static void SDCardRemoved();   // U盘移除
#endif
};

extern LinuxScreenHandler ScreenHandler;