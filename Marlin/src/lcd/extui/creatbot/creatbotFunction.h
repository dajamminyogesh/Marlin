/**
 *  creatbotFunction.h
 *
 *  Created on: 2023/08/25
 *      Author: CreatBot LYN
 *
 *  additional function interface that can't find in extui.
 */

#pragma once
#include "../ui_api.h"

typedef void (*axisMoveCallback)(void);

void delayMoveAxis(const_float_t offsetPosition, const ExtUI::axis_t axis);
void startMoveHook();

void creatbotInit();
void creatbotLoop(axisMoveCallback MoveCallback);

#if ENABLED(SDSUPPORT)
void setMediaState(bool);
bool getMediaState();

class CreatBotFileList : public ExtUI::FileList {
 public:
  CreatBotFileList();

  void     refresh();
  void     nextFile();
  uint16_t fileIndex();
  uint32_t fileSize();
  uint32_t timeStamp();

  void changeDir(const char *const dirname);
  void upDir();

 private:
  MediaFile *workDir;
  char       dirName[LONG_FILENAME_LENGTH];
  uint16_t   curFileIndex;
  uint32_t   curFileTime;
  uint32_t   curFileSize;
};
#endif  // SDSUPPORT
