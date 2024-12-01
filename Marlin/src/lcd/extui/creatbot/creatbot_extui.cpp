/**
 *  creatbot_extui.cpp
 *
 *  Created on: 2022/05/13
 *      Author: CreatBot LYN
 *
 *  implement for ui_api
 */

#include "../../../inc/MarlinConfigPre.h"

#if CREATBOT_LCD

  #include "../ui_api.h"
  #if ENABLED(DGUS_LCD_UI_CREATBOT)
    #include "dgus/DGUSScreenHandler.h"
  #elif ENABLED(CREATBOT_LINUX_LCD)
    #include "linux/LinuxScreenHandler.h"
  #endif

namespace ExtUI {
  void onStartup() { ScreenHandler.init(); }
  void onIdle() { ScreenHandler.loop(); }
  void onMediaInserted() { TERN_(SDSUPPORT, ScreenHandler.SDCardInserted()); }       // 存储器插入
  void onMediaError() { TERN_(SDSUPPORT, ScreenHandler.SDCardError()); }             // 存储器错误
  void onMediaRemoved() { TERN_(SDSUPPORT, ScreenHandler.SDCardRemoved()); }         // 存储器拔出
  void onPlayTone(const uint16_t frequency, const uint16_t duration) {}              // 蜂鸣器
  void onPrinterPoweroff() { ScreenHandler.PrinterPoweroffHook(); }                  // 打印机关闭
  void onPrinterKilled(FSTR_P const error, FSTR_P const component) {                 // 打印机异常
    ScreenHandler.PrinterKilledHook(error, component);                               //
  }                                                                                  //
  void onPrintTimerStarted() { ScreenHandler.PrintTimerStartHook(); }                // 打印开始
  void onPrintTimerPaused() { ScreenHandler.PrintTimerPauseHook(); }                 // 打印暂停
  void onPrintTimerStopped() { ScreenHandler.PrintTimerStopHook(); }                 // 打印停止
  void onPrintDone() { ScreenHandler.PrintDoneHook(); }                              // 打印完成
  void onFilamentRunout(const extruder_t extruder) {                                 // 耗材检测触发
    ScreenHandler.FilamentRunoutHook();                                              //
  }                                                                                  //
  void onUserConfirmRequired(const char *const msg) {                                // 等待用户确认
    ScreenHandler.UserConfirmRequiredHook(msg);                                      //
  }                                                                                  //
  void onStatusChanged(const char *const msg) {                                      // 状态信息改变
    ScreenHandler.StatusChangedHook(msg);                                            //
  }                                                                                  //
  void onHomingStart() { ScreenHandler.HomingStartHook(); }                          // 回零开始
  void onHomingDone() { ScreenHandler.HomingDoneHook(); }                            // 回零完成
  void onSteppersDisabled() {}                                                       // 去能电机
  void onSteppersEnabled() {}                                                        // 使能电机
  void onFactoryReset() { ScreenHandler.FactoryResetHook(); }                        // 完成恢复出厂设置
  void onStoreSettings(char *buff) { ScreenHandler.StoreSettingsHook(buff); }        // 保存设置->屏幕相关
  void onLoadSettings(const char *buff) { ScreenHandler.LoadSettingsHook(buff); }    // 加载设置->屏幕相关
  void onPostprocessSettings() {}                                                    // 应用设置中(后处理过程)
  void onSettingsStored(const bool success) { ScreenHandler.SettingsStoredHook(); }  // 完成保存设置
  void onSettingsLoaded(const bool success) {}                                       // 完成加载设置

  #if ENABLED(POWER_LOSS_RECOVERY)
  void onSetPowerLoss(const bool onoff) {}                           // 断电续打设置
  void onPowerLoss() {}                                              // 断电续打触发
  void onPowerLossResume() { ScreenHandler.PowerLossResumeHook(); }  // 断电续打继续
  #endif
  #if ENABLED(CREATBOT_LINUX_LCD)
  void onPowerOffSafety() { ScreenHandler.PowerOffSafetyHook(); }  // 安全关机
  #endif
  #if HAS_PID_HEATING
  void onPidTuning(const result_t rst) {}  // PID调试中
  #endif

  #if HAS_MESH
  void onLevelingStart() { ScreenHandler.LevelingStartHook(); }  // 调平开始
  void onLevelingDone() { ScreenHandler.LevelingDoneHook(); }    // 调平结束

  void onMeshUpdate(const int8_t xpos, const int8_t ypos, const_float_t zval) {
    // Called when any mesh points are updated
  }

  void onMeshUpdate(const int8_t xpos, const int8_t ypos, const probe_state_t state) {
    // Called to indicate a special condition
  }
  #endif
}  // namespace ExtUI

#endif  // CREATBOT_LCD
