/**
 *  creatbotFunction.cpp
 *
 *  Created on: 2023/08/25
 *      Author: CreatBot LYN
 *
 *  additional function that can't find in extui.
 */
#include "../../../inc/MarlinConfigPre.h"

#if CREATBOT_LCD
  #include "../../../module/planner.h"
  #include "../ui_api.h"
  #include "creatbotFunction.h"

static feedRate_t axisMoveSpeed = 0;  // 移轴速度
static millis_t   moveStartTime = 0;  // 移轴开始时间

void delayMoveAxis(const_float_t offsetPosition, const ExtUI::axis_t axis) {
  ExtUI::preOffsetAxisPosition_mm(offsetPosition, axis);
  moveStartTime = millis() + 500UL;
  axisMoveSpeed = axisMoveSpeed ? min(axisMoveSpeed, manual_feedrate_mm_s[axis]) : manual_feedrate_mm_s[axis];
}

WEAK void startMoveHook() {}

void creatbotInit() {
  moveStartTime = 0;
  axisMoveSpeed = 0;
}

void creatbotLoop(axisMoveCallback MoveCallback) {
  const millis_t ms = millis();

  // 延时移轴
  if (axisMoveSpeed && ELAPSED(ms, moveStartTime)) {
    MoveCallback();
    line_to_current_position(axisMoveSpeed);
    axisMoveSpeed = 0;
    startMoveHook();
  }
}

  #if ENABLED(SDSUPPORT)
static bool isMediaReady = false;  // U盘是否就绪

void setMediaState(bool isReady) { isMediaReady = isReady; }
bool getMediaState() { return isMediaReady; }

CreatBotFileList::CreatBotFileList() {
  workDir = &card.getWorkDir();
  workDir->rewind();
  ZERO(dirName);
  curFileIndex = -1;
  curFileSize  = -1;
  curFileTime  = -1;
}

void CreatBotFileList::refresh() {
  ExtUI::FileList::refresh();
  curFileIndex = -1;
  curFileSize  = -1;
  curFileTime  = -1;
  workDir->rewind();
}

void CreatBotFileList::nextFile() {
  dir_t p;
  if (card.selectNextValidFile(&p)) {
    curFileIndex++;
    curFileSize = p.fileSize;
    curFileTime = (uint32_t)(p.lastWriteDate << 8) | p.lastWriteTime;
  }
}

uint16_t CreatBotFileList::fileIndex() { return curFileIndex; }

uint32_t CreatBotFileList::fileSize() { return curFileSize; }

uint32_t CreatBotFileList::timeStamp() { return curFileTime; }

void CreatBotFileList::changeDir(const char *const dirname) {
  ExtUI::FileList::changeDir(dirname);
  refresh();
}

void CreatBotFileList::upDir() {
  ExtUI::FileList::upDir();
  refresh();
}
  #endif  // SDSUPPORT

#endif  // CREATBOT_LCD
