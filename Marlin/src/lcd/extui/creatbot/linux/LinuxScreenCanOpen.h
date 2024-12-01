/**
 *  LinuxScreenCanOpen.h
 *
 *  Created on: 2023/07/28
 *      Author: CreatBot LYN
 *
 *  Protocol layer for linux lcd.
 */
#pragma once

// #define DEBUG_LINUX_SCREEN

#define NOTIFY_BUFLEN 8

// PDO2 - PDO7
enum subIndexTemperture : uint8_t {
  subIndex_T_E0 = 1,
  subIndex_T_E1,
  subIndex_T_E2,
  subIndex_T_E3,
  subIndex_T_Bed,
  subIndex_T_Chamber,
  subIndex_T_Filament,
  subIndex_T_Water,
  // subIndex_T_Reserve1,
  // subIndex_T_Reserve2,
  // subIndex_T_Reserve3,
  // subIndex_T_Reserve4,
  subIndex_T_Count
};

// PDO8 - PDO10
enum subIndexTask : uint8_t {
  subIndex_Task_PrintPercent = 1,
  subIndex_Task_PrintTime,
  subIndex_Task_PrintFlowrateE0,
  subIndex_Task_PrintFlowrateE1,
  subIndex_Task_PrintFlowrateE2,
  subIndex_Task_PrintFlowrateE3,
  subIndex_Task_PrintFeedRate,
  // subIndex_Task_PrintReserve,
  subIndex_Task_Count
};

// PDO11 - PDO11
enum sudIndexPercent : uint8_t {
  subIndex_P_FanT = 1,
  subIndex_P_FanG,
  subIndex_P_FanA,
  subIndex_P_FanReserve,
  subIndex_P_ShutdownTime,
  // subIndex_P_Reserve1,
  // subIndex_P_Reserve2,
  // subIndex_P_Reserve3,
  subIndex_P_Count
};

// PDO12 - PDO13
enum sudIndexPosition : uint8_t {
  subIndex_Pos_X = 1,
  subIndex_Pos_X2,
  subIndex_Pos_Y,
  subIndex_Pos_Z,
  subIndex_Pos_Count
};

// PDO14 - PDO17
enum sudIndexStepPerMm : uint8_t {
  subIndex_SPM_X = 1,
  subIndex_SPM_Y,
  subIndex_SPM_Z,
  subIndex_SPM_E0,
  subIndex_SPM_E1,
  subIndex_SPM_E2,
  subIndex_SPM_E3,
  subIndex_SPM_Count
};

// PDO18 - PDO21
enum sudIndexAcceleration : uint8_t {
  subIndex_Acc_Print = 1,
  subIndex_Acc_Rectract,
  subIndex_Acc_Travel,
  subIndex_Acc_MaxX,
  subIndex_Acc_MaxY,
  subIndex_Acc_MaxZ,
  subIndex_Acc_MaxE,
  subIndex_Acc_Count
};

// PDO22 - PDO23
// enum sudIndexStepReserve : uint8_t {};

// PDO24 - PDO25
enum sudIndexStatistics : uint8_t {
  subIndex_Stat_PrintsTotal = 1,
  subIndex_Stat_PrintsSuccess,
  subIndex_Stat_PrintsLongest,
  subIndex_Stat_PrintsUsed,
  subIndex_Stat_filamentUsed,
  // subIndex_Stat_Reserve1,
  // subIndex_Stat_Reserve2,
  subIndex_Stat_Count
};

// PDO26 - PDO27
enum sudIndexRegister : uint8_t {
  subIndex_Reg_TraialPeriod = 1,
  subIndex_Reg_RegKey,
  subIndex_Reg_Count,
};

// PDO28 - PDO29
enum sudIndexLeveling : uint8_t {
  subIndex_Level_ZOffset = 1,
  subIndex_Level_FadeHeight,
  // subIndex_Level_Reserve1,
  // subIndex_Level_Reserve2,
  subIndex_Level_Count,
};

// 移轴相关
enum moveIndex : uint8_t { ALL_HOME, X_HOME, Y_HOME, Z_HOME, X_MINUS, Y_MINUS, Z_MINUS, X_PLUS, Y_PLUS, Z_PLUS };
enum moveUnit : uint8_t { MOVE_01MM, MOVE_1MM, MOVE_10MM, MOVE_100MM };

// 打印任务相关
enum taskCmd : uint8_t {
  TASK_PAUSE,       // 暂停打印
  TASK_RESUME,      // 继续打印
  TASK_STOP,        // 停止打印
  TASK_CHANGE,      // 更换耗材
  ACCIDENT_RESUME,  // 断电续打继续
  ACCIDENT_CANCEL,  // 断电续打取消
};

// 屏幕下发指令
enum commonCmd : uint8_t {
  FACTORY_RESET,   // 恢复出厂
  SAVE_SETTING,    // 保存设置
  FIRST_UNPACK,    // 首次开箱  ON, OFF
  REFRESH_FILE,    // 刷新文件  WORKDIR, FILENAME
  MOTOR_SWITCH,    // 电机使能  ON, OFF
  BABYSTEP,        // 平台微调  UP, DOWN
  CASELIGHT,       // 机箱照明  ON, OFF, TOGGLE
  ATUOSHUTDWON,    // 自动关机  ON, OFF, TOGGLE
  ATUOLEVELING,    // 自动调平  ON, OFF, TOGGLE
  CHANGE_TOOL,     // 切换喷头  1, 2, 3, 4
  STOP_OPERATE,    // 取消操作  HOME, MOVE, EXTRUDE, PROBE, SHUTDOWN
  WAIT_HEATING,    // 等待加热  SKIP, AGAIN
  LOAD_FILAMENT,   // 加载耗材  CONTINUE, MORE
  LEVELING_PROBE,  // 校准平台
  USER_CONFIRMED,  // 用户确认
  AUTO_POWEROFF,   // 自动关机
};

enum direction : uint8_t { UP, DOWN, LEFT, RIGHT, FRONT, BACK };
enum switchVal : uint8_t { OFF, ON, TOGGLE = 0xFF };
enum operate : uint8_t { HOME, MOVE, EXTRUDE, PROBE, SHUTDOWN };
enum heating : uint8_t { SKIP, AGAIN };
enum loading : uint8_t { CONTINUE, MORE };
enum fileAct : uint8_t { WORKDIR, FILENAME };

// 主板通知屏幕
enum notifyLCD : uint16_t {
  MEDIA_INSERT,    // U盘已插入
  MEDIA_REMOVE,    // U盘已移除
  MEDIA_ERROR,     // U盘读写异常
  FACTORY_DONE,    // 恢复出厂完成
  SAVE_DONE,       // 保存设置完成
  TASK_DONE,       // 打印任务已完成
  TIMER_START,     // 打印计时器开始
  TIMER_PAUSE,     // 打印计时器暂停
  TIMER_STOP,      // 打印计时器停止
  AXIS_MOVING,     // 移轴中...
  FILA_EXTRUDE,    // 耗材出丝中...
  FILA_RETRACT,    // 耗材回抽中...
  FILA_LOADING,    // 耗材装载中...
  FILA_UNLOADING,  // 耗材卸载中...
  HOME_START,      // 回零开始
  HOME_DONE,       // 回零完成
  LEVELING_START,  // 调平开始
  LEVELING_DONE,   // 调平完成
  FILAMENT_ERROR,  // 耗材异常
  TAR_TEMP_LOW,    // 目标温度过低
  CUR_TEMP_LOW,    // 当前温度过低
  WAITINT_USER,    // 等待用户确认
  HEATER_TIMEOUT,  // 喷头等待超时
  HEATER_HEATING,  // 喷头加热中
  HEATER_FINISH,   // 喷头加热完成
  PRINT_SYNCING,   // 准备暂停打印...
  PRINT_PARKING,   // 喷头停靠中...
  PRINT_PARKED,    // 喷头停靠完成
  PRINT_RESUMING,  // 打印恢复中...
  FILAMENT_INIT,   // 准备更换耗材...
  FILAMENT_DELAY,  // 等待耗材冷却...
  FILAMENT_WAIT,   // 等待插入耗材...
  FILAMENT_PURGE,  // 出丝测试中...
  PURGE_FINISH,    // 出丝完成,继续打印
  PURGE_OPTION,    // 挤出更多|继续打印
  ACTION_FINISH,   // 动作完成
  MAX_TEMP_HIGH,   // 关机温度过高
  POWEROFF_SAFETY, // 安全关机
  OTHER_NOTIFY,    // 其他通知
};

#define ARRIDX(name) (subIndex_##name - 1)

void sendNotify(notifyLCD);
