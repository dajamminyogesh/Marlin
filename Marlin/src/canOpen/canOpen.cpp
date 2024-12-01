/**
 *  canOpen.cpp
 *
 *  Created on: 2023/07/28
 *      Author: CreatBot LYN
 *
 *  Protocol layer for canOpen
 */

#include "../inc/MarlinConfigPre.h"

#if ENABLED(CANOPEN_SUPPORT)

  #include "../MarlinCore.h"
  #include "../gcode/queue.h"     // 联机状态判断
  #include "../module/planner.h"  // 运动状态判断
  #include "../sd/cardreader.h"   // SD卡相关
  #include "CreatBotOD.h"
  #include "canFile/canFile.h"  // CANFILE相关
  #include "canOpen.h"
  #include "canOpenHelper.h"

  #if HAS_LEVELING
    #include "../feature/bedlevel/bedlevel.h"
  #endif

  #if ENABLED(CASE_LIGHT_ENABLE)
    #include "../feature/caselight.h"
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    #include "../feature/powerloss.h"
  #endif

  #if ENABLED(HAS_CHIP_ID)
    #include "../feature/serial_number.h"
    #define PRODUCT_CHIPID getChipID32b()
  #else
    #define PRODUCT_CHIPID 0
  #endif

  #define DEBUG_OUT ENABLED(DEBUG_CANOPEN)
  #include "../core/debug_out.h"

  #ifndef PRODUCT_CODE
    #define PRODUCT_CODE 0x00000000;
  #endif
  #ifndef PRODUCT_DATE
    #define PRODUCT_DATE 0x20240315;
  #endif
  #ifndef PRODUCT_VER
    #define PRODUCT_VER "1.0.0";
  #endif
  #ifndef PRODUCT_UI
    #define PRODUCT_UI "7.1.0";
  #endif

using namespace canOpen;

extern UNS32 CreatBotOD_obj1005;                  // SyncCobID              同步COBID
extern UNS32 CreatBotOD_obj1006;                  // SyncPeriod             同步周期
extern UNS32 CreatBotOD_obj1007;                  // SyncWindow             同步窗口
extern UNS8  CreatBotOD_obj1008[32];              // DeviceName             机型名称
extern UNS8  CreatBotOD_obj1009[7];               // DeviceHVersion         硬件版本
extern UNS8  CreatBotOD_obj100A[12];              // DeviceSVersion         软件版本
extern UNS16 CreatBotOD_obj1017;                  // ProducerHeartbeat      生产者心跳
extern UNS32 CreatBotOD_obj1018_Vendor_ID;        // IdentityVendorID       制造商ID
extern UNS32 CreatBotOD_obj1018_Product_Code;     // IdentityProductCode    产品代码
extern UNS32 CreatBotOD_obj1018_Revision_Number;  // IdentityRevisionNumber 修订版本号
extern UNS32 CreatBotOD_obj1018_Serial_Number;    // IdentitySerialNumber   序列号

static uint8_t nodeNum = 0;
OPTCODE(CANOPEN_LCD, static bool LCDIsReady = false)

constexpr const char HWVersion[] PROGMEM = PRODUCT_VER;
constexpr const char UIVerison[] PROGMEM = PRODUCT_UI;
constexpr const char FWVersion[] PROGMEM = {
    // Version
    SHORT_BUILD_VERSION[0], SHORT_BUILD_VERSION[1], SHORT_BUILD_VERSION[2], '.',

    // YY year
    __DATE__[9], __DATE__[10],

    // First month letter, Oct Nov Dec = '1' otherwise '0'
    (__DATE__[0] == 'O' || __DATE__[0] == 'N' || __DATE__[0] == 'D') ? '1' : '0',

    // Second month letter
    (__DATE__[0] == 'J')   ? ((__DATE__[1] == 'a') ? '1' : ((__DATE__[2] == 'n') ? '6' : '7'))  // Jan, Jun or Jul
    : (__DATE__[0] == 'F') ? '2'                                                                // Feb
    : (__DATE__[0] == 'M') ? (__DATE__[2] == 'r') ? '3' : '5'                                   // Mar or May
    : (__DATE__[0] == 'A') ? (__DATE__[1] == 'p') ? '4' : '8'                                   // Apr or Aug
    : (__DATE__[0] == 'S') ? '9'                                                                // Sep
    : (__DATE__[0] == 'O') ? '0'                                                                // Oct
    : (__DATE__[0] == 'N') ? '1'                                                                // Nov
    : (__DATE__[0] == 'D') ? '2'                                                                // Dec
                           : 0,

    // First day letter, replace space with digit
    __DATE__[4] == ' ' ? '0' : __DATE__[4],

    // Second day letter
    __DATE__[5],

    '\0'};

// 获取激活状态
inline static uint8_t getValidState() {
  return regState::REG_NONE;
}

// 获取LCD状态
#if ENABLED(CANOPEN_LCD)
  bool getLCDState(){ return LCDIsReady; }
#endif

// 获取工作状态
inline static uint8_t getWorkState() {
  if (TERN0(CANFILE, IS_CANFILE_OPEN()) || TERN0(SDSUPPORT, IS_SD_FILE_OPEN()))
    if (TERN0(CANFILE, IS_CANFILE_PAUSED()) || TERN0(SDSUPPORT, IS_SD_PAUSED()))
      return workState::PAUSED;
    else
      return workState::PRINTING;
  else if (queue.get_current_line_number())
    return workState::ONLINE;
  else
    return workState::IDLE;
}

// 初始化更新 (大部分为只读信息)
static void initNodeInfo() {
  // 协议要求信息
  CreatBotOD_obj1005                 = 0x80 | 0x40000000;           // 同步ID为0x80, 生产同步信息
  CreatBotOD_obj1006                 = 100000;                      // 同步周期为100ms
  CreatBotOD_obj1007                 = 10000;                       // 同步窗口期为10ms
  CreatBotOD_obj1017                 = 3000;                        // 生产者心跳为3秒
  CreatBotOD_obj1018_Vendor_ID       = 0xCBCBCBCB;                  // 默认制造商ID
  CreatBotOD_obj1018_Product_Code    = PRODUCT_CODE;                // 包含主板,屏幕,扩展板信息
  CreatBotOD_obj1018_Revision_Number = PRODUCT_DATE;                // 硬件发布日期作为修订版本号
  CreatBotOD_obj1018_Serial_Number   = PRODUCT_CHIPID;              // 机器序列号
  strcpy_P((char *)CreatBotOD_obj1008, PSTR(CUSTOM_MACHINE_NAME));  // 机器名称
  strcpy_P((char *)CreatBotOD_obj1009, HWVersion);                  // 硬件版本(修订日期的直观表达)
  strcpy_P((char *)CreatBotOD_obj100A, FWVersion);                  // 固件版本

  // 机器信息
  strcpy_P((char *)MarchineInfo_FW_Ver, FWVersion);
  strcpy_P((char *)MarchineInfo_UI_Ver, TERN(CANOPEN_LCD, UIVerison, "0.0.0"));
  MarchineInfo_ExtNum                 = EXTRUDERS;
  MarchineInfo_BedSupport             = ENABLED(HAS_HEATED_BED);
  MarchineInfo_ChamberSupport         = ENABLED(HAS_HEATED_CHAMBER);
  MarchineInfo_FilamentChamberSupport = false;
  MarchineInfo_WaterTempSupport       = false;
  MarchineInfo_WIFISupport            = ENABLED(CREATBOT_WIFI_SUPPORT);
  MarchineInfo_CameraSupport          = false;

  // 配置最大温度设置
  OPTCODE(HAS_HOTEND0, MaxTemp[subIndex_MAXT_E0 - 1] = (HEATER_0_MAXTEMP - HOTEND_OVERSHOOT))
  OPTCODE(HAS_HOTEND1, MaxTemp[subIndex_MAXT_E1 - 1] = (HEATER_1_MAXTEMP - HOTEND_OVERSHOOT))
  OPTCODE(HAS_HOTEND2, MaxTemp[subIndex_MAXT_E2 - 1] = (HEATER_2_MAXTEMP - HOTEND_OVERSHOOT))
  OPTCODE(HAS_HOTEND3, MaxTemp[subIndex_MAXT_E3 - 1] = (HEATER_3_MAXTEMP - HOTEND_OVERSHOOT))
  OPTCODE(HAS_HEATED_BED, MaxTemp[subIndex_MAXT_Bed - 1] = BED_MAX_TARGET)
  OPTCODE(HAS_HEATED_CHAMBER, MaxTemp[subIndex_MAXT_Chamber - 1] = CHAMBER_MAX_TARGET)

  // 初始化全局信息
  CMD_TASK     = 0x0000;
  CMD_MOVE     = 0x0000;
  CMD_EXTRUDER = 0x0000;
  CMD_COMMON   = 0x0000;
  FILE_INDEX   = 0x0000;
  ZERO(GCODE_INPUT);

  // 初始化屏幕节点信息
  OPTCODE(CANOPEN_LCD, initNodeInfoLCD())
}

// 初始化节点 PDO
static void initNodePDO() {
  disablePDO(PDO(1));
  OPTCODE(CANOPEN_LCD, PDODisableLCD())
}

// 更新全局信息
static void updateGlobalInfo() {
  CanOpen_Global_Common uCommon;
  CanOpen_Global_WIFI   uWIFI;
  CanOpen_Global_Switch uSwitch;
  CanOpen_Global_State  uState;

  uCommon.validState = getValidState();
  uCommon.workState  = getWorkState();
  uCommon.activeT    = active_extruder;
  Global_Common      = uCommon.value;

  #if ENABLED(CREATBOT_WIFI_SUPPORT)
  uWIFI.workCode    = 0;
  uWIFI.cameraState = 0;
  uWIFI.cloudState  = 0;
  Global_WIFI       = uWIFI.value;
  #endif

  uSwitch.caseLight    = TERN1(CASE_LIGHT_ENABLE, caselight.on);
  uSwitch.autoPower    = 0;
  uSwitch.autoLeveling = TERN0(HAS_LEVELING, planner.leveling_active);
  Global_Switch        = uSwitch.value;

  uState.diskState     = TERN0(SDSUPPORT, IS_SD_INSERTED());
  uState.moving        = planner.has_blocks_queued();
  uState.probeValid    = TERN0(HAS_LEVELING, leveling_is_valid());
  uState.waitHeating   = wait_for_heatup;
  uState.powerloss     = TERN0(POWER_LOSS_RECOVERY, recovery.state);
  Global_State         = uState.value;
}

// 通用SDO回调, 检测是否有缓存的SDO要发送
void fisishSDO(CO_Data *d, uint8_t nodeId) {
  resetClientSDOLineFromNodeId(d, nodeId);

  // 检测CANFILE的SDO缓存
  #if ENABLED(CANFILE)
  if (checkSDOCacheCanFile()) return;
  #endif

  // 检测LCD的SDO缓存
  #if ENABLED(CANOPEN_LCD)
  if (nodeId == LCD_NODE_ID) {
    checkSDOCacheLCD();
  }
  #endif
}

// 更新从节点的连接状态
static void updateSlaveState(CO_Data *d, uint8_t nodeId, bool isReady) {
  #if ENABLED(CANOPEN_LCD)
  if (nodeId == LCD_NODE_ID) {
    if (LCDIsReady ^ isReady) {
      LCDIsReady = isReady;
      nodeNum += LCDIsReady ? 1 : -1;
      if (LCDIsReady) {
        // 启用屏幕相关的PDO发送
        PDOEnableLCD();
        PDOFilterLCD();
        DEBUG_ECHOLNPGM("LCD PDO enable");
      } else {
        // 禁用屏幕相关的PDO发送
        PDODisableLCD();
        DEBUG_ECHOLNPGM("LCD PDO disable");
      }
    }
  }
  #endif

  if (nodeNum)
    PDOEnable(d, PDO(1));
  else
    PDODisable(d, PDO(1));
}

// 更新从节点的心跳超时
static void updateSlaveHeart(CO_Data *d, UNS8 nodeId) {
  uint16_t data = 0;
  uint32_t size = 2;
  uint32_t abortCode;

  if (getReadResultNetworkDict(d, nodeId, &data, &size, &abortCode) == SDO_FINISHED) {
    uint32_t time = data * 1.5;
    NOMORE(time, 0xFFFF);
    if (time) {
  #if ENABLED(CANOPEN_LCD)
      if ((nodeId == LCD_NODE_ID) && (HEART_INDEX_LCD < *d->ConsumerHeartbeatCount)) {
        d->ConsumerHeartbeatEntries[HEART_INDEX_LCD] = ((uint32_t)nodeId << 16) | time;
      }
  #endif
    }
    DEBUG_ECHOLNPGM("Read heartTime timeout is ", time, " , NodeID is ", nodeId);
  } else {
    DEBUG_ECHOLNPGM("Failed at read heartTime, abortCode is ", abortCode, " , NodeID is ", nodeId);
    fisishSDO(d, nodeId);
  }
}

// 注册从节点心跳相关回调函数
static void registerHeartCallBack() {
  // 从节点启动回调
  CreatBotOD_Data.post_SlaveBootup = [](CO_Data *d, uint8_t SlaveID) {
    // 重置与从节点之间先前可能存在的SDO通讯
    resetClientSDOLineFromNodeId(d, SlaveID);
    // 获取从节点的心跳时间
    readNetworkDictCallback(d, SlaveID, 0x1017, 0x00, 0, &updateSlaveHeart, 0);
  };

  // 从节点状态改变回调
  CreatBotOD_Data.post_SlaveStateChange = [](CO_Data *d, uint8_t nodeId, e_nodeState newNodeState) {
    DEBUG_ECHOLNPGM("Node have changed, nodeID is ", nodeId, " , newNodeState is ", newNodeState);
    updateSlaveState(d, nodeId, newNodeState == Operational);
  };

  // 从节点跳超时回调
  CreatBotOD_Data.heartbeatError = [](CO_Data *d, uint8_t heartbeatID) {
    DEBUG_ECHOLNPGM("Node have left, nodeID is ", heartbeatID);
    updateSlaveState(d, heartbeatID, false);
  };
}

// 注册节点回调
static void registerNode() {
  registerHeartCallBack();                  // 注册子节点心跳回调函数
  OPTCODE(CANOPEN_LCD, registerNodeLCD());  // 注册LCD节点回调函数
  OPTCODE(CANFILE, registerCanFile());      // 注册CANFILE功能回调函数
}

// CanOpen节点初始化
void initNode() {
  initNodeInfo();  // 初始化节点常量信息
  initNodePDO();   // 初始化节点PDO信息
  registerNode();  // 注册节点回调函数

  canInit(&CreatBotOD_Data, CANOPEN_BAUDRATE);
  setState(&CreatBotOD_Data, Initialisation);
  setState(&CreatBotOD_Data, Operational);
}

// CanOpen节点更新
void updateNode() {
  updateGlobalInfo();
  OPTCODE(CANOPEN_LCD, updateLCDInfo());
  OPTCODE(CANFILE, checkCanFileData());
}

#endif  // CREATBOT_LINUX_LCD
