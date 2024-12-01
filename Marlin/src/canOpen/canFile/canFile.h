/**
 *  CanFile.h
 *
 *  Created on: 2024/01/06
 *      Author: CreatBot LYN
 */
#pragma once

#include "../../inc/MarlinConfigPre.h"

// #define DEBUG_CANFILE

#if ENABLED(CANFILE)

  #include "../../src/core/utility.h"

  #define CANFILE_BLOCK_NUM   4    // CANFILE 缓冲区块个数
  #define CANFILE_BLOCK_SIZE  256  // CANFILE 缓冲区块大小

  #define CANFILE_BUFFER_SIZE (CANFILE_BLOCK_SIZE * CANFILE_BLOCK_NUM)  // CANFILE 缓冲区总大小
  #define CANFILE_VALID_MASK  ((1 << CANFILE_BLOCK_NUM) - 1)            // CANFILE 缓冲区有效性掩码

// SDO (0x2007)
enum subIndexCanFile : uint8_t {
  subIndex_canFileSize = 1,
  subIndex_canFileBuf,
  subIndex_canFileCount,
};

typedef union {
  uint8_t data[CANFILE_BUFFER_SIZE];
  uint8_t subData[CANFILE_BLOCK_NUM][CANFILE_BLOCK_SIZE];
} canFileBuf_t;

typedef struct {
  bool busy      : 1;  // 文件传输 传输中
  bool opened    : 1;  // 文件传输 已经开始
  bool closing   : 1;  // 文件传输 正在中止
  bool cached    : 1;  // 文件传输 索引被缓存
  bool started   : 1;  // 打印任务 已启动, 文件已经被读取
  bool printing  : 1;  // 打印任务 打印中, 文件正在被读取
  bool aborting  : 1;  // 打印任务 取消中, 文件传输正在中止
  bool finishing : 1;  // 打印任务 完成中, 文件准备关闭
} canFileFlag_t;

class CANFile {
 public:
  CANFile() { fileClear(); }

  // 传输相关
  static void newFile(uint32_t size);
  static void newData(uint8_t *data);
  static void getData();
  static void checkData();
  static bool hasCache();

  // 打印相关
  static bool eof() { return pos() >= fileSize; }  // 是否到达文件结尾
  static bool get(uint8_t &ch);                    // 获取下一个字符, 返回操作是否成功

  static uint32_t pos();  // 获取当前文件位置
  #if HAS_PRINT_PROGRESS_PERMYRIAD
  static uint16_t permyriadDone();
  #endif
  static uint8_t percentDone();

  static void taskStart();
  static void taskPause();
  static void taskStop();
  static void taskFinishing();
  static void taskFinished();
  static void taskAborting();
  static void taskAborted();

  static void fileTaskStop() { fileStop(); }

  static bool isFinishing() { return fileFlag.finishing; }
  static bool isAborting() { return fileFlag.aborting; }

  static bool isOpened();
  static bool isPrinting();
  static bool isPaused();
  static bool isFetching();

 private:
  static void fileInit(uint32_t newSize);
  static void fileWrite(uint8_t *newData);
  static void fileClear();
  static void fileRead();
  static void fileStop();

  static uint32_t      fileSize;    // 文件总大小
  static uint32_t      blockWrite;  // 文件块写索引
  static uint32_t      blockRead;   // 文件块读索引
  static uint8_t       fileValid;   // 文件数据有效性
  static uint16_t      fileOffset;  // 文件数据偏移
  static canFileBuf_t  fileData;    // 文件数据内容
  static canFileFlag_t fileFlag;    // 文件状态标志

  static millis_t commTimeout;  // 数据传输超时
};

  #define IS_CANFILE_PRINTING() canFile.isPrinting()
  #define IS_CANFILE_PAUSED()   canFile.isPaused()
  #define IS_CANFILE_FETCHING() canFile.isFetching()
  #define IS_CANFILE_OPEN()     canFile.isOpened()

extern CANFile canFile;

#else

  #define IS_CANFILE_PRINTING() false
  #define IS_CANFILE_PAUSED()   false
  #define IS_CANFILE_FETCHING() false
  #define IS_CANFILE_OPEN()     false

#endif  // CANFILE