#pragma once

#define WIFI_DEBUG_ENABLE true

#define WIFI_BUF_LEN      1024
#define WIFI_MSG_FH       0xA0
#define WIFI_MSG_FT       0xA1

enum recv_state { GetFH, GetLenH, GetLenL, GetFT, RevcDone };

struct wifi_ring_buffer {
  uint8_t buffer[WIFI_BUF_LEN];
  uint8_t head;
  uint8_t tail;
};

struct wifi_frame_msg {
  bool valid;
  char msg[WIFI_BUF_LEN];
};

class WiFiImplESP {
 private:
  /* data */
 public:
  wifi_ring_buffer wifi_buffer;
  wifi_frame_msg   wifiMSQ;

  void Init();
  void loop();
  void readMsgByFrame();
  void SendData(const uint8_t);
  void SendData(const uint8_t *, uint16_t);
  void SendData(const char *);
  void SendData(const String);
  int  unicode_to_utf8(uint16_t *, int);
};

extern WiFiImplESP WiFiESPModule;

#if WIFI_DEBUG_ENABLE
  #define WIFIDEBUG(x)     WiFi_Serial.print(x)
  #define WIFIBUGLN(x)     WiFi_Serial.println(x)
  #define WIFIBUG_HEX(x)   WiFi_Serial.print(x, HEX)
  #define WIFIBUGLN_HEX(x) WiFi_Serial.println(x, HEX)
  #define WIFIBUGEOL       WiFi_Serial.println()
#else
  #define WIFIBUG(x)
  #define WIFIBUGLN(x)
  #define WIFIBUG_HEX(x)
  #define WIFIBUGLN_HEX(x)
  #define WIFIBUGEOL
#endif
