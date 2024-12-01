/**
 *  LinuxScreenCanOpen.cpp
 *
 *  Created on: 2023/07/28
 *      Author: CreatBot LYN
 *
 *  Protocol layer for linux lcd.
 */

#include "../../../../inc/MarlinConfigPre.h"

#if ENABLED(CREATBOT_LINUX_LCD)

  #include "../../../../MarlinCore.h"
  #include "../../../../canOpen/canOpen.h"
  #include "../../../../canOpen/canOpenHelper.h"
  #include "../../../../libs/circularqueue.h"
  #include "../../../../module/printcounter.h"
  #include "../../../../module/stepper.h"
  #include "../../../../module/temperature.h"
  #include "../../ui_api.h"
  #include "../creatbotFunction.h"
  #include "LinuxScreenCanOpen.h"
  #include "LinuxScreenHelper.h"

  #if ENABLED(POWER_LOSS_RECOVERY)
    #include "../../../../feature/powerloss.h"
  #endif
  #define DEBUG_OUT ENABLED(DEBUG_LINUX_SCREEN)
  #include "../../../../core/debug_out.h"

static uint8_t nodeNum    = 0;
static bool    LCDIsReady = false;

static CircularQueue<notifyLCD, NOTIFY_BUFLEN> notifyRing;

  #if ENABLED(SDSUPPORT)
static CreatBotFileList fileList;
  #endif

// 更新温度信息
static void updateTempInfo() {
  OPTCODE(HAS_HOTEND0, CurTemp[ARRIDX(T_E0)] = CANOPEN_CURTEMP(H0))
  OPTCODE(HAS_HOTEND1, CurTemp[ARRIDX(T_E1)] = CANOPEN_CURTEMP(H1))
  OPTCODE(HAS_HOTEND2, CurTemp[ARRIDX(T_E2)] = CANOPEN_CURTEMP(H2))
  OPTCODE(HAS_HOTEND3, CurTemp[ARRIDX(T_E3)] = CANOPEN_CURTEMP(H3))
  OPTCODE(HAS_HEATED_BED, CurTemp[ARRIDX(T_Bed)] = CANOPEN_CURTEMP(BED))
  OPTCODE(HAS_HEATED_CHAMBER, CurTemp[ARRIDX(T_Chamber)] = CANOPEN_CURTEMP(CHAMBER))
  // CurTemp[ARRIDX(T_Filament)] = 0;
  // CurTemp[ARRIDX(T_Water)]    = 0;

  OPTCODE(HAS_HOTEND0, TarTemp[ARRIDX(T_E0)] = CANOPEN_TARTEMP(H0))
  OPTCODE(HAS_HOTEND1, TarTemp[ARRIDX(T_E1)] = CANOPEN_TARTEMP(H1))
  OPTCODE(HAS_HOTEND2, TarTemp[ARRIDX(T_E2)] = CANOPEN_TARTEMP(H2))
  OPTCODE(HAS_HOTEND3, TarTemp[ARRIDX(T_E3)] = CANOPEN_TARTEMP(H3))
  OPTCODE(HAS_HEATED_BED, TarTemp[ARRIDX(T_Bed)] = CANOPEN_TARTEMP(BED))
  OPTCODE(HAS_HEATED_CHAMBER, TarTemp[ARRIDX(T_Chamber)] = CANOPEN_TARTEMP(CHAMBER))
  // TarTemp[ARRIDX(T_Filament)] = 0;
}

// 更新打印相关信息
static void updatePrintInfo() {
  TaskInfo_PrintPercent  = TERN(HAS_PRINT_PROGRESS_PERMYRIAD,  //
                                ExtUI::getProgress_permyriad() / 100.0, ExtUI::getProgress_percent());
  TaskInfo_PrintTime     = ExtUI::getProgress_seconds_elapsed();
  TaskInfo_PrintFeedRate = ExtUI::getFeedrate_percent();
  OPTCODE(HAS_EXTRUDER0, TaskInfo_PrintFlowrateE0 = ExtUI::getFlow_percent(ExtUI::extruder_t::E0))
  OPTCODE(HAS_EXTRUDER1, TaskInfo_PrintFlowrateE1 = ExtUI::getFlow_percent(ExtUI::extruder_t::E1))
  OPTCODE(HAS_EXTRUDER2, TaskInfo_PrintFlowrateE2 = ExtUI::getFlow_percent(ExtUI::extruder_t::E2))
  OPTCODE(HAS_EXTRUDER3, TaskInfo_PrintFlowrateE3 = ExtUI::getFlow_percent(ExtUI::extruder_t::E3))
}

// 更新百分比相关信息
static void updatePercentInfo() {
  OPTCODE(AUTO_FAN_CAN_ADJUST, Percent[ARRIDX(P_FanT)] = ui8_to_percent(thermalManager.extruder_auto_fan_speed))
  OPTCODE(HAS_FAN0, Percent[ARRIDX(P_FanG)] = ExtUI::getTargetFan_percent(ExtUI::fan_t::FAN0))
  OPTCODE(HAS_FAN1, Percent[ARRIDX(P_FanA)] = ExtUI::getTargetFan_percent(ExtUI::fan_t::FAN1))
  // TODO 自动关机时间索引
  Percent[ARRIDX(P_ShutdownTime)] = 0;
}

// 更新坐标位置信息
static void updatePosInfo() {
  OPTCODE(HAS_X_AXIS, Position[ARRIDX(Pos_X)] = ExtUI::getAxisPosition_mm(ExtUI::axis_t::X))
  OPTCODE(HAS_Y_AXIS, Position[ARRIDX(Pos_Y)] = ExtUI::getAxisPosition_mm(ExtUI::axis_t::Y))
  OPTCODE(HAS_Z_AXIS, Position[ARRIDX(Pos_Z)] = ExtUI::getAxisPosition_mm(ExtUI::axis_t::Z))
  // OPTCODE(HAS_X2_STEPPER, Position[ARRIDX(Pos_X2)] = ExtUI::getAxisPosition_mm(ExtUI::axis_t::X2))  // TODO
}

// 更新电机相关信息
static void updateStepInfo() {
  // 电机步进值
  OPTCODE(HAS_X_AXIS, StepPerMm[ARRIDX(SPM_X)] = ExtUI::getAxisSteps_per_mm(ExtUI::axis_t::X))
  OPTCODE(HAS_Y_AXIS, StepPerMm[ARRIDX(SPM_Y)] = ExtUI::getAxisSteps_per_mm(ExtUI::axis_t::Y))
  OPTCODE(HAS_Z_AXIS, StepPerMm[ARRIDX(SPM_Z)] = ExtUI::getAxisSteps_per_mm(ExtUI::axis_t::Z))
  OPTCODE(HAS_EXTRUDER0, StepPerMm[ARRIDX(SPM_E0)] = ExtUI::getAxisSteps_per_mm(ExtUI::extruder_t::E0))
  OPTCODE(HAS_EXTRUDER1, StepPerMm[ARRIDX(SPM_E1)] = ExtUI::getAxisSteps_per_mm(ExtUI::extruder_t::E1))
  OPTCODE(HAS_EXTRUDER2, StepPerMm[ARRIDX(SPM_E2)] = ExtUI::getAxisSteps_per_mm(ExtUI::extruder_t::E2))
  OPTCODE(HAS_EXTRUDER3, StepPerMm[ARRIDX(SPM_E3)] = ExtUI::getAxisSteps_per_mm(ExtUI::extruder_t::E3))

  // 加速度
  Acceleration[ARRIDX(Acc_Print)]    = ExtUI::getPrintingAcceleration_mm_s2();
  Acceleration[ARRIDX(Acc_Rectract)] = ExtUI::getRetractAcceleration_mm_s2();
  Acceleration[ARRIDX(Acc_Travel)]   = ExtUI::getTravelAcceleration_mm_s2();
  OPTCODE(HAS_X_AXIS, Acceleration[ARRIDX(Acc_MaxX)] = ExtUI::getAxisMaxAcceleration_mm_s2(ExtUI::axis_t::X))
  OPTCODE(HAS_Y_AXIS, Acceleration[ARRIDX(Acc_MaxY)] = ExtUI::getAxisMaxAcceleration_mm_s2(ExtUI::axis_t::Y))
  OPTCODE(HAS_Z_AXIS, Acceleration[ARRIDX(Acc_MaxZ)] = ExtUI::getAxisMaxAcceleration_mm_s2(ExtUI::axis_t::Z))
  OPTCODE(HAS_EXTRUDER0, Acceleration[ARRIDX(Acc_MaxE)] = ExtUI::getAxisMaxAcceleration_mm_s2(ExtUI::extruder_t::E0))
}

// 更新统计信息
static void updateStatistics() {
  #if ENABLED(PRINTCOUNTER)
  printStatistics state    = print_job_timer.getStats();
  Statistics_PrintsTotal   = state.totalPrints;
  Statistics_PrintsSuccess = state.finishedPrints;
  Statistics_PrintsLongest = state.longestPrint;
  Statistics_PrintsUsed    = state.printTime;
  Statistics_FilamentUsed  = state.filamentUsed;
  #endif
}

// 更新调平信息
static void updateLeveling() {
  #if HAS_LEVELING
  Leveling_ZOffset    = ExtUI::getZOffset_mm() * -1.0;
  Leveling_FadeHeight = planner.z_fade_height;
  #endif
}

static void firstUnpack(switchVal);
static void refreshFileToLcd(fileAct);
static void enableMoter(switchVal);
static void enableLight(switchVal);
static void enableAutoLeveling(switchVal);
static void changeExtruder(uint8_t);
static void stopOperateWaiting(operate);
static void setHeaterWaiting(heating);
static void setLoadWaiting(loading);

// 温度设置回调函数
static uint32_t onTarTempSet(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_TARTEMP) {
    switch (subindex) {
      case subIndex_T_E0: OPTCODE(HAS_HOTEND, CANOPEN_SETTEMP(H0, TarTemp[ARRIDX(T_E0)])) break;
      case subIndex_T_E1: OPTCODE(HAS_HOTEND1, CANOPEN_SETTEMP(H1, TarTemp[ARRIDX(T_E1)])) break;
      case subIndex_T_E2: OPTCODE(HAS_HOTEND2, CANOPEN_SETTEMP(H2, TarTemp[ARRIDX(T_E2)])) break;
      case subIndex_T_E3: OPTCODE(HAS_HOTEND3, CANOPEN_SETTEMP(H3, TarTemp[ARRIDX(T_E3)])) break;
      case subIndex_T_Bed: OPTCODE(HAS_HEATED_BED, CANOPEN_SETTEMP(BED, TarTemp[ARRIDX(T_Bed)])) break;
      case subIndex_T_Chamber: OPTCODE(HAS_HEATED_CHAMBER, CANOPEN_SETTEMP(CHAMBER, TarTemp[ARRIDX(T_Chamber)])) break;
      case subIndex_T_Filament:
      case subIndex_T_Water:
      default: break;
    }
  }
  updateTempInfo();  // 再次更新温度信息, 避免温度设置被最大值限制

  return OD_SUCCESSFUL;
}

// 挤出量设置回调函数
static uint32_t onFlowrateSet(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_TASKINFO) {
    switch (subindex) {
      case subIndex_Task_PrintFlowrateE0:
        OPTCODE(HAS_EXTRUDERS, ExtUI::setFlow_percent(TaskInfo_PrintFlowrateE0, ExtUI::extruder_t::E0))
        break;
      case subIndex_Task_PrintFlowrateE1:
        OPTCODE(HAS_EXTRUDER1, ExtUI::setFlow_percent(TaskInfo_PrintFlowrateE1, ExtUI::extruder_t::E1))
        break;
      case subIndex_Task_PrintFlowrateE2:
        OPTCODE(HAS_EXTRUDER2, ExtUI::setFlow_percent(TaskInfo_PrintFlowrateE2, ExtUI::extruder_t::E2))
        break;
      case subIndex_Task_PrintFlowrateE3:
        OPTCODE(HAS_EXTRUDER3, ExtUI::setFlow_percent(TaskInfo_PrintFlowrateE3, ExtUI::extruder_t::E3))
        break;
      default: break;
    }
  }
  return OD_SUCCESSFUL;
}

// 打印速度设置回调函数
static uint32_t onFeedrateSet(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_TASKINFO) {
    if (subindex = subIndex_Task_PrintFeedRate) {
      ExtUI::setFeedrate_percent(TaskInfo_PrintFeedRate);
    }
  }
  return OD_SUCCESSFUL;
}

// 百分比设置回调函数
static uint32_t onPercentSet(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_PERCNET) {
    switch (subindex) {
      case subIndex_P_FanT:
        OPTCODE(AUTO_FAN_CAN_ADJUST, thermalManager.extruder_auto_fan_speed = percent_to_ui8(Percent[ARRIDX(P_FanT)]))
        break;
      case subIndex_P_FanG:
        OPTCODE(HAS_FAN0, ExtUI::setTargetFan_percent(Percent[ARRIDX(P_FanG)], ExtUI::fan_t::FAN0))
        break;
      case subIndex_P_FanA:
        OPTCODE(HAS_FAN1, ExtUI::setTargetFan_percent(Percent[ARRIDX(P_FanA)], ExtUI::fan_t::FAN1))
        break;
      case subIndex_P_ShutdownTime: break;  // TODO
      default: break;
    }
  }
  return OD_SUCCESSFUL;
}

// 步进值设置回调函数
static uint32_t onSPMSet(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_STEPPERMM) {
    switch (subindex) {
      case subIndex_SPM_X:
        OPTCODE(HAS_X_AXIS, ExtUI::setAxisSteps_per_mm(StepPerMm[ARRIDX(SPM_X)], ExtUI::axis_t::X))
        break;
      case subIndex_SPM_Y:
        OPTCODE(HAS_Y_AXIS, ExtUI::setAxisSteps_per_mm(StepPerMm[ARRIDX(SPM_Y)], ExtUI::axis_t::Y))
        break;
      case subIndex_SPM_Z:
        OPTCODE(HAS_Z_AXIS, ExtUI::setAxisSteps_per_mm(StepPerMm[ARRIDX(SPM_Z)], ExtUI::axis_t::Z))
        break;
      case subIndex_SPM_E0:
        OPTCODE(HAS_EXTRUDER0, ExtUI::setAxisSteps_per_mm(StepPerMm[ARRIDX(SPM_E0)], ExtUI::extruder_t::E0))
        break;
      case subIndex_SPM_E1:
        OPTCODE(HAS_EXTRUDER1, ExtUI::setAxisSteps_per_mm(StepPerMm[ARRIDX(SPM_E1)], ExtUI::extruder_t::E1))
        break;
      case subIndex_SPM_E2:
        OPTCODE(HAS_EXTRUDER2, ExtUI::setAxisSteps_per_mm(StepPerMm[ARRIDX(SPM_E2)], ExtUI::extruder_t::E2))
        break;
      case subIndex_SPM_E3:
        OPTCODE(HAS_EXTRUDER3, ExtUI::setAxisSteps_per_mm(StepPerMm[ARRIDX(SPM_E3)], ExtUI::extruder_t::E3))
        break;
      default: break;
    }
  }
  return OD_SUCCESSFUL;
}

// 加速度设置回调函数
static uint32_t onAccSet(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_ACCELERATION) {
    switch (subindex) {
      case subIndex_Acc_Print: ExtUI::setPrintingAcceleration_mm_s2(Acceleration[ARRIDX(Acc_Print)]); break;
      case subIndex_Acc_Rectract: ExtUI::setRetractAcceleration_mm_s2(Acceleration[ARRIDX(Acc_Rectract)]); break;
      case subIndex_Acc_Travel: ExtUI::setTravelAcceleration_mm_s2(Acceleration[ARRIDX(Acc_Travel)]); break;
      case subIndex_Acc_MaxX:
        OPTCODE(HAS_X_AXIS, ExtUI::setAxisMaxAcceleration_mm_s2(Acceleration[ARRIDX(Acc_MaxX)], ExtUI::axis_t::X))
        break;
      case subIndex_Acc_MaxY:
        OPTCODE(HAS_Y_AXIS, ExtUI::setAxisMaxAcceleration_mm_s2(Acceleration[ARRIDX(Acc_MaxY)], ExtUI::axis_t::Y))
        break;
      case subIndex_Acc_MaxZ:
        OPTCODE(HAS_Z_AXIS, ExtUI::setAxisMaxAcceleration_mm_s2(Acceleration[ARRIDX(Acc_MaxZ)], ExtUI::axis_t::Z))
        break;
      case subIndex_Acc_MaxE:
        OPTCODE(HAS_EXTRUDER0,
                ExtUI::setAxisMaxAcceleration_mm_s2(Acceleration[ARRIDX(Acc_MaxE)], ExtUI::extruder_t::E0))
        break;
      default: break;
    }
  }
  return OD_SUCCESSFUL;
}

// 调平参数设置回调函数
static uint32_t onLevelSet(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  #if HAS_LEVELING
  if (indexTable->index == INDEX_LEVELING) {
    switch (subindex) {
      case subIndex_Level_ZOffset: ExtUI::setZOffset_mm(Leveling_ZOffset * -1.0); break;
      case subIndex_Level_FadeHeight: planner.set_z_fade_height(Leveling_FadeHeight); break;
      default: break;
    }
  }
  #endif
  return OD_SUCCESSFUL;
}

// 任务指令回调函数
static uint32_t onCmdTask(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_CMD_TASK) {
    switch ((taskCmd)(CMD_TASK & 0xFF)) {
      case taskCmd::TASK_PAUSE:
        if (ExtUI::isPrintingFromMedia() && !ExtUI::isPrintingFromMediaPaused()) {
          ExtUI::pausePrint();
        }
        break;
      case taskCmd::TASK_RESUME:
        if (ExtUI::isPrintingFromMediaPaused()) {
          if (TERN1(HAS_FILAMENT_SENSOR, ExtUI::getFilamentInsertState())) {
            ExtUI::resumePrint();
          } else {
            sendNotify(notifyLCD::FILAMENT_WAIT);
          }
        }
        break;
      case taskCmd::TASK_STOP:
        if (ExtUI::isPrintingFromMedia()) {
          quickstop_stepper();
          ExtUI::stopPrint();
        }
        break;
      case taskCmd::TASK_CHANGE:
        if (ExtUI::isPrintingFromMedia() && !ExtUI::isPrintingFromMediaPaused()) {
          ExtUI::injectCommands(F("M600"));
          sendNotify(notifyLCD::FILAMENT_INIT);
        }
        break;
      case taskCmd::ACCIDENT_RESUME: break;
      case taskCmd::ACCIDENT_CANCEL: break;
      default: break;
    }
  }
  return OD_SUCCESSFUL;
}

// 移轴指令回调函数
static uint32_t onCmdMove(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_CMD_MOVE) {
    float offset;

    // clang-format off
    switch ((moveUnit)(CMD_MOVE & 0xFF)) {  // 低字节表示移轴单位
      case moveUnit::MOVE_01MM:   offset = 0.1;   break;
      case moveUnit::MOVE_1MM:    offset = 1.0;   break;
      case moveUnit::MOVE_10MM:   offset = 10.0;  break;
      case moveUnit::MOVE_100MM:  offset = 100.0; break;
      default:                    offset = 0.0;   break;
    }

    switch ((moveIndex)(CMD_MOVE >> 8)) {  // 高字节表示移轴命令
      case moveIndex::ALL_HOME:   ExtUI::injectCommands(F("G28"));  break;
      case moveIndex::X_HOME:     ExtUI::injectCommands(F("G28X")); break;
      case moveIndex::Y_HOME:     ExtUI::injectCommands(F("G28Y")); break;
      case moveIndex::Z_HOME:     ExtUI::injectCommands(F("G28Z")); break;
      case moveIndex::X_MINUS:    offset *= -1;
      case moveIndex::X_PLUS:     delayMoveAxis(offset, ExtUI::axis_t::X); break;
      case moveIndex::Y_MINUS:    offset *= -1;
      case moveIndex::Y_PLUS:     delayMoveAxis(offset, ExtUI::axis_t::Y); break;
      case moveIndex::Z_MINUS:    offset *= -1;
      case moveIndex::Z_PLUS:     delayMoveAxis(offset, ExtUI::axis_t::Z); break;
      default: break;
    }
    // clang-format on
  }
  return OD_SUCCESSFUL;
}

// 送/退丝, 加/卸载 回调函数
static uint32_t onCmdExtrude(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_CMD_EXTRUDE) {
    ExtUI::extruder_t curTool = ExtUI::getActiveTool();
    if (thermalManager.targetTooColdToExtrude(curTool)) {
      sendNotify(notifyLCD::TAR_TEMP_LOW);
    } else {
      if (CMD_EXTRUDER == INT16_MAX) {         // 0x7FFF
        ExtUI::injectCommands(F("M701"));      // 加载耗材
      } else if (CMD_EXTRUDER == INT16_MIN) {  // 0x8000
        ExtUI::injectCommands(F("M702"));      // 卸载耗材
      } else {
        if (thermalManager.tooColdToExtrude(curTool)) {
          sendNotify(notifyLCD::CUR_TEMP_LOW);
        } else {
          UI_INCREMENT_BY(AxisPosition_mm, (float)CMD_EXTRUDER, curTool);
          sendNotify((CMD_EXTRUDER > 0) ? notifyLCD::FILA_EXTRUDE : notifyLCD::FILA_RETRACT);
        }
      }
    }
  }
  return OD_SUCCESSFUL;
}

// 其他通用指令回调函数
static uint32_t onCmdCommon(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_CMD_COMMON) {
    uint8_t cmdPara = CMD_COMMON & 0xFF;     // 低字节表示参数
    switch ((commonCmd)(CMD_COMMON >> 8)) {  // 高字节表示指令
      case commonCmd::FACTORY_RESET: ExtUI::injectCommands(F("M502")); break;
      case commonCmd::SAVE_SETTING: ExtUI::injectCommands(F("M500")); break;
      case commonCmd::FIRST_UNPACK: firstUnpack((switchVal)cmdPara); break;
      case commonCmd::REFRESH_FILE: refreshFileToLcd((fileAct)cmdPara); break;
      case commonCmd::MOTOR_SWITCH: enableMoter((switchVal)cmdPara); break;
      case commonCmd::BABYSTEP: break;
      case commonCmd::CASELIGHT: enableLight((switchVal)cmdPara); break;
      case commonCmd::ATUOSHUTDWON: break;
      case commonCmd::ATUOLEVELING: enableAutoLeveling((switchVal)cmdPara); break;
      case commonCmd::CHANGE_TOOL: changeExtruder(cmdPara); break;
      case commonCmd::STOP_OPERATE: stopOperateWaiting((operate)cmdPara); break;
      case commonCmd::WAIT_HEATING: setHeaterWaiting((heating)cmdPara); break;
      case commonCmd::LOAD_FILAMENT: setLoadWaiting((loading)cmdPara); break;
      case commonCmd::LEVELING_PROBE: ExtUI::injectCommands(F("G29N")); break;
      case commonCmd::USER_CONFIRMED: ExtUI::setUserConfirmed(); break;
      case commonCmd::AUTO_POWEROFF: LCDCtrlShutdown(); break;
    }
  }
  return OD_SUCCESSFUL;
}

// 指定文件索引回调函数
static uint32_t onFileIndex(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  #if ENABLED(SDSUPPORT)
  if (indexTable->index == INDEX_FILE_INDEX) {
    if (FILE_INDEX == UINT16_MAX) {  // 返回上级
      fileList.upDir();
      refreshFileToLcd(fileAct::WORKDIR);
    } else {
      if (fileList.seek(SD_ORDER(FILE_INDEX, fileList.count()))) {
        if (fileList.isDir()) {  // 打开子目录
          fileList.changeDir(fileList.shortFilename());
          refreshFileToLcd(fileAct::WORKDIR);
        } else {  // 打印文件
          ExtUI::printFile(fileList.shortFilename());
        }
      }
    }
  }
  #endif
  return OD_SUCCESSFUL;
}

// 初始化屏幕相关常量信息
void initNodeInfoLCD() {
  #if HAS_AUTO_FAN
  Percent[ARRIDX(P_FanT)] = ui8_to_percent(EXTRUDER_AUTO_FAN_SPEED);
  #endif
}

// 注册屏幕相关字典回调函数
void registerNodeLCD() {
  registerODCallBackAll(INDEX_TARTEMP, subIndex_T_Count, &onTarTempSet);
  registerODCallBackOne(INDEX_TASKINFO, subIndex_Task_PrintFlowrateE0, &onFlowrateSet);
  registerODCallBackOne(INDEX_TASKINFO, subIndex_Task_PrintFlowrateE1, &onFlowrateSet);
  registerODCallBackOne(INDEX_TASKINFO, subIndex_Task_PrintFlowrateE2, &onFlowrateSet);
  registerODCallBackOne(INDEX_TASKINFO, subIndex_Task_PrintFlowrateE3, &onFlowrateSet);
  registerODCallBackOne(INDEX_TASKINFO, subIndex_Task_PrintFeedRate, &onFeedrateSet);
  registerODCallBackAll(INDEX_PERCNET, subIndex_P_Count, &onPercentSet);
  registerODCallBackAll(INDEX_STEPPERMM, subIndex_SPM_Count, &onSPMSet);
  registerODCallBackAll(INDEX_ACCELERATION, subIndex_Acc_Count, &onAccSet);
  registerODCallBackAll(INDEX_REGISTER, subIndex_Reg_Count, nullptr);
  registerODCallBackAll(INDEX_LEVELING, subIndex_Level_Count, &onLevelSet);
  registerODCallBackOne(INDEX_CMD_TASK, 0x00, &onCmdTask);
  registerODCallBackOne(INDEX_CMD_MOVE, 0x00, &onCmdMove);
  registerODCallBackOne(INDEX_CMD_EXTRUDE, 0x00, &onCmdExtrude);
  registerODCallBackOne(INDEX_CMD_COMMON, 0x00, &onCmdCommon);
  registerODCallBackOne(INDEX_FILE_INDEX, 0x00, &onFileIndex);
}

// 更新屏幕相关字典
void updateLCDInfo() {
  updateTempInfo();
  updatePrintInfo();
  updatePercentInfo();
  updatePosInfo();
  updateStepInfo();
  updateStatistics();
  updateLeveling();
}

// 屏幕PDO过滤
void PDOFilterLCD() {
  // 禁用屏幕用不到的PDO, 以节省带宽
  disablePDO(PDO(4));                             // 当前温度预留PDO
  disablePDO(PDO(7));                             // 目标温度预留PDO
  disablePDO(PDO(22));                            // 电机参数预留PDO
  disablePDO(PDO(23));                            // 电机参数预留PDO
  disablePDO(PDO(26));                            // 打印统计预留PDO
  disablePDO(PDO(29));                            // 自动调平预留PDO
  OPTCODE(DISABLE_LEVELING, disablePDO(PDO(28)))  // 调平相关PDO
}

// 检测屏幕的SDO缓存
void checkSDOCacheLCD() {
  // 检测是否有SDO通知要发送
  if (!notifyRing.isEmpty()) {
    DEBUG_ECHOLNPGM("reSendNotify");
    sendNotify(notifyRing.dequeue());
    return;
  }

  // 其他检测
}

static void sendNotifyIndex(notifyLCD notifyIndex, void (*successCallback)(notifyLCD)) {
  DEBUG_ECHOPGM("sendNotify: ", notifyIndex, "\t");
  if (sendNotifyToLCD((uint16_t &)notifyIndex, fisishSDO)) {  // 通知发送失败, 缓存通知索引
    DEBUG_ECHOLNPGM("Cached!");
    if (!notifyRing.enqueue(notifyIndex)) {
      DEBUG_ECHOLNPGM("NOTIFY buffer seems too small.");
    }
  } else {
    successCallback(notifyIndex);  // 调用发送成功的回调函数，并传递成功的通知索引
  }
}

// 发送成功的回调函数
static void notifySendCallback(notifyLCD notifyIndex) {
  DEBUG_ECHOLNPGM("Notify: ", notifyIndex, " send successfully.");
}

// 发送通知
void sendNotify(notifyLCD notifyIndex) {
  return sendNotifyIndex(notifyIndex, notifySendCallback);
}

// 切换首次开箱
static void firstUnpack(switchVal onoff) {
  switch (onoff) {
    case switchVal::ON: {
      ExtUI::setAxisPosition_mm(Z_MAX_POS, ExtUI::axis_t::Z, 0.0, false);  // 预设Z高度为最大值
      sync_plan_position();
    } break;
    case switchVal::OFF: {
      if (!ExtUI::isAxisPositionKnown(ExtUI::axis_t::Z)) {             // Z轴位置未知
        ExtUI::setAxisPosition_mm(0.0, ExtUI::axis_t::Z, 0.0, false);  // 恢复Z高度为默认值
        sync_plan_position();
      }
    } break;
    default: break;
  }
}

// 切换电机使能
static void enableMoter(switchVal onoff) {
  switch (onoff) {
    case switchVal::ON: stepper.enable_all_steppers(); break;
    case switchVal::OFF: stepper.disable_all_steppers(); break;
    default: break;
  }
}

// 切换机箱照明
static void enableLight(switchVal onoff) {
  OPTCODE(CASE_LIGHT_ENABLE, if (onoff == ExtUI::getCaseLightState()) return)
  if (onoff == switchVal::TOGGLE) {
    OPTCODE(CASE_LIGHT_ENABLE, ExtUI::setCaseLightState(!ExtUI::getCaseLightState()))
  } else {
    OPTCODE(CASE_LIGHT_ENABLE, ExtUI::setCaseLightState(onoff))
  }
}

// 切换自动调平
static void enableAutoLeveling(switchVal onoff) {
  OPTCODE(HAS_LEVELING, if (onoff == ExtUI::getLevelingActive()) return)
  if (onoff == switchVal::TOGGLE) {
    OPTCODE(HAS_LEVELING, ExtUI::setLevelingActive(!ExtUI::getLevelingActive()))
  } else {
    OPTCODE(HAS_LEVELING, ExtUI::setLevelingActive(onoff))
  }
}

// 切换喷头
static void changeExtruder(uint8_t toolIndex) {
  switch (toolIndex) {
    OPTCODE(HAS_HOTEND0, case 0 : ExtUI::setActiveTool(ExtUI::extruder_t::E0, DISABLED(DUAL_X_CARRIAGE)); break)
    OPTCODE(HAS_HOTEND1, case 1 : ExtUI::setActiveTool(ExtUI::extruder_t::E1, DISABLED(DUAL_X_CARRIAGE)); break)
    OPTCODE(HAS_HOTEND2, case 2 : ExtUI::setActiveTool(ExtUI::extruder_t::E2, DISABLED(DUAL_X_CARRIAGE)); break)
    OPTCODE(HAS_HOTEND3, case 3 : ExtUI::setActiveTool(ExtUI::extruder_t::E3, DISABLED(DUAL_X_CARRIAGE)); break)
    default: break;
  }
}

// 取消各种操作的等待 (回零, 移轴, 送丝 等)
static void stopOperateWaiting(operate action) {
  switch (action) {
    case operate::HOME: quickstop_stepper(); break;
    case operate::MOVE:
      quickstop_stepper();
      planner.cleaning_buffer_counter = 0;
      break;
    case operate::EXTRUDE:
      quickstop_stepper();
      planner.cleaning_buffer_counter = 0;
      stepper.disable_e_steppers();
      break;
    case operate::PROBE: quickstop_stepper(); break;
    case operate::SHUTDOWN: TERN_(POWER_OFF_WAIT_FOR_COOLDOWN, powerManager.cancelAutoPowerOff());
  #if ENABLED(POWER_LOSS_RECOVERY)
      if (TERN1(RECOVERY_USE_EEPROM, recovery.check())) recovery.state = true;
  #endif
      break;
    default: break;
  }
}

// 设置喷头加热等待状态 (跳过加热, 重新加热)
static void setHeaterWaiting(heating action) {
  switch (action) {
    case heating::SKIP: wait_for_heatup = false; break;
    case heating::AGAIN: ExtUI::setUserConfirmed(); break;
    default: break;
  }
}

// 设置耗材加载等待状态 (继续打印, 加载更多)
static void setLoadWaiting(loading action) {
  #if M600_PURGE_MORE_RESUMABLE
  switch (action) {
    case loading::CONTINUE: ExtUI::setPauseMenuResponse(PAUSE_RESPONSE_RESUME_PRINT); break;
    case loading::MORE: ExtUI::setPauseMenuResponse(PAUSE_RESPONSE_EXTRUDE_MORE); break;
    default: break;
  }
  #else
  UNUSED(action);
  #endif
}

  #if ENABLED(SDSUPPORT)
/**
 * 刷新工作文件夹
 *    快速SDO  B1_< + B2_count + B1_isRoot
 *    分块SDO  B1_* + B2_index + B1_isDir + B4_size + B4_timeStamp + longFilename
 *    分块SDO  B1_* ...
 *    分块SDO  B1_* ...
 *    快速SDO  B1_>
 *
 * 获取当前文件名
 *    分块SDO  B1_: + longFilename
 **/
static void refreshFileToLcdCallBack(CO_Data *d, UNS8 nodeId) {
  resetClientSDOLineFromNodeId(d, nodeId);
  if ((uint16_t)(fileList.fileIndex() + 1) == fileList.count()) {
    uint8_t data = '>';  // 文件列表 尾
    sendFileInfoToLCD(1, &data, fisishSDO, false);
    DEBUG_ECHOLNPGM("refreshFileToLcd Done");
  } else {
    fileList.nextFile();          // 获取新文件名信息
    uint8_t     data[256] = {0};  // 文件列表 列表项
    const char *fname     = fileList.filename();
    data[0]               = '*';
    data[3]               = fileList.isDir();
    *(uint16_t *)&data[1] = fileList.fileIndex();
    *(uint32_t *)&data[4] = fileList.fileSize();
    *(uint32_t *)&data[8] = fileList.timeStamp();
    strcpy((char *)&data[12], fname);
    DEBUG_ECHOLNPGM("index: ", *(uint16_t *)&data[1], ",\tisDir: ", data[3], ",\tname: ", fname);
    sendFileInfoToLCD(12 + strlen(fname), data, refreshFileToLcdCallBack, true);
  }
}

static void refreshFileToLcd(fileAct action) {
  switch (action) {
    case fileAct::WORKDIR: {
      DEBUG_ECHOLNPGM("refreshFileToLcd Init");
      fileList.refresh();           // 刷新工作目录
      uint8_t data[4]       = {0};  // 文件列表 头
      data[0]               = '<';
      data[3]               = fileList.isAtRootDir();
      *(uint16_t *)&data[1] = fileList.count();
      sendFileInfoToLCD(4, data, refreshFileToLcdCallBack, false);  // 发送文件列表信息
    } break;
    case fileAct::FILENAME: {
      DEBUG_ECHOLNPGM("refreshFileToLcd curFileName");
      uint8_t     data[256] = {0};  // 文件名
      const char *fname     = fileList.filename();
      data[0]               = ':';
      strcpy((char *)&data[1], fname);
      sendFileInfoToLCD(1 + strlen(fname), data, fisishSDO, true);
    } break;
    default: break;
  }
}
  #else   //! SDSUPPORT
static void refreshFileToLcd(fileAct action) {}
  #endif  // SDSUPPORT

#endif  // CREATBOT_LINUX_LCD
