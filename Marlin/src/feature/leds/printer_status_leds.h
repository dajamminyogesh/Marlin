/**
 *  printer_event_leds.h
 *
 *  Created on: 2024/04/23
 *      Author: CreatBot LYN
 *
 *  LED color changing based on printer work status
 */

#pragma once

/**
 * OFF  未初始化
 * IDLE 空闲待机
 * WORK 正在打印, 正在加热
 * WARN 正在暂停, 耗材异常
 * ERR  温度出错
 * DONE 打印完成
 */

class PrinterStatusLEDs {
 public:
  enum PrinterStatus_t { OFF, IDLE, WORK, WARN, ERR, DONE };
  static void update(bool error = false);

 private:
  static PrinterStatus_t state;
};

#ifndef LEDCOLOR_IDLE
  #define LEDCOLOR_IDLE LEDColorWhite()  // 默认空闲中白光
#endif
#ifndef LEDCOLOR_WORK
  #define LEDCOLOR_WORK LEDColorBlue()  // 默认工作中蓝光
#endif
#ifndef LEDCOLOR_WARN
  #define LEDCOLOR_WARN LEDColorRed()  // 默认警告时红光
#endif
#ifndef LEDCOLOR_ERR
  #define LEDCOLOR_ERR LEDCOLOR_WARN  // 默认等效于警告
#endif
#ifndef LEDCOLOR_DONE
  #define LEDCOLOR_DONE LEDCOLOR_IDLE  // 默认等效于空闲
#endif

extern PrinterStatusLEDs printerStatusLEDs;
