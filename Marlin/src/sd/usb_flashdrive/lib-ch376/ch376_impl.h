/*
 * ch376_impl.h
 *
 *  Created on: 2022/04/13
 *      Author: CreatBot LYN
 *
 *  CH376 communication interface
 */
#pragma once

#include "ch376_cmd.h"
#include "ch376_settings.h"

/* CH376 工作模式 */
#define CH376_MODE_USB_DEV_NO_READY        0x00
#define CH376_MODE_USB_DEV_READY_EXT_FW    0x01
#define CH376_MODE_USB_DEV_READY_SELF_FW   0x02
#define CH376_MODE_SD_HOST                 0x03
#define CH376_MODE_USB_HOST_NO_READY       0x04
#define CH376_MODE_USB_HOST_READY_NO_SOF   0x05
#define CH376_MODE_USB_HOST_READY_AUTO_SOF 0x06
#define CH376_MODE_USB_HOST_READY_RESET    0x07

/* CH376 状态代码 */
#define CH376_STATUS_UNINIT     0x00
#define CH376_STATUS_READY      0x01
#define CH376_STATUS_ERROR_COMM 0x11
#define CH376_STATUS_ERROR_MODE 0x12
#define CH376_STATUS_ERROR_INT  0x13

#define CH376_STATUS_ERROR_MASK 0x10

/*************************************************/

#ifdef CH376_USE_SPI

class CH376 {
 public:
  bool init();

  bool    queryInterrupt();              // 查询是否有中断产生
  uint8_t getInterrupt();                // 获取中断状态并取消中断请求
  uint8_t getCh376State();               // 获取芯片状态
  void    resetChip();                   // 执行硬件复位
  bool    checkChip();                   // 测试通讯接口
  void    setSDOInt(bool allow = true);  // 是否设置SPI的SDO引脚为中断方式
  bool    setWorkMode(uint8_t mode);     // 设置工作模式
  uint8_t testDevice();                  // 测试设备连接性
  void    autoSetup();                   // 自动配置设备(返回中断)
  void    diskInit();                    // 初始化USB存储设备(返回中断)
  void    diskMount();                   // 加载USB存储设备(返回中断)
  void    diskSize();                    // 获取USB存储容量(返回中断)
  void    diskFileSysInit();             // 初始化USB文件系统(返回中断)

  void diskRead(uint32_t addr, uint8_t blocks);   // 读取USB磁盘指定物理地址的指定长度数据
  void diskReadGo();                              // 继续操作USB磁盘读取
  void diskWrite(uint32_t addr, uint8_t blocks);  // 写入USB磁盘指定物理地址的指定长度数据
  void diskWriteGo();                             // 继续操作USB磁盘写入

  uint8_t readData0(uint8_t *buf);                      // 从当前缓冲区读取数据块,返回长度
  void    writeData0(const uint8_t *buf, uint8_t len);  // 向USB主机端点的发送缓冲区写入数据块

 private:
  void setupPins();
  void enableChipSelect();
  void disableChipSelect();

  void     sendCmd(const uint8_t cmd);
  void     sendCmd(const uint8_t cmd, uint8_t argn, ...);
  void     sendData(const uint8_t *data, const uint8_t length);
  void     sendByte(const uint8_t b);
  uint8_t  recvByte();
  uint32_t recvWord();
  void     recvData(uint8_t *data, const uint8_t length);
  void     endCmd();
};

#endif  // CH376_USE_SPI

#ifdef CH376_USE_UART
  #error "todo this."
#endif

#ifdef CH376_USE_PART
  #error "todo this."
#endif
