#pragma once

#define MSG_ENQ        "ENQ"  // 查询打印机
#define MSG_ACK        "ACK"  // 打印机应答
#define MSG_BEL        "BEL"  // 连接打印机
#define MSG_SYN        "SYN"  // 同步打印机信息
#define MSG_TAB        "TAB"  // 切换控制权限
#define MSG_GCO        "GCO"  // GCODE代码发送
#define MSG_CMD        "CMD"  // 任务指令发送
#define MSG_CONFIRM    "OK"   // 指令确认

#define MSG_SSID       "SSID"
#define MSG_KEY        "KEY"
#define MSG_UUID       "UUID"

#define CMD_RESUME     "RESUME"
#define CMD_PAUSE      "PAUSE"
#define CMD_CANCEL     "CANCEL"
#define CMD_FILE       "FILE"

#define MSG_PUSH       "PUSH"
#define MSG_VIEW       "VIEW"
#define MSG_VIEW_SEND  "SEND"
#define MSG_VIEW_DONE  "DONE"
#define MSG_DELAY_SYNC "DELAY"

#define SCAN_MASK      0x07
#define WIFI_LINK_NUM  5
#define WIFI_SSID_SIZE 32 + 1
#define WIFI_KEY_SIZE  32 + 1
#define WIFI_UUID_SIZE 15 + 1

enum WifiState { WifiNoConnect, WifiRssi1, WifiRssi2, WifiRssi3, WifiRssi4, WifiConnecting, WifiSsidNoFind, WifiKeyErr };
enum CloudState { CloudDisable, CloudNoConnect, CloudConected };
enum CameraState { CameraDisable, CameraClosed, CameraOpened };

#define CONNECT_TIMEOUT 5000

class WIFI_ESP {
 public:
  bool enable;  // If the WiFi is Enable
  char connSSID[WIFI_SSID_SIZE];
  char connKEY[WIFI_KEY_SIZE];
  char uuid[WIFI_UUID_SIZE];
  void init();
  void run();

  bool isOpen() { return true; }

  uint8_t  getWifiState() { return wifiState; }
  uint8_t  getCameraState() { return cameraState; }
  uint8_t  getCloudState() { return cloudState; }
  uint32_t getIP() { return IP_STA; }

  void getFilenameInfo(uint8_t target_line, char *strbuf);
  void endSysFile(char *);

  void actionSynPrinterInfo();
  void actionSynFilesInfo();

  void getStationInfo();
  void actionJoinAP();
  void resetWifiInfo();
  void dealySYNC();

 private:
  uint32_t    IP_STA;  //  the IP ADDR of station
  WifiState   wifiState;
  CloudState  cloudState;
  CameraState cameraState;

  void resetConnState();
  void sendMsg(const char *msg);
  void processMsg();
  void synPrinterInfo();
  void setHostName();
};

extern WIFI_ESP myWifi;
