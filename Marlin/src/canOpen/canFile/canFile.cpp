/**
 *  CanFile.cpp
 *
 *  Created on: 2024/01/06
 *      Author: CreatBot LYN
 */

#include "../../inc/MarlinConfigPre.h"

#if ENABLED(CANFILE)
  #include "../../inc/MarlinConfig.h"
  #include "../../module/printcounter.h"
  #include "../canOpen.h"
  #include "../canOpenHelper.h"
  #include "canFile.h"

  #if ENABLED(POWER_LOSS_RECOVERY)
    #include "../../feature/powerloss.h"
  #endif

  #define DEBUG_OUT ENABLED(DEBUG_CANFILE)
  #include "../../core/debug_out.h"

static uint32_t onCanFile(CO_Data *d, const indextable *indexTable, uint8_t subindex) {
  if (indexTable->index == INDEX_CANFILE) {
    switch (subindex) {
      case subIndex_canFileSize: canFile.newFile(CAN_FILE_SIZE); break;
      case subIndex_canFileBuf: canFile.newData(CAN_FILE_BUFFER); break;
      default: break;
    }
  }
  return OD_SUCCESSFUL;
}

// 注册CANFILE相关回调函数
void registerCanFile() { registerODCallBackAll(INDEX_CANFILE, subIndex_canFileCount, &onCanFile); }

// 检测CANFILE的SDO缓存
bool checkSDOCacheCanFile() {
  if (canFile.hasCache()) {
    DEBUG_ECHOLNPGM("reFileTransfer");
    canFile.getData();
    return true;
  }
  return false;
}

// 检测CANFILE的数据状态
void checkCanFileData() { canFile.checkData(); }

/********************** CANFile 类 ************************/

uint32_t      CANFile::fileSize   = 0;
uint32_t      CANFile::blockWrite = 0;
uint32_t      CANFile::blockRead  = 0;
uint8_t       CANFile::fileValid  = 0;
uint16_t      CANFile::fileOffset = 0;
canFileBuf_t  CANFile::fileData   = {0};
canFileFlag_t CANFile::fileFlag   = {0};

millis_t CANFile::commTimeout     = 0;

void CANFile::newFile(uint32_t size) {
  fileInit(size);
  if (size != 0) {
    fileRead();
    TERN_(POWER_LOSS_RECOVERY, recovery.handleCanfile(true);)
    taskStart();
  } else {
    TERN_(POWER_LOSS_RECOVERY, recovery.handleCanfile(false);)
    taskStop();
  }
}

void CANFile::newData(uint8_t *data) {
  fileWrite(data);
  getData();
}

void CANFile::getData() {
  if (fileFlag.closing)  // 正在停止传输
    fileStop();
  else if (fileValid != CANFILE_VALID_MASK)  // 存在脏数据
    fileRead();
}

void CANFile::checkData() {
  if (fileFlag.busy && ELAPSED(millis(), commTimeout)) {
    fileFlag.busy = false;
    DEBUG_ECHOLNPGM("fileRead Timeout, reRead!");
    getData();
  }
}

bool CANFile::hasCache() { return fileFlag.cached; }

void CANFile::fileClear() {
  fileSize    = 0;
  blockWrite  = 0;
  blockRead   = 0;
  fileValid   = 0;
  fileOffset  = 0;
  fileData    = {0};
  fileFlag    = {false};
  commTimeout = 0;
}

void CANFile::fileInit(uint32_t newSize) {
  DEBUG_ECHOLNPGM("fileInit size: ", newSize);
  fileClear();
  if (newSize != 0) {  // 大小为0表示取消断电续打
    fileSize = newSize;
  #if ENABLED(POWER_LOSS_RECOVERY)
    if (recovery.state) {
      blockWrite = recovery.info.canfilepos / CANFILE_BLOCK_SIZE;
      blockRead  = recovery.info.canfilepos / CANFILE_BLOCK_SIZE;
      fileOffset = recovery.info.canfilepos % CANFILE_BUFFER_SIZE;
    }
  #endif  // POWER_LOSS_RECOVERY
  }
}

void CANFile::fileWrite(uint8_t *newData) {
  if (!fileFlag.busy) return;

  DEBUG_ECHOPGM("fileWrite -> BlockIndex is: ", blockWrite);
  uint8_t writeIndex = blockWrite++ % CANFILE_BLOCK_NUM;
  fileValid |= (1 << writeIndex);  // 当前写入的块数据已经收到, 数据为最新有效数据
  memcpy(fileData.subData[writeIndex], newData, CANFILE_BLOCK_SIZE);  // 存储新数据

  fileFlag.opened = true;
  fileFlag.busy   = false;
  commTimeout     = 0;
  DEBUG_ECHOLNPGM(" <- Done.");
}

void CANFile::fileRead() {
  if (fileFlag.busy) return;
  if ((blockWrite * CANFILE_BLOCK_SIZE) >= fileSize) {  // 文件传输完成
    if (fileFlag.opened) fileStop();
  } else {  // 正常读取
    DEBUG_ECHOPGM("fileRead  -> BlockIndex is: ", blockWrite);
    if (sendIndexToCanFile(blockWrite, fisishSDO) == 0x00) {  // 发送缓存通知
      fileFlag.busy   = true;
      fileFlag.cached = false;
      commTimeout     = millis() + (SDO_TIMEOUT_MS / 2);
      DEBUG_ECHOLNPGM(" <- Done.");
    } else {  // 可能发送失败
      fileFlag.cached = true;
      DEBUG_ECHOLNPGM(" <- Cached!");
    }
  }
}

void CANFile::fileStop() {
  if (fileFlag.busy) {
    fileFlag.closing = true;
  } else {
    DEBUG_ECHOPGM("fileStop -> BlockIndex is: ", blockWrite);
    uint32_t blockEol = UINT32_MAX;
    if (sendIndexToCanFile(blockEol, fisishSDO) == 0x00) {  // 发送完成通知
      fileFlag.cached  = false;
      fileFlag.closing = false;
      fileFlag.opened  = false;
      if ((blockWrite * CANFILE_BLOCK_SIZE) >= fileSize) {
        DEBUG_ECHOLNPGM(" <- Finished.");
      } else {
        DEBUG_ECHOLNPGM(" <- Abort.");
      }
    } else {  // 可能发送失败
      fileFlag.cached  = true;
      fileFlag.closing = true;
      DEBUG_ECHOLNPGM(" <- Cached!");
    }
  }
}

bool CANFile::get(uint8_t &ch) {
  if (fileValid && (pos() < fileSize)) {  // 数据有效,且字节索引有效
    ch = fileData.data[fileOffset++];

    fileOffset %= CANFILE_BUFFER_SIZE;

    if (!(fileOffset % CANFILE_BLOCK_SIZE)) {  // 数据偏移要求切换块
      uint8_t readIndex = blockRead++ % CANFILE_BLOCK_NUM;
      fileValid &= ~(1 << readIndex);  // 当前读取的块已经完成读取, 数据为过期脏数据
      getData();                       // 获取新数据
    }
    return true;
  } else {
    return false;
  }
}

uint32_t CANFile::pos() {
  // 读索引 * 块大小 + 块内偏移位置
  return (blockRead * CANFILE_BLOCK_SIZE) + (fileOffset % CANFILE_BLOCK_SIZE);
}

  #if HAS_PRINT_PROGRESS_PERMYRIAD
uint16_t CANFile::permyriadDone() {
  if (isOpened() && fileSize)
    return pos() * 10000.0 / fileSize;
  else
    return 0;
}
  #endif

uint8_t CANFile::percentDone() {
  if (isOpened() && fileSize)
    return pos() * 100.0 / fileSize;
  else
    return 0;
}

void CANFile::taskStart() {
  fileFlag.started  = true;
  fileFlag.printing = true;
  print_job_timer.start();
}

void CANFile::taskPause() {
  fileFlag.started  = true;
  fileFlag.printing = false;
  print_job_timer.pause();
}

void CANFile::taskStop() {
  fileFlag.started   = false;
  fileFlag.printing  = false;
  fileFlag.aborting  = false;
  fileFlag.finishing = false;
}

void CANFile::taskFinishing() {
  fileValid          = 0;
  fileFlag.finishing = true;
}

void CANFile::taskFinished() {
  fileValid          = 0;
  fileFlag.finishing = false;
}

void CANFile::taskAborting() {
  fileValid         = 0;
  fileFlag.aborting = true;
  fileStop();
}

void CANFile::taskAborted() {
  fileValid         = 0;
  fileFlag.aborting = false;
  taskStop();
}

bool CANFile::isOpened() { return (fileFlag.started); }
bool CANFile::isPrinting() { return (fileFlag.started && fileFlag.printing); }
bool CANFile::isPaused() { return (fileFlag.started && !fileFlag.printing); }
bool CANFile::isFetching() { return (fileValid && isPrinting()); }

#endif