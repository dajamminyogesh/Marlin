/*
 * usb_host.cpp
 *
 *  Created on: 2022/04/12
 *      Author: CreatBot LYN
 *
 *  USB interface use CH376S
 */

#include "../../../inc/MarlinConfigPre.h"

#if BOTH(USB_FLASH_DRIVE_SUPPORT, USE_CH376)

  #include "../../../inc/MarlinConfig.h"
  #include "usb_host.h"

  #define SWP32(A)                                                                                                     \
    ((((uint32_t)(A)&0xFF000000) >> 24) | (((uint32_t)(A)&0x00FF0000) >> 8) | (((uint32_t)(A)&0x0000FF00) << 8) |      \
     (((uint32_t)(A)&0x000000FF) << 24))

static CH376_CMD_DATA cmdData;

static uint8_t  usb_task_state;           // 主机状态
static uint8_t  usb_task_sub_state;       // 存储设备状态
static millis_t next_test_ms = millis();  // 设备测试定时

static uint32_t diskSectorNum;   // 磁盘的总扇区数
static uint32_t diskSectorSize;  // 磁盘的扇区大小

USBHost     usb;
BulkStorage bulk(&usb);

bool USBHost::start() { return init(); }

void USBHost::Task() {
  // 芯片未初始化完成
  uint8_t implState = getCh376State();
  if (implState != CH376_STATUS_READY) {
    // 芯片状态错误
    if (implState && CH376_STATUS_ERROR_MASK) {
      setUsbTaskState(USB_STATE_ERROR);
    }
    return;
  }

  // 同步主机状态和存储设备状态
  if (usb_task_state < USB_STATE_RUNNING) {
    setUsbTaskSubState(USB_SUB_STATE_UNMOUNT);
  }

  // CH376中断处理
  if (queryInterrupt()) {
    switch (getInterrupt()) {
      case USB_INT_CONNECT:                                         // 设备连接(0x15)
        if (usb_task_state == USB_STATE_IDLE) {                     // 模式05触发
          if (setWorkMode(CH376_MODE_USB_HOST_READY_RESET)) {       // 设置模式07
            if (setWorkMode(CH376_MODE_USB_HOST_READY_AUTO_SOF)) {  // 设置模式06
              setUsbTaskState(USB_STATE_RESET_DEVICE);
            }
          }
        } else if (usb_task_state == USB_STATE_RESET_DEVICE) {  // 模式06触发
          setUsbTaskState(USB_STATE_CONFIGURING);
          diskInit();  // 初始化设备
        }
        break;
      case USB_INT_DISCONNECT:                                // 设备断开(0x16)
        if (setWorkMode(CH376_MODE_USB_HOST_READY_NO_SOF)) {  // 设置模式05
          setUsbTaskState(USB_STATE_IDLE);
        }
        break;
      case USB_INT_SUCCESS:                             // 事务执行成功(0x14)
        if (usb_task_state == USB_STATE_CONFIGURING) {  // diskInit(是存储设备)
          setUsbTaskState(USB_STATE_RUNNING);
          setUsbTaskSubState(USB_SUB_STATE_UNMOUNT);
          diskMount();  // 初始化磁盘
        } else if (usb_task_state == USB_STATE_RUNNING) {
          if (usb_task_sub_state == USB_SUB_STATE_UNMOUNT) {  // diskMount(正确完成)
            setUsbTaskSubState(USB_SUB_STATE_MOUNTED);
            diskSize();                                              // 获取磁盘大小
          } else if (usb_task_sub_state == USB_SUB_STATE_MOUNTED) {  // diskSize(正确完成)
            setUsbTaskSubState(USB_SUB_STATE_READY);
            readData0((uint8_t *)&cmdData);
            diskSectorNum  = SWP32(cmdData.DiskSize.mDiskSizeSec);
            diskSectorSize = SWP32(cmdData.DiskSize.mSectorSize);
          } else if (usb_task_sub_state == USB_SUB_STATE_READY) {
            SERIAL_ECHOLNPGM("Strange!");
          }
        }
        break;
      case USB_INT_BUF_OVER:  // 磁盘传输数据错误(0x17)
        break;
      case USB_INT_DISK_READ:  // 磁盘请求数据读出(0x1D)
        break;
      case USB_INT_DISK_WRITE:  // 磁盘请求数据写入(0x1E)
        break;
      case USB_INT_DISK_ERR:                               // 磁盘操作失败(0x1F)
        if (usb_task_state == USB_STATE_CONFIGURING) {     // diskInit(不是存储设备)
          setUsbTaskState(USB_STATE_IDLE);                 // 设置为空闲,等待换设备
        } else if (usb_task_state == USB_STATE_RUNNING) {  // diskMount(可能是读卡器)
          setUsbTaskSubState(USB_SUB_STATE_UNMOUNT);       // 磁盘需要重新加载
          next_test_ms = millis() + 200;                   // 每200毫秒测试一次
        }
        break;
      default: break;
    }
  } else {  // 防止中断针脚异常,定时测试设备连接状态
    if (usb_task_state == USB_STATE_IDLE || usb_task_state == USB_STATE_RUNNING) {
      if (ELAPSED(millis(), next_test_ms)) {
        next_test_ms = millis() + 2000;  // 默认每两秒测试一次

        // 测试设备连接状态
        switch (testDevice()) {
          case USB_INT_CONNECT:  // 设备连接但是没有初始化(0x15)
            if (setWorkMode(CH376_MODE_USB_HOST_READY_AUTO_SOF)) {
              setUsbTaskState(USB_STATE_RESET_DEVICE);
            }
            break;
          case USB_INT_DISCONNECT:  // 设备已经断开(0x16)
            if (usb_task_state != USB_STATE_IDLE) {
              if (setWorkMode(CH376_MODE_USB_HOST_READY_NO_SOF)) {
                setUsbTaskState(USB_STATE_IDLE);
              }
            }
            break;
          case USB_INT_USB_READY:  // 设备已经初始化(0x18)
            if (usb_task_state == USB_STATE_RUNNING) {
              if (usb_task_sub_state == USB_SUB_STATE_UNMOUNT) {
                diskMount();  // 测试磁盘连接状态
              }
            }
            break;
          default: break;
        }
      }
    }
  }
}

uint8_t USBHost::getUsbTaskState() { return usb_task_state; }

void USBHost::setUsbTaskState(uint8_t state) { usb_task_state = state; }

void USBHost::setUsbTaskSubState(uint8_t subState) { usb_task_sub_state = subState; }

bool BulkStorage::LUNIsGood(uint8_t t) {
  if (usb_task_state == USB_STATE_RUNNING)
    return usb_task_sub_state == USB_SUB_STATE_READY;
  else
    return false;
};

uint32_t BulkStorage::GetCapacity(uint8_t lun) {
  if ((usb_task_state == USB_STATE_RUNNING) && (usb_task_sub_state == USB_SUB_STATE_READY))
    return diskSectorNum;
  else
    return 0;
};

uint16_t BulkStorage::GetSectorSize(uint8_t lun) {
  if ((usb_task_state == USB_STATE_RUNNING) && (usb_task_sub_state == USB_SUB_STATE_READY))
    return diskSectorSize;
  else
    return 0;
}

uint8_t BulkStorage::Read(uint8_t lun, uint32_t addr, uint16_t bsize, uint8_t blocks, uint8_t *buf) {
  usb->diskRead(addr, blocks);
  while (true) {
    while (!usb->queryInterrupt()) continue;

    uint8_t intCode = usb->getInterrupt();
    if (intCode == USB_INT_SUCCESS) {
      return 0;
    } else if (intCode == USB_INT_DISK_READ) {
      usb->readData0(buf);
      usb->diskReadGo();
      buf += 64;
    } else {
      return intCode;
    }
  }
}

uint8_t BulkStorage::Write(uint8_t lun, uint32_t addr, uint16_t bsize, uint8_t blocks, const uint8_t *buf) {
  usb->diskWrite(addr, blocks);
  while (true) {
    while (!usb->queryInterrupt()) continue;

    uint8_t intCode = usb->getInterrupt();
    if (intCode == USB_INT_SUCCESS) {
      return 0;
    } else if (intCode == USB_INT_DISK_WRITE) {
      usb->writeData0(buf, 64);
      usb->diskWriteGo();
      buf += 64;
    } else {
      return intCode;
    }
  }
}

#endif  // USB_FLASH_DRIVE_SUPPORT