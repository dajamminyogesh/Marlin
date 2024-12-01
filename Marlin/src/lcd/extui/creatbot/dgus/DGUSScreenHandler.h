/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

enum DGUSLCD_Screens : uint8_t;

#include "../../../../inc/MarlinConfigPre.h"
#include "../../ui_api.h"
#include "DGUSDisplayDef.h"
#include "DGUSVPVariable.h"
using namespace ExtUI;

class DGUSScreenHandler {
 public:
  DGUSScreenHandler() = default;

  static void init();
  static bool loop();

  static void PrinterPoweroffHook();
  static void PrinterKilledHook(FSTR_P const error, FSTR_P const component);
  static void PrintTimerStartHook();
  static void PrintTimerPauseHook();
  static void PrintTimerStopHook();
  static void PrintDoneHook();
  static void FactoryResetHook();
  static void StoreSettingsHook(char *);
  static void LoadSettingsHook(const char *);
  static void SettingsStoredHook();
  static void HomingStartHook();
  static void HomingDoneHook();
  static void FilamentRunoutHook();
#if HAS_LEVELING
  static void LevelingStartHook();
  static void LevelingDoneHook();
#endif
#if ENABLED(POWER_LOSS_RECOVERY)
  static void PowerLossResumeHook();
#endif
  static void StatusChangedHook(const char *const msg);
  static void UserConfirmRequiredHook(const char *const msg);

  static void TouchHook(DGUS_VP_Variable &, void *);            // 处理屏幕触摸
  static void PageHook(DGUS_VP_Variable &, void *);             // 处理当前页面
  static void HandlePageDefault(DGUS_VP_Variable &, void *);    // 返回默认页
  static void HandlePrintStatuSet(DGUS_VP_Variable &, void *);  // 打印状态设置
  static void HandleStopPrint(DGUS_VP_Variable &, void *);      // 中止打印
  static void HandlePreheat(DGUS_VP_Variable &, void *);        // 预热按钮
  static void HandleCoolDown(DGUS_VP_Variable &, void *);       // 冷却按钮
  static void HandleFilament(DGUS_VP_Variable &, void *);       // 耗材菜单按钮
  static void HandleSetting(DGUS_VP_Variable &, void *);        // 设置菜单按钮
  static void HandleAdjust(DGUS_VP_Variable &, void *);         // 调整菜单按钮
  static void HandleRebootIntoDFUMode(DGUS_VP_Variable &, void *);  // 重启进入DFU模式

  static void HandleHome(DGUS_VP_Variable &, void *);            // 回零按钮
  static void HandleManualMove(DGUS_VP_Variable &, void *);      // 移轴按钮
  static void HandleZAxisUp(DGUS_VP_Variable &, void *);         // 暂停时Z轴向上移动
  static void HandleZAxisDown(DGUS_VP_Variable &, void *);       // 暂停时Z轴向下移动
  static void HandleManualExtrude(DGUS_VP_Variable &, void *);   // 送(退)丝按钮
  static void HandleLoadFilament(DGUS_VP_Variable &, void *);    // 加(卸)载按钮
  static void HandleFilamentChange(DGUS_VP_Variable &, void *);  // 更换耗材按钮
  static void HandleBabyStepUp(DGUS_VP_Variable &, void *);      // 打印时BabyStep上调
  static void HandleBabyStepDown(DGUS_VP_Variable &, void *);    // 打印时BabyStep下调

  static void HandleStore(DGUS_VP_Variable &, void *);  // 保存设置按钮
  static void HandleReset(DGUS_VP_Variable &, void *);  // 恢复出厂按钮

  static void HandleCancelShutdown(DGUS_VP_Variable &, void *);  // 关机冷却取消按钮
  static void HandleHeatSkip(DGUS_VP_Variable &, void *);        // 加热等待取消按钮
  static void HandlePopButton(DGUS_VP_Variable &, void *);       // 弹窗按钮

#if ENABLED(SDSUPPORT)
  static void SDCardInserted();                              // U盘插入
  static void SDCardError();                                 // U盘异常
  static void SDCardRemoved();                               // U盘移除
  static void HandleFileOpen(DGUS_VP_Variable &, void *);    // U盘按钮
  static void HandleFileAction(DGUS_VP_Variable &, void *);  // U盘列表项
  static void HandleFileNext(DGUS_VP_Variable &, void *);    // U盘下一页
  static void HandleFileLast(DGUS_VP_Variable &, void *);    // U盘上一页
  static void HandleFileUp(DGUS_VP_Variable &, void *);      // U盘上一级
  static void HandleFilePrint(DGUS_VP_Variable &, void *);   // U盘文件打印
  static void HandleFileCancel(DGUS_VP_Variable &, void *);  // U盘文件取消
  static void UpdateFileName(DGUS_VP_Variable &);            // U盘更新文件名
  static void UpdatePrintFileName(DGUS_VP_Variable &);       // 确认打印页面更新文件名
  static void UpdateFileICO(DGUS_VP_Variable &);             // U盘更新文件图标
#endif

  // 步进设置
  static void HandleSettingMotion(DGUS_VP_Variable &, void *) { GotoScreen(DGUSLCD_SCREEN_MOTION_SETTING); }
  // 风扇设置
  static void HandleSettingFan(DGUS_VP_Variable &, void *) { GotoScreen(DGUSLCD_SCREEN_FANSPEED_SETTING); }
  // 预热设置
  static void HandleSettingPreheat(DGUS_VP_Variable &, void *) { GotoScreen(DGUSLCD_SCREEN_PREHEAT_SETTING); }
  // 更多设置
  static void HandleSettingMore(DGUS_VP_Variable &, void *) { GotoScreen(DGUSLCD_SCREEN_MORESETTING); }

#if HAS_LEVELING
  static void HandleSettingLeveling(DGUS_VP_Variable &, void *);    // 调平设置
  static void HandleLevelingSwitch(DGUS_VP_Variable &, void *);     // 调平开关
  static void HandleLevelingCalibrate(DGUS_VP_Variable &, void *);  // 调平启动
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  static void HandleAccidentReuse(DGUS_VP_Variable &, void *);   // 断电续打继续按钮
  static void HandleAccidentCancel(DGUS_VP_Variable &, void *);  // 断电续打取消按钮
#endif

// TODO CreatBot 换成合适的宏定义
#if ENABLED(CREATBOT_WIFI_SUPPORT)
  // WIFI设置  //TODO CreatBot 根据wifi状态去不同页面
  static void HandleSettingWifi(DGUS_VP_Variable &, void *) { GotoScreen(DGUSLCD_SCREEN_CONNECT_WIFI_12); }
#endif

  static void UpdateTemperatureMax(DGUS_VP_Variable &);                // 更新最大温度
  static void UpdateTemperatureCur(DGUS_VP_Variable &);                // 更新当前温度
  static void UpdateTemperatureTar(DGUS_VP_Variable &);                // 更新目标温度
  static void HandleTemperatureSet(DGUS_VP_Variable &, void *);        // 设置目标温度
  static void UpdateFeedrate(DGUS_VP_Variable &);                      // 更新全局速度
  static void HandleFeedrateSet(DGUS_VP_Variable &, void *);           // 设置全局速度
  static void UpdateActivatedExtruder(DGUS_VP_Variable &);             // 更新活动挤出机
  static void HandleActivatedExtruderSet(DGUS_VP_Variable &, void *);  // 设置活动挤出机
  static void UpdateFilamentLength(DGUS_VP_Variable &);                // 更新耗材测试的长度
  static void HandleFilamentLengthSet(DGUS_VP_Variable &, void *);     // 设置耗材测试的长度
  static void UpdatePrintPercentInfo(DGUS_VP_Variable &);              // 更新打印百分比信息
  static void UpdateTimeInfo(DGUS_VP_Variable &);                      // 更新时间
  static void UpdateAxisString(DGUS_VP_Variable &);                    // 更新xyz轴字符串
  static void UpdateMsg(DGUS_VP_Variable &);                           // 更新字符串信息
  static void UpdateVersionInfo(DGUS_VP_Variable &);                   // 更新版本信息
  static void UpdateFlowrate(DGUS_VP_Variable &);                      // 更新挤出量
  static void HandleFlowrateSet(DGUS_VP_Variable &, void *);           // 设置挤出量
  static void UpdateAxisPos(DGUS_VP_Variable &);                       // 更新轴坐标
  static void UpdateAxisUnit(DGUS_VP_Variable &);                      // 更新轴单位
  static void HandleAxisUnitSet(DGUS_VP_Variable &, void *);           // 设置轴单位
  static void UpdateStepPerMM(DGUS_VP_Variable &);                     // 更新步进值
  static void HandleStepPerMMSet(DGUS_VP_Variable &, void *);          // 设置步进值
  static void UpdatePreHeat(DGUS_VP_Variable &);                       // 更新预热参数
  static void HandlePreheadSet(DGUS_VP_Variable &, void *);            // 设置预热参数
  static void UpdateFan(DGUS_VP_Variable &);                           // 更新风扇参数
  static void HandleFanSet(DGUS_VP_Variable &, void *);                // 设置风扇参数
  static void UpdateSwitchIcon(DGUS_VP_Variable &);                    // 开关图标更新
  static void HandleSwitchAction(DGUS_VP_Variable &, void *);          // 设置开关按钮
  static void UpdateBabyStep(DGUS_VP_Variable &var);                   // 更新BabyStep
#if HAS_LEVELING                                                       //
  static void UpdateProbeOffsetZ(DGUS_VP_Variable &);                  // 更新调平偏移
  static void HandleProbeOffsetZSet(DGUS_VP_Variable &, void *);       // 设置调平偏移
  static void UpdateFadeHeight(DGUS_VP_Variable &);                    // 更新调平淡化高度
  static void HandleFadeHeightSet(DGUS_VP_Variable &, void *);         // 设置调平淡化高度
#endif                                                                 //

#if ENABLED(CREATBOT_WIFI_SUPPORT)
  static void UpdateWiFiInfo(char *, char *);                     // 更新WIFI信息
  static void UpdateWiFiUUID(char *);                             // 更新WIFIUUID
  static void UpdateWiFiHomeIcon(void);                           // 更新WIFI信号图标
  static void UpdateWiFiState(void);                              // 更新WIFI状态图标
  static void UpdateFilelistStatus(char *);                       // 更新文件列表状态
  static bool HandleGetFileNameInfo(uint8_t, uint8_t *, char *);  // 远程获取文件名信息
  static void HandleFileViewAction(uint8_t);                      // 远程浏览文件操作
  static void HandleFileItemAction(uint8_t);                      // 远程文件操作
  static void HandleWiFiInfoSet(DGUS_VP_Variable &, void *);      // 设置WIFI信息
  static void HandleConnectWiFi(DGUS_VP_Variable &, void *);      // 连接WIFI
#endif

 private:
  static void GotoDefault();                                            // 跳转到默认页
  static void GotoPopup(uint16_t newPopIndex);                          // 跳转到弹窗页面
  static void GotoScreen(DGUSLCD_Screens screen);                       // 跳转到指定页面
  static void DelayGotoScreen(DGUSLCD_Screens screen, millis_t delay);  // 跳转到指定页面, 指定跳转延时
  static void DelayGotoPop(uint16_t Pop, millis_t delay);               // 跳转到指定弹窗, 指定跳转延时
  static void UpdateNewScreen(DGUSLCD_Screens newscreen);               // 更新新的页面
  static void UpdateScreenInitData();                                   // 更新初始化数据 (非自动更新)
  static void UpdateScreenVPData();                                     // 更新当前页面数据
  static void ForceCompleteUpdate() {                                   // 强制完全更新
    updatePtr      = 0;
    ScreenComplete = false;
  }
  static bool IsScreenComplete() { return ScreenComplete; }  // 更新是否完成
  static bool IsInTouch() { return InTouch; }                // 屏幕是否触摸
  static bool IsDefaultScreen();                             // 是否是默认页面
  static bool IsHoldOnScreen();                              // 是否是挂起页面

  // static DGUSLCD_Screens getCurrentScreen() { return current_screen; }

  //手动移轴回调
  static void axisMoveHook() { GotoPopup(POP1_ICO_MOVING); }

 private:
  static DGUSLCD_Screens curScreen;      // 当前显示的页面
  static DGUSLCD_Screens preScreen;      // 先前显示的页面(主要用于弹窗返回)
  static DGUSLCD_Screens delayScreen;    // 延时显示的页面
  static millis_t        delayJumpTime;  // 延时跳转时间

  static uint8_t  updatePtr;       // 页面数据更新指针(受制于串口缓冲区数据可能更新不完全)
  static uint16_t skipVP;          // 忽略更新的数据变量(避免与用户交互冲突)
  static bool     ScreenComplete;  // 更新完成的标志
  static bool     ChangeComplete;  // 页面跳转完成的标志
  static bool     InTouch;         // 触摸状态
  static uint16_t popIndex;        // 弹窗图标索引
  static uint16_t delayPop;        // 延时弹窗索引
};

extern DGUSScreenHandler ScreenHandler;

struct DGUSSetting {
  celsius_t preheat_T_temp;  // 喷头预热值
  celsius_t preheat_B_temp;  // 热床预热值
  celsius_t preheat_C_temp;  // 热风预热值
  bool      isCaseLightOn;   // 是否开启灯光
  bool      isAutoPowerOff;  // 是否自动关机
  bool      isAutoBackMain;  // 是否自动返回主页
};

#define LCD_PAGE_TIMEOUT 15000  // ms = 15s // 自动返回屏幕默认页的时间
#define LCD_TIME_DISPALY 300    // s = 5min // 自动关机倒计时显示的时间