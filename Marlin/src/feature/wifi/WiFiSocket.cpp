#include "../../inc/MarlinConfigPre.h"

#if ENABLED(CREATBOT_WIFI_SUPPORT)
  #include "../../inc/MarlinConfig.h"

  #include "../../../src/lcd/extui/creatbot/dgus/DGUSDisplayDef.h"
  #include "../../../src/lcd/extui/creatbot/dgus/DGUSScreenHandler.h"
  #include "../../../src/lcd/extui/ui_api.h"
  #include "../../../src/module/temperature.h"
  #include "WiFiImplESP.h"
  #include "WiFiSocket.h"

using namespace ExtUI;

WIFI_ESP myWifi;

// wifi_frame_msg wifiMSQ = { false, {0} };

void WIFI_ESP::init() {
  WiFiESPModule.Init();
  resetConnState();
  // getStationInfo();
}

void WIFI_ESP::run() {
  WiFiESPModule.loop();
  processMsg();
}

void WIFI_ESP::resetConnState() {
  wifiState   = WifiNoConnect;
  cameraState = CameraDisable;
  cloudState  = CloudDisable;
  memset(connSSID, 0, sizeof(connSSID));
  memset(connKEY, 0, sizeof(connKEY));
  ScreenHandler.UpdateWiFiInfo(connSSID, connKEY);  // TODO need remove
}

void WIFI_ESP::processMsg() {
  static millis_t timeout     = millis() + CONNECT_TIMEOUT;
  static bool     moduleExist = false;
  if (moduleExist && ELAPSED(millis(), timeout)) {
    timeout     = millis() + CONNECT_TIMEOUT;
    moduleExist = false;
    resetConnState();
  }

  if (!WiFiESPModule.wifiMSQ.valid) return;
  timeout = millis() + CONNECT_TIMEOUT;
  if (!moduleExist) moduleExist = true;
  #if defined(WIFI_DEBUG) || defined(WIFI_IMPL_DEBUG)
  SERIAL_ECHOLNPAIR("<<<:", wifiMSQ.msg);
  #endif
  char *msgPtr  = nullptr;
  char *tempPtr = nullptr;
  msgPtr        = strtok_r(WiFiESPModule.wifiMSQ.msg, ",", &tempPtr);

  if (!strcmp_P(msgPtr, PSTR(MSG_SYN))) {
    msgPtr      = strtok_r(NULL, ",", &tempPtr);  // the wifiState
    wifiState   = (WifiState)atoi(msgPtr);
    msgPtr      = strtok_r(NULL, ",", &tempPtr);  // camera opened
    cameraState = (CameraState)(atoi(msgPtr));
    msgPtr      = strtok_r(NULL, ",", &tempPtr);  // remote cloud server is connected
    cloudState  = (CloudState)(atoi(msgPtr));
    synPrinterInfo();
  } else if (!strcmp_P(msgPtr, PSTR(MSG_GCO))) {
    msgPtr = strtok_r(NULL, ",", &tempPtr);  // the GCO Code
    ExtUI::injectCommands(msgPtr);
    char msg[8] = {0};
    sprintf_P(msg, PSTR(MSG_GCO "," MSG_CONFIRM));
    sendMsg(msg);
  } else if (!strcmp_P(msgPtr, PSTR(MSG_CMD))) {
    msgPtr = strtok_r(NULL, ",", &tempPtr);  // the CMD Code

    if (!strcmp_P(msgPtr, PSTR(CMD_RESUME))) {
      if (ExtUI::isPrintingFromMediaPaused()) ExtUI::resumePrint();
    } else if (!strcmp_P(msgPtr, PSTR(CMD_PAUSE))) {
      if (ExtUI::isPrintingFromMedia() && !ExtUI::isPrintingFromMediaPaused()) ExtUI::pausePrint();
    } else if (!strcmp_P(msgPtr, PSTR(CMD_CANCEL))) {
      if (ExtUI::isPrintingFromMedia()) ExtUI::stopPrint();
    } else if (!strcmp_P(msgPtr, PSTR(CMD_FILE))) {
      msgPtr = strtok_r(NULL, ",", &tempPtr);  // the file index
      ScreenHandler.HandleFileItemAction(atoi(msgPtr));
    } else {
  #if defined(WIFI_DEBUG) || defined(WIFI_IMPL_DEBUG)
      SERIAL_ECHOPGM_P("Unknown WIFI CMD command: ", msgPtr);
  #endif
    }

    char msg[8] = {0};
    sprintf_P(msg, PSTR(MSG_CMD "," MSG_CONFIRM));
    sendMsg(msg);
  } else if (!strcmp_P(msgPtr, PSTR(MSG_SSID))) {
    strcpy(connSSID, tempPtr);
  } else if (!strcmp_P(msgPtr, PSTR(MSG_KEY))) {
    strcpy(connKEY, tempPtr);
    ScreenHandler.UpdateWiFiInfo(connSSID, connKEY);  // TODO need remove
  } else if (!strcmp_P(msgPtr, PSTR(MSG_UUID))) {
    strcpy(uuid, strtok_r(NULL, ",", &tempPtr));
    ScreenHandler.UpdateWiFiUUID(uuid);
  } else if (!strcmp_P(msgPtr, PSTR(MSG_VIEW))) {
    if (!ExtUI::isPrinting()) {
      msgPtr = strtok_r(NULL, ",", &tempPtr);  // the view mode
      switch (*msgPtr) {
        case '0': break;
        case '1': ScreenHandler.HandleFileViewAction(1); break;
        case '2': ScreenHandler.HandleFileViewAction(2); break;
        case '3': ScreenHandler.HandleFileViewAction(3); break;
        default: WiFiESPModule.wifiMSQ.valid = false; return;
      }
      actionSynFilesInfo();
    }
  } else {
  #if defined(WIFI_DEBUG) || defined(WIFI_IMPL_DEBUG)
    SERIAL_ECHOLN("Invalid message");
  #endif
  }
  WiFiESPModule.wifiMSQ.valid = false;
}

void WIFI_ESP::sendMsg(const char *msg) {
  #if defined(WIFI_DEBUG) || defined(WIFI_IMPL_DEBUG)
  SERIAL_ECHOPGM_P(">>>:", msg);
  #endif
  uint16_t msgLen = strlen(msg);
  WiFiESPModule.SendData((uint8_t)WIFI_MSG_FH);
  WiFiESPModule.SendData((msgLen >> 8) & 0xFF);
  WiFiESPModule.SendData(msgLen & 0xFF);
  WiFiESPModule.SendData(msg);
  WiFiESPModule.SendData((uint8_t)WIFI_MSG_FT);
}

void WIFI_ESP::actionSynPrinterInfo() { synPrinterInfo(); }

void WIFI_ESP::actionSynFilesInfo() {
  char fileNameList[1024] = "";
  for (uint8_t i = 0; i < LCD_FILE_PER_PAGE; i++) {
    getFilenameInfo(i, fileNameList);
  }
  char jointMsg[strlen(fileNameList) + 20] = "";
  sprintf_P(jointMsg, "%s,%s,", PSTR(MSG_PUSH), PSTR(MSG_VIEW));
  endSysFile(jointMsg);
  strcat(jointMsg, fileNameList);
  sendMsg(jointMsg);
}

void WIFI_ESP::actionJoinAP() {
  char msg[40] = {0};
  sprintf_P(msg, PSTR(MSG_SSID ",%s"), connSSID);
  sendMsg(msg);
  safe_delay(5);
  sprintf_P(msg, PSTR(MSG_KEY ",%s"), connKEY);
  sendMsg(msg);
}

void WIFI_ESP::getStationInfo() {
  char msg[6] = {0};
  sprintf_P(msg, PSTR(MSG_SSID "?"));
  sendMsg(msg);
  safe_delay(5);
  sprintf_P(msg, PSTR(MSG_KEY "?"));
  sendMsg(msg);
}

void WIFI_ESP::synPrinterInfo() {
  char Z_buf[8] = {0}, P_buf[8] = {0}, msg[64] = {0};
  dtostrf(current_position[Z_AXIS], 3, 2, Z_buf);
  #if ENABLED(SDSUPPORT)
  auto getPercentF = []() -> float {
    return TERN(HAS_PRINT_PROGRESS_PERMYRIAD, ExtUI::getProgress_permyriad() / 100.0, ExtUI::getProgress_percent());
  };
  dtostrf(getPercentF(), 3, 2, P_buf);
  #else   //! SDSUPPORT
  strcpy_P(P_buf, PSTR("0.00"));
  #endif  // SDSUPPORT

  uint8_t state  = 0;
  bool    insert = 0;
  #if ENABLED(SDSUPPORT)
  if (ExtUI::isPrintingFromSerial())
    state = 3;
  else if (ExtUI::isPrintingFromMedia())
    if (!ExtUI::isPrintingFromMediaPaused())
      state = 1;
    else
      state = 2;
  else
    state = 0;
  insert = ExtUI::isMediaInserted();
  #else   //! SDSUPPORT
  if (ExtUI::isPrinting())
    state = 3;
  else
    state = 0;
  insert = 0;
  #endif  // SDSUPPORT

  sprintf_P(msg, PSTR(MSG_SYN ",%d,%d,%d,%d,%d,%d,%d,%d,%s,%s,%d,%d"), (int)(thermalManager.wholeDegHotend(0)),
  #if HOTENDS > 1
            (int)thermalManager.wholeDegHotend(1),
  #else
            -1,
  #endif
            (int)thermalManager.wholeDegBed(),
  #ifdef HAS_HEATED_CHAMBER
            (int)thermalManager.wholeDegChamber(),
  #else
            -1,
  #endif
            (int)thermalManager.degTargetHotend(0),
  #if HOTENDS > 1
            (int)thermalManager.degTargetHotend(1),
  #else
            -1,
  #endif
            (int)thermalManager.degTargetBed(),
  #ifdef HAS_HEATED_CHAMBER
            (int)thermalManager.degTargetChamber(),
  #else
            -1,
  #endif
            Z_buf, P_buf, (int)state, (int)insert);
  sendMsg(msg);
}

// 获取单个文件名
void WIFI_ESP::getFilenameInfo(uint8_t target_line, char *strbuf) {
  char    buffer[LONG_FILENAME_LENGTH] = {0};
  uint8_t isdir;
  if (ScreenHandler.HandleGetFileNameInfo(target_line, &isdir, buffer)) {  // 得到文件名称
    WiFiESPModule.unicode_to_utf8((uint16_t *)buffer, LONG_FILENAME_LENGTH);
  }
  char jointStr[strlen(buffer) + 2] = "";
  if (isdir == 2) {  // 判断是否文件夹
    jointStr[0] = '<';
    strcat(jointStr, buffer);
    strcat(jointStr, PSTR(">"));
  } else if (isdir == 1) {
    jointStr[0] = ':';
    strcat(jointStr, buffer);
    strcat(jointStr, PSTR(":"));
  } else {  // 没有文件
    return;
  }
  strcat(strbuf, jointStr);
}

void WIFI_ESP::endSysFile(char *str) {  // 获取下一页 上一页 上一层选项
  // VIEW,DONE,isLastLev,isLastPage,isNextPage    -->    10 + 5
  char status[8] = "";
  ScreenHandler.UpdateFilelistStatus(status);
  strcat(str, status);
}

void WIFI_ESP::resetWifiInfo() {
  char msg[37] = {0};
  sprintf_P(msg, PSTR("RST,%s"), CUSTOM_MACHINE_NAME);
  sendMsg(msg);
  resetConnState();
}

void WIFI_ESP::dealySYNC() { sendMsg(MSG_DELAY_SYNC); }

// void WIFI_ESP::setHostName(){
//   //TODO
// }

#endif  // CREATBOT_WIFI_SUPPORT