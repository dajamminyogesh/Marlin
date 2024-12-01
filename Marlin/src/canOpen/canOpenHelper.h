/**
 *  canOpenHelper.h
 *
 *  Created on: 2023/08/25
 *      Author: CreatBot LYN
 *
 *  helper for linux lcd's canOpen.
 */
#pragma once

#include "../inc/MarlinConfigPre.h"
#include "CreatBotOD.h"
#include "canOpen.h"

#if HAS_EXTRUDERS
  #define HAS_EXTRUDER0
  #if EXTRUDERS > 1
    #define HAS_EXTRUDER1
    #if EXTRUDERS > 2
      #define HAS_EXTRUDER2
      #if EXTRUDERS > 3
        #define HAS_EXTRUDER3
      #endif
    #endif
  #endif
#endif

#if HAS_HOTEND
  #define HAS_HOTEND0
  #if HOTENDS > 1
    #define HAS_HOTEND1
    #if HOTENDS > 2
      #define HAS_HOTEND2
      #if HOTENDS > 3
        #define HAS_HOTEND3
      #endif
    #endif
  #endif
#endif

#if HAS_AUTO_FAN && ENABLED(ADJUST_EXTRUDER_AUTO_FAN)
  #define AUTO_FAN_CAN_ADJUST
#endif

#if DISABLED(HAS_LEVELING)
  #define DISABLE_LEVELING
#endif

// 注册单个回调
inline static void registerODCallBackOne(uint16_t wIndex, uint8_t bSubindex, ODCallback_t callback) {
  RegisterSetODentryCallBack(&CreatBotOD_Data, wIndex, bSubindex, callback);
}

// 注册一组回调
inline static void registerODCallBackAll(uint16_t wIndex, uint8_t bCount, ODCallback_t callback) {
  for (uint8_t i = 1; i < bCount; i++) {
    RegisterSetODentryCallBack(&CreatBotOD_Data, wIndex, i, callback);
  }
}

inline static void enablePDO(uint8_t pdoNum) { PDOEnable(&CreatBotOD_Data, pdoNum); }
inline static void disablePDO(uint8_t pdoNum) { PDODisable(&CreatBotOD_Data, pdoNum); }

#if ENABLED(CANOPEN_LCD)
// 禁用屏幕的PDO
inline static void PDODisableLCD() {
  for (uint8_t i = LCD_PDO_MIN_INDEX; i <= LCD_PDO_MAX_INDEX; i++) {
    disablePDO(i);
  }
}

// 启用屏幕的PDO
inline static void PDOEnableLCD() {
  for (uint8_t i = LCD_PDO_MIN_INDEX; i <= LCD_PDO_MAX_INDEX; i++) {
    enablePDO(i);
  }
}

// 发送数据到文件名缓冲区
inline static uint8_t sendFileInfoToLCD(uint32_t len, void *data, SDOCallback_t Callback, uint8_t useBlockMode) {
  return writeNetworkDictCallBack(&CreatBotOD_Data, LCD_NODE_ID, INDEX_FILE_BUFFER, 0x00,  // 屏幕文件缓存
                                  len, CanOpen_visible_string, data, Callback, useBlockMode);
}

// 发送通知索引到屏幕
inline static uint8_t sendNotifyToLCD(uint16_t &data, SDOCallback_t Callback) {
  return writeNetworkDictCallBack(&CreatBotOD_Data, LCD_NODE_ID, INDEX_NOTIFY_OUTPUT, 0x00,  // 屏幕通知缓存
                                  2, 0, &data, Callback, 0);
}
#endif  // CANOPEN_LCD

#if ENABLED(CANFILE)
// 发送索引到CANFILE主机
inline static uint8_t sendIndexToCanFile(u_int32_t &index, SDOCallback_t Callback) {
  return writeNetworkDictCallBack(&CreatBotOD_Data, CANFILE_HOST_NODE_ID, INDEX_CANFILE_BLOCKID, 0x00,  // CANFILE块索引
                                  4, 0, &index, Callback, 0);
}
#endif  // CANFILE

void fisishSDO(CO_Data *, uint8_t);
