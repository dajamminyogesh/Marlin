/*
 * usb_host.h
 *
 *  Created on: 2022/04/12
 *      Author: CreatBot LYN
 *
 *  USB interface use CH376S
 */

#pragma once

#include "ch376_impl.h"
typedef enum {
  USB_STATE_IDLE,          // 主机空闲
  USB_STATE_RESET_DEVICE,  // 主机总线重置
  USB_STATE_CONFIGURING,   // 主机配置设备
  USB_STATE_ERROR,         // 主机状态错误(CH376错误或事务执行错误)
  USB_STATE_RUNNING,       // 主机检测到存储设备并初始化完成,此时磁盘不一定就绪
} usb_state_t;

typedef enum {
  USB_SUB_STATE_UNMOUNT,  // 磁盘未就绪
  USB_SUB_STATE_MOUNTED,  // 磁盘已经就绪
  USB_SUB_STATE_READY,    // 磁盘可以进行读写
} usb_sub_state_t;

class USBHost : public CH376 {
 public:
  bool start();
  void Task();

  uint8_t getUsbTaskState();
  uint8_t regRd(uint8_t reg) { return 0x0; };

 private:
  void setUsbTaskState(uint8_t state);
  void setUsbTaskSubState(uint8_t subState);
};

class BulkStorage {
 public:
  BulkStorage(USBHost *usb) : usb(usb){};

  bool     LUNIsGood(uint8_t t);
  uint32_t GetCapacity(uint8_t lun);
  uint16_t GetSectorSize(uint8_t lun);
  uint8_t  Read(uint8_t lun, uint32_t addr, uint16_t bsize, uint8_t blocks, uint8_t *buf);
  uint8_t  Write(uint8_t lun, uint32_t addr, uint16_t bsize, uint8_t blocks, const uint8_t *buf);

  USBHost *usb;
};

extern USBHost     usb;
extern BulkStorage bulk;
