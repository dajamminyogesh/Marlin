/**
 *  LinuxScreenHandler.cpp
 *
 *  Created on: 2023/07/27
 *      Author: CreatBot LYN
 *
 *  handler for linux lcd.
 */

#include "../../../../inc/MarlinConfigPre.h"

#if ENABLED(CREATBOT_LINUX_LCD)

  #include "../creatbotFunction.h"
  #include "LinuxScreenCanOpen.h"
  #include "LinuxScreenHandler.h"

void LinuxScreenHandler::init() { creatbotInit(); }

void LinuxScreenHandler::loop() { creatbotLoop(axisMoveCallback); }

void startMoveHook() { sendNotify(AXIS_MOVING); }

// 状态信息改变
void LinuxScreenHandler::StatusChangedHook(const char *const msg) {}

// 通知用户确认
void LinuxScreenHandler::UserConfirmRequiredHook(const char *const msg) {
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
  StatusChangedHook(msg);  // 更新状态字符串
  #endif

  if (!strcmp_P(msg, GET_TEXT(MSG_ADVANCED_PAUSE_WAITING)) ||         //
      !strcmp_P(msg, GET_TEXT(MSG_USERWAIT))) {                       // 操作等待: 按下继续
    sendNotify(notifyLCD::WAITINT_USER);                              //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_NOZZLE_PARKED))) {           // 操作等待: 喷头停靠完成,按下继续
    sendNotify(notifyLCD::PRINT_PARKED);                              //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_HEAT)) ||    //
             !strcmp_P(msg, GET_TEXT(MSG_HEATER_TIMEOUT))) {          // 操作等待: 喷头已经冷却,按下加热
    sendNotify(notifyLCD::HEATER_TIMEOUT);                            //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_INSERT))) {  // 操作等待: 插入耗材,按下继续
    sendNotify(notifyLCD::FILAMENT_WAIT);                             //
  }

  else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_CONT_PURGE))) {       // 操作等待: 出丝完成,继续打印
    sendNotify(notifyLCD::PURGE_FINISH);                                     //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_OPTION_HEADER))) {  // 操作等待: 挤出更多|继续打印
    sendNotify(notifyLCD::PURGE_OPTION);                                     //
  }

  else if (!strcmp_P(msg, GET_TEXT(MSG_PAUSE_PRINT_SYNCING))) {        // 状态等待      缓冲区执行中 ...
    sendNotify(notifyLCD::PRINT_SYNCING);                              //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_PAUSE_PRINT_PARKING))) {      // 状态等待      喷头停靠中 ...
    sendNotify(notifyLCD::PRINT_PARKING);                              //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_INIT))) {     // 状态等待      更换耗材 ...
    sendNotify(notifyLCD::FILAMENT_INIT);                              //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_LOAD))) {     // 状态等待/执行 耗材装载中 ...
    sendNotify(notifyLCD::FILA_LOADING);                               //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_DELAY))) {    // 状态等待/执行 耗材冷却中 ...
    sendNotify(notifyLCD::FILAMENT_DELAY);                             //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_UNLOAD))) {   // 状态等待/执行 耗材卸载中 ...
    sendNotify(notifyLCD::FILA_UNLOADING);                             //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_PURGE))) {    // 状态执行      出丝测试中 ...
    sendNotify(notifyLCD::FILAMENT_PURGE);                             //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_RESUME))) {   // 状态执行      打印恢复中 ...
    sendNotify(notifyLCD::PRINT_RESUMING);                             //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_FILAMENT_CHANGE_HEATING))) {  // 状态等待      加热中 ...
    sendNotify(notifyLCD::HEATER_HEATING);                             //
  } else if (!strcmp_P(msg, GET_TEXT(MSG_REHEATDONE))) {               // 状态指示      加热完成.
    sendNotify(notifyLCD::HEATER_FINISH);                              //
  } else if (!strcmp_P(msg, "")) {                                     // 状态指示      操作完成.
    sendNotify(notifyLCD::ACTION_FINISH);                              //
  } else {                                                             // 其他未知消息
    sendNotify(notifyLCD::OTHER_NOTIFY);                               //
  }
}

  #if ENABLED(SDSUPPORT)
// U盘插入
void LinuxScreenHandler::SDCardInserted() {
  setMediaState(true);
  sendNotify(notifyLCD::MEDIA_INSERT);
}

// U盘异常
void LinuxScreenHandler::SDCardError() {
  setMediaState(false);
  sendNotify(notifyLCD::MEDIA_ERROR);
}

// U盘移除
void LinuxScreenHandler::SDCardRemoved() {
  setMediaState(false);
  sendNotify(notifyLCD::MEDIA_REMOVE);
}
  #endif

// 打印机关闭
void LinuxScreenHandler::PrinterPoweroffHook() {
  #if ENABLED(POWER_OFF_WAIT_FOR_COOLDOWN)
  HOTEND_LOOP() {
    if (ExtUI::getActualTemp_celsius(ExtUI::extruder_t(e)) >= (AUTO_POWER_E_TEMP)) {
      sendNotify(notifyLCD::MAX_TEMP_HIGH);
      break;
    }
  }
  #endif
}

// 打印机异常
void LinuxScreenHandler::PrinterKilledHook(FSTR_P const error, FSTR_P const component) {}

// 打印任务开始
void LinuxScreenHandler::PrintTimerStartHook() { sendNotify(notifyLCD::TIMER_START); }

// 打印任务暂停
void LinuxScreenHandler::PrintTimerPauseHook() { sendNotify(notifyLCD::TIMER_PAUSE); }

// 打印任务停止
void LinuxScreenHandler::PrintTimerStopHook() { sendNotify(notifyLCD::TIMER_STOP); }

// 打印任务完成
void LinuxScreenHandler::PrintDoneHook() { sendNotify(notifyLCD::TASK_DONE); }

// 恢复出厂设置
void LinuxScreenHandler::FactoryResetHook() {
  sendNotify(notifyLCD::FACTORY_DONE);
  settings.save();
}

// 保存屏幕相关设置
void LinuxScreenHandler::StoreSettingsHook(char *) {}

// 加载屏幕相关设置
void LinuxScreenHandler::LoadSettingsHook(const char *) {}

// 完成保存设置
void LinuxScreenHandler::SettingsStoredHook() { sendNotify(notifyLCD::SAVE_DONE); }

// 开始回零
void LinuxScreenHandler::HomingStartHook() { sendNotify(notifyLCD::HOME_START); }

// 回零完成
void LinuxScreenHandler::HomingDoneHook() { sendNotify(notifyLCD::HOME_DONE); }

// 耗材检测触发
void LinuxScreenHandler::FilamentRunoutHook() { sendNotify(notifyLCD::FILAMENT_ERROR); }

  #if HAS_LEVELING
// 调平开始
void LinuxScreenHandler::LevelingStartHook() { sendNotify(notifyLCD::LEVELING_START); }

// 调平完成
void LinuxScreenHandler::LevelingDoneHook() { sendNotify(notifyLCD::LEVELING_DONE); }
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
// 断电续打继续
void LinuxScreenHandler::PowerLossResumeHook() {}
  #endif

// 安全关机
void LinuxScreenHandler::PowerOffSafetyHook() { sendNotify(notifyLCD::POWEROFF_SAFETY); }

#endif  // CREATBOT_LINUX_LCD