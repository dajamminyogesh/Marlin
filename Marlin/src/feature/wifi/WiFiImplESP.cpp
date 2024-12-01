#include "../../inc/MarlinConfigPre.h"
#if ENABLED(CREATBOT_WIFI_SUPPORT)
  #include "../../inc/MarlinConfig.h"
  #include "WiFiImplESP.h"

WiFiImplESP WiFiESPModule;

  #ifdef CREATBOT_WIFI_SUPPORT
    #define WiFi_Serial MSERIAL(WiFi_SERIAL_PORT)
  #endif

void WiFiImplESP::Init() {
  #ifndef CREATBOT_WIFI_BAUDRATE
    #define CREATBOT_WIFI_BAUDRATE 115200
  #endif
  WiFi_Serial.begin(CREATBOT_WIFI_BAUDRATE);
  wifi_buffer = {{0}, 0, 0};
  wifiMSQ     = {0, {0}};
  WIFIBUGLN("WiFi_start!\n");
}

void WiFiImplESP::loop() { readMsgByFrame(); }

void WiFiImplESP::readMsgByFrame() {
  static uint8_t  recvState   = GetFH;
  static uint16_t recvIndex   = 0;
  static uint16_t recvDataLen = 0;

  if (wifiMSQ.valid) return;

  if (recvState == RevcDone) {  // the next receive.
    recvState = GetFH;
    memset(&wifiMSQ, 0, sizeof(wifi_frame_msg));
  }
  while (WiFi_Serial.available() > 0) {
    uint8_t c = WiFi_Serial.read();
    if (recvState == GetFH) {
      if (c == WIFI_MSG_FH) {
        recvState   = GetLenH;
        recvIndex   = 0;
        recvDataLen = 0;
      }
    } else if (recvState == GetLenH) {
      recvDataLen |= c << 8;
      recvState = GetLenL;
    } else if (recvState == GetLenL) {
      recvDataLen |= c;
      recvState = GetFT;
    } else if (recvState == GetFT) {
      if (c == WIFI_MSG_FT && recvIndex == recvDataLen) {
        recvState     = RevcDone;
        wifiMSQ.valid = true;
        break;
      } else if (c == WIFI_MSG_FT && recvIndex > recvDataLen) {  // 接收失败 重新接收
        recvState   = GetFH;
        recvIndex   = 0;
        recvDataLen = 0;
      } else {
        if (recvIndex >= WIFI_BUF_LEN) {
          recvState = GetFH;
        } else {
          wifiMSQ.msg[recvIndex++] = c;
        }
      }
    }
  }
}

void WiFiImplESP::SendData(const uint8_t value) { WiFi_Serial.write(value); }

void WiFiImplESP::SendData(const uint8_t *value, uint16_t Len) { WiFi_Serial.write(value, size_t(Len)); }

void WiFiImplESP::SendData(const char *str) { WiFi_Serial.print(str); }

void WiFiImplESP::SendData(const String str) { WiFi_Serial.print(str.c_str()); }

int WiFiImplESP::unicode_to_utf8(uint16_t *in, int insize) {
  int   i           = 0;
  int   outsize     = 0;
  int   charscount  = 0;
  char  result[150] = "0";
  char *tmp         = NULL;

  charscount        = insize / sizeof(uint16_t);
  tmp               = result;

  for (i = 0; i < charscount; i++) {
    uint16_t unicode = in[i];

    if (unicode >= 0x0000 && unicode <= 0x007f) {
      *tmp = (char)unicode;
      tmp += 1;
      outsize += 1;
    } else if (unicode >= 0x0080 && unicode <= 0x07ff) {
      *tmp = 0xc0 | (unicode >> 6);
      tmp += 1;
      *tmp = 0x80 | (unicode & (0xff >> 2));
      tmp += 1;
      outsize += 2;
    } else if (unicode >= 0x0800 && unicode <= 0xffff) {
      *tmp = 0xe0 | (unicode >> 12);
      tmp += 1;
      *tmp = 0x80 | (unicode >> 6 & 0x003f);
      tmp += 1;
      *tmp = 0x80 | (unicode & (0xff >> 2));
      tmp += 1;
      outsize += 3;
    }
  }

  *tmp = '\0';
  strcpy((char *)in, result);
  return 0;
}

#endif  // CREATBOT_WIFI_SUPPORT
