/**
 *  canOpen.h
 *
 *  Created on: 2023/07/28
 *      Author: CreatBot LYN
 *
 *  Protocol layer for canOpen
 */
#pragma once

// #define DEBUG_CANOPEN

#define CANOPEN_BAUDRATE 500000

#define PDO(num)         (num - 1)

#ifdef CANOPEN_LCD
  #define LCD_NODE_ID     1  // 屏幕节点ID
  #define HEART_INDEX_LCD 0  // 消费者心跳索引(屏幕)
#endif

#ifdef CANOPEN_MINIBOARD
  #define LCD_NODE_ID     2  // 机头板节点ID
  #define HEART_INDEX_LCD 1  // 消费者心跳索引(机头板)
#endif

#ifdef CANFILE
  #if BOTH(CANFILE_HOST_IS_LCD, CANOPEN_LCD)
    #define CANFILE_HOST_NODE_ID LCD_NODE_ID  // CANFILE主机节点
  #endif

  #ifndef CANFILE_HOST_NODE_ID
    #error "CANFILE need a node as file host."
  #endif
#endif

/******************* Object Index (SDO Server) *******************/
#define INDEX_MACHINE_INFO 0x2000  // 机器信息
#define INDEX_CMD_TASK     0x2001  // 指令 - 任务
#define INDEX_CMD_MOVE     0x2002  // 指令 - 移轴
#define INDEX_CMD_EXTRUDE  0x2003  // 指令 - 送丝
#define INDEX_CMD_COMMON   0x2004  // 指令 - 其他
#define INDEX_FILE_INDEX   0x2005  // 选择文件索引   SDSUPPORT
#define INDEX_GCODE_INPUT  0x2006  // 手动Gcode输入
#define INDEX_CANFILE      0x2007  // 传输CAN文件    CANFILE
#define INDEX_MAXTEMP      0x2100  // 最大温度限制
/*****************************************************************/

/******************* Object Index (SDO Client) *******************/
#ifdef CANOPEN_LCD
  #define INDEX_FILE_BUFFER   0x2200  // 屏幕文件缓存,用于发送文件名
  #define INDEX_NOTIFY_OUTPUT 0x2300  // 屏幕通知缓存,用于发送实时通知
  #define INDEX_MSG_OUTPUT    0x2301  // 屏幕消息缓存,用于发送字符消息
#endif

#ifdef CANFILE
  #define INDEX_CANFILE_BLOCKID 0x2400  // 文件块索引,用于下载CANFILE数据
#endif
/*****************************************************************/

/********************** Object Index (PDO) ***********************/
#define INDEX_GLOBAL 0x3000  // PDO1

#ifdef CANOPEN_LCD
  #define LCD_PDO_MIN_INDEX  1   // PDO2
  #define LCD_PDO_MAX_INDEX  28  // PDO29

  #define INDEX_CURTEMP      0x3001  // PDO2 - PDO4
  #define INDEX_TARTEMP      0x3002  // PDO5 - PDO7
  #define INDEX_TASKINFO     0x3003  // PDO8 - PDO10
  #define INDEX_PERCNET      0x3004  // PDO11 - PDO11
  #define INDEX_POSITION     0x3005  // PDO12 - PDO13
  #define INDEX_STEPPERMM    0x3006  // PDO14 - PDO17
  #define INDEX_ACCELERATION 0x3007  // PDO18 - PDO21
  #define INDEX_STEPRESERVE  0x3008  // PDO22 - PDO23
  #define INDEX_STATISTICS   0x3009  // PDO24 - PDO25
  #define INDEX_REGISTER     0x300A  // PDO26 - PDO27
  #define INDEX_LEVELING     0x300B  // PDO28 - PDO29
#endif
/*****************************************************************/

namespace canOpen {
  // 打印机状态
  enum regState : uint8_t { REG_NONE, REG_TRIAL, REG_LIMIT, REG_VALID };
  enum workState : uint8_t { IDLE, PRINTING, PAUSED, ONLINE, PAUSING, RESUMING, STOPPING };

  // SDO (0x2100)
  enum subIndex_MaxTemp : uint8_t {
    subIndex_MAXT_E0 = 1,
    subIndex_MAXT_E1,
    subIndex_MAXT_E2,
    subIndex_MAXT_E3,
    subIndex_MAXT_Bed,
    subIndex_MAXT_Chamber,
    subIndex_MAXT_Filament,
    subIndex_MAXT_Water,
    subIndex_MAXT_Reserve1,
    subIndex_MAXT_Reserve2,
    subIndex_MAXT_Reserve3,
    subIndex_MAXT_Reserve4,
  };

  // 通用信息位图
  union CanOpen_Global_Common {
    uint8_t value;
    struct {
      uint8_t activeT    : 2;
      uint8_t reserve    : 1;
      uint8_t workState  : 3;
      uint8_t validState : 2;
    };
  };

  // WIFI相关位图
  union CanOpen_Global_WIFI {
    uint8_t value;
    struct {
      uint8_t reserve     : 2;
      uint8_t cloudState  : 1;
      uint8_t cameraState : 1;
      uint8_t workCode    : 4;
    };
  };

  // 开关量相关位图
  union CanOpen_Global_Switch {
    uint16_t value;
    struct {
      uint8_t reserve      : 5;
      uint8_t autoLeveling : 1;
      uint8_t autoPower    : 1;
      uint8_t caseLight    : 1;
      uint8_t reserve2;
    };
  };

  // 状态量相关位图
  union CanOpen_Global_State {
    uint16_t value;
    struct {
      uint8_t reserve     : 3;
      uint8_t powerloss   : 1;
      uint8_t waitHeating : 1;
      uint8_t probeValid  : 1;
      uint8_t moving      : 1;
      uint8_t diskState   : 1;
      uint8_t reserve2;
    };
  };
}  // namespace canOpen

void initNode();
void updateNode();

#ifdef CANOPEN_LCD
void initNodeInfoLCD();
void registerNodeLCD();
void updateLCDInfo();
void PDOFilterLCD();
void checkSDOCacheLCD();
bool getLCDState();
#endif  // CANOPEN_LCD

#ifdef CANFILE
void registerCanFile();
void checkCanFileData();
bool checkSDOCacheCanFile();
#endif  // CANFILE