/**
 * DGUSScreenHandlerHelper.h
 *
 *  Created on: 2022/05/18
 *      Author: CreatBot LYN
 *
 *  Helper for DGUSScreenHandler
 */
#pragma once

#include "../../ui_api.h"
#include "DGUSDisplay.h"
#include "DGUSDisplayDef.h"
#include "DGUSVPVariable.h"

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../../../../feature/powerloss.h"
#endif

using namespace ExtUI;

constexpr float cpow(const float x, const int y) { return y == 0 ? 1.0 : x * cpow(x, y - 1); }

inline uint16_t swap16(const uint16_t value) { return (value & 0xFFU) << 8U | (value >> 8U); }

inline uint32_t swap32(const uint32_t value) {
  return (value & 0x000000FFU) << 24U | (value & 0x0000FF00U) << 8U | (value & 0x00FF0000U) >> 8U |
         (value & 0xFF000000U) >> 24U;
}

// 从屏幕变量获取对应的轴
bool getSelectorFromVarialbe(DGUS_VP_Variable &var, ExtUI::axis_t &axis) {
  switch (var.VP) {  // clang-format off
    case VP_XPos:
    case VP_X_STR_ADDR:
    case VP_X_STEP_PER_MM:
    case VP_X_STEP_PER_MM_Set:  axis = ExtUI::axis_t::X;       break;
    case VP_YPos:
    case VP_Y_STR_ADDR:
    case VP_Y_STEP_PER_MM:
    case VP_Y_STEP_PER_MM_Set:  axis = ExtUI::axis_t::Y;       break;
    case VP_BabyStepZ:
    case VP_ZPos:
    case VP_Z_STR_ADDR:
    case VP_Z_STEP_PER_MM:
    case VP_Z_STEP_PER_MM_Set:  axis = ExtUI::axis_t::Z;       break;
    default: return false;
  }  // clang-format on
  return true;
}

// 从屏幕变量获取对应的挤出器
bool getSelectorFromVarialbe(DGUS_VP_Variable &var, ExtUI::extruder_t &extruder) {
  switch (var.VP) {  // clang-format off
    case VP_Flowrate_E:
    case VP_Flowrate_E_Set:
    case VP_T_ACT_Cur:
    case VP_T_ACT_Tar:
                                extruder = ExtUI::getActiveTool();      break;
    case VP_E_STEP_PER_MM:
    case VP_E_STEP_PER_MM_Set:  extruder = ExtUI::extruder_t::E0;       break;
    default: return false;
  }  // clang-format on
  return true;
}

// 从屏幕变量获取对应的加热器
bool getSelectorFromVarialbe(DGUS_VP_Variable &var, ExtUI::heater_t &heater) {
  switch (var.VP) {  // clang-format off
    case VP_T_E0_Cur:
    case VP_T_E0_Tar:
    case VP_T_E0_Set:
    case VP_PreHeat_T_E0:
    case VP_PreHeat_T_E0_Set:
                                      heater = ExtUI::heater_t::H0; break;
    case VP_T_E1_Cur:
    case VP_T_E1_Tar:
    case VP_T_E1_Set:
    case VP_PreHeat_T_E1:
    case VP_PreHeat_T_E1_Set:
                                      heater = ExtUI::heater_t::H1; break;
    case VP_T_E2_Cur:
    case VP_T_E2_Tar:
    case VP_T_E2_Set:
    case VP_PreHeat_T_E2:
    case VP_PreHeat_T_E2_Set:
                                      heater = ExtUI::heater_t::H2; break;
    case VP_T_Bed_Cur:
    case VP_T_Bed_Tar:
    case VP_T_Bed_Set:
    case VP_PreHeat_T_Bed:
    case VP_PreHeat_T_Bed_Set:
                                      heater = ExtUI::heater_t::BED; break;
    case VP_T_Chamber_Cur:
    case VP_T_Chamber_Tar:
    case VP_T_Chamber_Set:
    case VP_PreHeat_T_Chamber:
    case VP_PreHeat_T_Chamber_Set:
                                      heater = ExtUI::heater_t::CHAMBER; break;
    default: return false;
  }  // clang-format on
  return true;
}

// 从屏幕变量获取对应的风扇
bool getSelectorFromVarialbe(DGUS_VP_Variable &var, ExtUI::fan_t &fan) {
  switch (var.VP) {  // clang-format off
    case VP_Fan0_Percentage:
    case VP_Fan0_Percentage_Set:    fan = ExtUI::fan_t::FAN0; break;
    case VP_Fan1_Percentage:
    case VP_Fan1_Percentage_Set:    fan = ExtUI::fan_t::FAN1; break;
    default: return false;
  }  // clang-format on
  return true;
}

// 获取指定的值并更新屏幕
template <typename selectorType, uint16_t decimals = 0, typename WireType = uint16_t,
          typename funcType = float (*)(const selectorType)>
static void UpdateValToLCD(funcType getValFunc, DGUS_VP_Variable &var) {
  selectorType selector;
  if (getSelectorFromVarialbe(var, selector)) {
    float value = getValFunc(selector);
    value *= cpow(10, decimals);
    dgusdisplay.WriteVariable(var.VP, (WireType)LROUND(value));
  }
}

// 获取指定的值并更新屏幕
template <uint16_t decimals = 0, typename WireType = uint16_t, typename funcType = float (*)(void)>
static void UpdateValToLCD(funcType getValFunc, DGUS_VP_Variable &var) {
  float value = getValFunc();
  value *= cpow(10, decimals);
  dgusdisplay.WriteVariable(var.VP, (WireType)LROUND(value));
}

// 从屏幕获取指定的值并设置机器
template <typename selectorType, uint16_t decimals = 0, typename WireType = uint16_t,
          typename funcType = void (*)(float, selectorType)>
static void SetValFromLCD(funcType setValFunc, DGUS_VP_Variable &var, void *val_ptr) {
  selectorType selector;
  if (getSelectorFromVarialbe(var, selector)) {
    WireType newValue = *(uint8_t *)val_ptr;
    if (sizeof(newValue) == 2) {
      newValue = swap16(*(WireType *)val_ptr);
    } else if (sizeof(newValue) == 4) {
      newValue = swap32(*(WireType *)val_ptr);
    }
    setValFunc(newValue / cpow(10, decimals), selector);
  }
}

// 从屏幕获取指定的值并设置机器
template <uint16_t decimals = 0, typename WireType = uint16_t, typename funcType = void (*)(float)>
static void SetValFromLCD(funcType setValFunc, DGUS_VP_Variable &var, void *val_ptr) {
  WireType newValue = *(uint8_t *)val_ptr;
  if (sizeof(newValue) == 2) {
    newValue = swap16(*(WireType *)val_ptr);
  } else if (sizeof(newValue) == 4) {
    newValue = swap32(*(WireType *)val_ptr);
  }
  setValFunc(newValue / cpow(10, decimals));
}

// 从指定页面中获取页面列表指针
const uint16_t *DGUSLCD_FindScreenVPMapList(uint8_t screen) {
  const uint16_t         *ret;
  const struct VPMapping *map = VPMap;
  while ((ret = (uint16_t *)pgm_read_ptr(&(map->VPList)))) {
    if (pgm_read_byte(&(map->screen)) == screen) return ret;
    map++;
  }
  return nullptr;
}

// 获取机器的默认页面
const DGUSLCD_Screens DGUSLCD_GetDefaultScreen() {
  DGUSLCD_Screens DGUSLCD_SCREEN_DEFAULT = DGUSLCD_SCREEN_BOOT;

  if (TERN0(POWER_LOSS_RECOVERY, recovery.recoveryPrepare))  // 断电续打准备阶段
    return DGUSLCD_SCREEN_DEFAULT_PRINT_PAUSE;

  if (ExtUI::isPrintingFromMedia())
    if (ExtUI::isPrintingFromMediaPaused() || TERN0(HAS_RESUME_CONTINUE, wait_for_user))
      DGUSLCD_SCREEN_DEFAULT = DGUSLCD_SCREEN_DEFAULT_PRINT_RESUME;  // 继续打印页面
    else
      DGUSLCD_SCREEN_DEFAULT = DGUSLCD_SCREEN_DEFAULT_PRINT_PAUSE;  // 正在打印页面
  else if (ExtUI::isPrintingFromSerial())
    DGUSLCD_SCREEN_DEFAULT = DGUSLCD_SCREEN_DEFAULT_ONLINE;  // 联机打印页面
  else if (ExtUI::isMediaInserted())
    if (ExtUI::isAnyHeating())
      DGUSLCD_SCREEN_DEFAULT = DGUSLCD_SCREEN_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT;  // 加热中&U盘插入
    else
      DGUSLCD_SCREEN_DEFAULT = DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT;  // 未加热&U盘插入
  else if (ExtUI::isAnyHeating())
    DGUSLCD_SCREEN_DEFAULT = DGUSLCD_SCREEN_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE;  // 加热中&U盘移除
  else
    DGUSLCD_SCREEN_DEFAULT = DGUSLCD_SCREEN_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE;  // 未加热&U盘移除

  return DGUSLCD_SCREEN_DEFAULT;
}

// 是否是弹窗页面
inline bool DGUSLCD_IsPopScreen(DGUSLCD_Screens screen) {
  if (screen == DGUSLCD_SCREEN_POP_0 ||  // 弹窗页面0
      screen == DGUSLCD_SCREEN_POP_1 ||  // 弹窗页面1
      screen == DGUSLCD_SCREEN_POP_2) {  // 弹窗页面2
    return true;
  } else
    return false;
}

// 指定变量和颜色画进度条
inline void DGUSLCD_DrawProgressBar(uint16_t adr, uint16_t color, uint16_t permill) {
  uint16_t x0 = map(permill, 0, 1000, 0, PROGRESS_BAR_X1);
  if (x0)
    dgusdisplay.FillRectangle(adr, PROGRESS_BAR_X0, PROGRESS_BAR_Y0, x0, PROGRESS_BAR_Y1, color);
  else
    dgusdisplay.ClearCanvas(adr);
}

#if ENABLED(CreatBot_DGUS_V6)
// 添加剪切矩形数据块
void DGUSLCD_CpyCutData(unsigned char *dataPtr, uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t x1,
                        uint16_t y1, uint16_t page) {
  const unsigned char data[] = {// 要剪切的页面
                                (uint8_t)(page >> 8), (uint8_t)(page & 0xFF),
                                // 被剪切矩形的左上角坐标
                                (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF), (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF),
                                // 被剪切矩形的右下角坐标
                                (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF), (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF),
                                // 粘贴矩形的左上角坐标
                                (uint8_t)(x >> 8), (uint8_t)(x & 0xFF), (uint8_t)(y >> 8), (uint8_t)(y & 0xFF)};
  memcpy(dataPtr, data, sizeof(data));
}

#define PRESET_BUFFER (CUT_DATA_LENGTH * PROGRESS_BAR_R)
// 指定变量预设进度条的相关数据
inline void DGUSLCD_PresetProgressBar(uint16_t adr) {
  static_assert(PRESET_BUFFER < 255, "PRESET_BUFFER should less then 255");
  unsigned char data[PRESET_BUFFER * 2] = {0x00};

  for (uint8_t i = 0; i < PROGRESS_BAR_R; i++) {
    uint16_t yU = LROUND(PROGRESS_BAR_Y0 + PROGRESS_BAR_R - SQRT((2 * PROGRESS_BAR_R - i) * i));
    uint16_t yD = LROUND(PROGRESS_BAR_Y1 - PROGRESS_BAR_R + SQRT((2 * PROGRESS_BAR_R - i) * i));

    DGUSLCD_CpyCutData(&data[CUT_DATA_LENGTH * i * 2],        // 数据块位置
                       PROGRESS_BAR_X0 + i, PROGRESS_BAR_Y0,  // 复制位置
                       PROGRESS_BAR_X0 + i, PROGRESS_BAR_Y0,  // 剪切区域左上角
                       PROGRESS_BAR_X0 + i, yU - 1,           // 剪切区域右下角
                       PROGRESS_BAR_PAGE);                    // 剪切页面

    DGUSLCD_CpyCutData(&data[CUT_DATA_LENGTH * (i * 2 + 1)],  // 数据块位置
                       PROGRESS_BAR_X0 + i, yD,               // 复制位置
                       PROGRESS_BAR_X0 + i, yD,               // 剪切区域左上角
                       PROGRESS_BAR_X0 + i, PROGRESS_BAR_Y1,  // 剪切区域右下角
                       PROGRESS_BAR_PAGE);                    // 剪切页面
  }

  dgusdisplay.WriteVariable(adr + 16, data, PRESET_BUFFER);
  dgusdisplay.WriteVariable(adr + 16 + PRESET_BUFFER / 2, &data[PRESET_BUFFER], PRESET_BUFFER);
}

// 指定变量和背景页面粘贴进度条
inline void DGUSLCD_CutProgressBar(uint16_t adr, uint16_t permill) {
  const uint16_t x0 = map(permill, 0, 1000, PROGRESS_BAR_X1, PROGRESS_BAR_X0);
  const uint16_t x1 = PROGRESS_BAR_X0 + PROGRESS_BAR_R;

  // 剪切矩形
  unsigned char data[32] = {0x00, 0x06, 0x00, 0x02};

  // 复制右侧样式
  DGUSLCD_CpyCutData(&data[4],                                               // 第一个数据块
                     PROGRESS_BAR_X0, PROGRESS_BAR_Y0,                       // 复制位置
                     x0, PROGRESS_BAR_Y0, PROGRESS_BAR_X1, PROGRESS_BAR_Y1,  // 剪切的区域
                     PROGRESS_BAR_PAGE);                                     // 剪切页面

  // 复制左侧样式
  DGUSLCD_CpyCutData(&data[18],                                              // 第二个数据块
                     PROGRESS_BAR_X0, PROGRESS_BAR_Y0,                       // 复制位置
                     PROGRESS_BAR_X0, PROGRESS_BAR_Y0, x1, PROGRESS_BAR_Y1,  // 剪切的区域
                     PROGRESS_BAR_PAGE);                                     // 剪切页面

  // 左侧达不到剪切条件
  if (x0 + PROGRESS_BAR_R > PROGRESS_BAR_X2) {
    uint8_t offset = (PROGRESS_BAR_X1 - x0) / 2;
    NOMORE(offset, PROGRESS_BAR_R);
    data[3]  = 2 + offset * 2;  // 修改数据块总个数
    data[18] = 0xFE;            // 跳过第二个数据块
  }

  dgusdisplay.WriteVariable(adr, data, sizeof(data));
}
#endif

// 更新状态字符串信息
inline void DGUSLCD_UpdateStatusMsg(const char *const msg) {
  dgusdisplay.WriteString(VP_MSG_M117, msg, LCD_MSG_LENGTH);
#if ENABLED(CreatBot_DGUS_V5)
  dgusdisplay.WriteString(VP_MSG_M117_SHORT, msg, LCD_MSG_LENGTH);
#elif ENABLED(CreatBot_DGUS_V6)
  dgusdisplay.WriteString(VP_MSG_M117_ROLL, msg, LCD_ROLLMSG_LENGTH);
#endif
}