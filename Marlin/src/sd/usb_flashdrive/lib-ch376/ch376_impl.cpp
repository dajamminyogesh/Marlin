/*
 * ch376_impl.cpp
 *
 *  Created on: 2022/04/13
 *      Author: CreatBot LYN
 *
 *  CH376 communication interface
 */

#include "../../../inc/MarlinConfigPre.h"

#if BOTH(USB_FLASH_DRIVE_SUPPORT, USE_CH376)

  #include "../../../HAL/shared/Delay.h"
  #include "../../../inc/MarlinConfig.h"
  #include "ch376_cmd.h"
  #include "ch376_impl.h"

  #if PIN_EXISTS(UDISK_RST)
    #undef CH376_RST_PIN
    #define CH376_RST_PIN UDISK_RST_PIN
  #endif
  #if PIN_EXISTS(UDISK_BUSY)
    #undef CH376_BUSY_PIN
    #define CH376_BUSY_PIN UDISK_BUSY_PIN
  #endif
  #if PIN_EXISTS(UDISK_INT)
    #undef CH376_INT_PIN
    #define CH376_INT_PIN UDISK_INT_PIN
  #endif

  #if CH376_USE_BUSY_PIN
    #define BUSY_CHECK while (READ(CH376_BUSY_PIN))
  #else
    #define CH376_BUSY_DELAY 1500  // ns = 1.5us
    #define BUSY_CHECK       DELAY_NS(CH376_BUSY_DELAY)
  #endif

static uint8_t ch376_state = CH376_STATUS_UNINIT;

  #ifdef CH376_USE_SPI

    #if PIN_EXISTS(UDISK_MOSI)
      #undef CH376_MOSI_PIN
      #define CH376_MOSI_PIN UDISK_MOSI_PIN
    #endif

    #if PIN_EXISTS(UDISK_MISO)
      #undef CH376_MISO_PIN
      #define CH376_MISO_PIN UDISK_MISO_PIN
    #endif

    #if PIN_EXISTS(UDISK_SS)
      #undef CH376_CS_PIN
      #define CH376_CS_PIN UDISK_SS_PIN
    #endif

    #if PIN_EXISTS(UDISK_SCK)
      #undef CH376_CLK_PIN
      #define CH376_CLK_PIN UDISK_SCK_PIN
    #endif

FORCE_INLINE void CH376::enableChipSelect() { WRITE(CH376_CS_PIN, LOW); }

FORCE_INLINE void CH376::disableChipSelect() { WRITE(CH376_CS_PIN, HIGH); }

void CH376::setupPins() {
    #if CH376_USE_RST_PIN && PIN_EXISTS(CH376_RST)
  SET_OUTPUT(CH376_RST_PIN);
    #endif
    #if CH376_USE_BUSY_PIN && PIN_EXISTS(CH376_BUSY)
  SET_INPUT_PULLDOWN(CH376_BUSY_PIN);
    #endif
    #if CH376_USE_INT_PIN && PIN_EXISTS(CH376_INT)
  SET_INPUT_PULLUP(CH376_INT_PIN);
    #endif
}

// 发送单个命令
void CH376::sendCmd(const uint8_t cmd) {
  enableChipSelect();
  spiSend(cmd);
  BUSY_CHECK;
}

// 发送带参数的命令，其中argn为参数个数
void CH376::sendCmd(const uint8_t cmd, uint8_t argn, ...) {
  enableChipSelect();
  spiSend(cmd);
  BUSY_CHECK;

  if (argn < 1) return;

  va_list ap;
  va_start(ap, argn);

  // argn--;
  // spiSend(va_arg(ap, int));
  // BUSY_CHECK;

  while (argn--) {
    spiSend(va_arg(ap, int));
  }
  va_end(ap);
}

// 发送指定长度的数据
void CH376::sendData(const uint8_t *data, const uint8_t length) {
  int n = length, i = 0;
  while (n--) {
    spiSend(data[i++]);
  }
}

// 发送一个字节
void CH376::sendByte(const uint8_t b) { spiSend(b); }

// 接收一个字节
uint8_t CH376::recvByte() {
  uint8_t result = spiRec();
  BUSY_CHECK;
  return result;
}

// 接收四个字节
uint32_t CH376::recvWord() {
  uint32_t result = spiRec();
  BUSY_CHECK;
  result |= ((uint16_t)spiRec() << 8 | (uint32_t)spiRec() << 16 | (uint32_t)spiRec() << 24);
  return result;
}

// 接收指定长度的数据
void CH376::recvData(uint8_t *data, const uint8_t length) {
  int n = length;
  while (n--) {
    *data = spiRec();
    if (n) data++;
  }
}

// 命令传输结束
void CH376::endCmd() { disableChipSelect(); }

// 查询是否有中断产生
bool CH376::queryInterrupt() {
    #if CH376_USE_INT_PIN
  return READ(CH376_INT_PIN) ? false : true;
    #else
  return READ(CH376_MISO_PIN) ? false : true;  // SDO
    #endif
}

// 获取中断状态并取消中断请求
uint8_t CH376::getInterrupt() {
  uint8_t intState;
  sendCmd(CMD01_GET_STATUS);
  intState = recvByte();
  endCmd();
  return intState;
}

// 获取芯片状态
uint8_t CH376::getCh376State() { return ch376_state; }

// 执行硬件复位
void CH376::resetChip() {
  sendCmd(CMD00_RESET_ALL);
  endCmd();
}

// 测试通讯接口
bool CH376::checkChip() {
  uint8_t testData = random(0xFF);

  sendCmd(CMD11_CHECK_EXIST, 1, testData);
  uint8_t result = recvByte();
  endCmd();

  if (testData + result == 0xFF)
    return true;
  else
    return false;
}

// 是否设置SPI的SDO引脚为中断方式
void CH376::setSDOInt(bool allow) {
  sendCmd(CMD20_SET_SDO_INT, 2, 0x16, allow ? 0x90 : 0x10);
  endCmd();
}

// 设置工作模式
bool CH376::setWorkMode(uint8_t mode) {
  sendCmd(CMD11_SET_USB_MODE, 1, mode);
  DELAY_US(15);  // delay 15us.
  uint8_t result = recvByte();
  endCmd();

  if (result == CMD_RET_SUCCESS)
    return true;
  else
    return false;
}

// 测试设备连接性
uint8_t CH376::testDevice() {
  sendCmd(CMD01_TEST_CONNECT);
  uint8_t result = recvByte();
  endCmd();
  return result;
}

// 自动配置设备(返回中断)
void CH376::autoSetup() {
  sendCmd(CMD0H_AUTO_SETUP);
  endCmd();
}

// 尝试初始化USB存储设备(返回中断)
void CH376::diskInit() {
  sendCmd(CMD0H_DISK_INIT);
  endCmd();
}

// 尝试加载USB存储设备(返回中断)
void CH376::diskMount() {
  sendCmd(CMD0H_DISK_MOUNT);
  endCmd();
}

// 获取USB存储容量(返回中断)
void CH376::diskSize() {
  sendCmd(CMD0H_DISK_SIZE);
  endCmd();
}

// 初始化USB文件系统(返回中断)
void CH376::diskFileSysInit() {
  sendCmd(CMD10_SET_FILE_NAME);
  sendByte(0);  // 空字符表示初始化文件系统
  endCmd();
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();
}

// 读取USB磁盘指定物理地址的指定长度数据
void CH376::diskRead(uint32_t addr, uint8_t blocks) {
  sendCmd(CMD5H_DISK_READ, 5, (uint8_t)(addr), (uint8_t)(addr >> 8), (uint8_t)(addr >> 16), (uint8_t)(addr >> 24),
          blocks);
  endCmd();
}

// 继续操作USB磁盘读取
void CH376::diskReadGo() {
  sendCmd(CMD0H_DISK_RD_GO);
  endCmd();
}

// 写入USB磁盘指定物理地址的指定长度数据
void CH376::diskWrite(uint32_t addr, uint8_t blocks) {
  sendCmd(CMD5H_DISK_WRITE, 5, (uint8_t)(addr), (uint8_t)(addr >> 8), (uint8_t)(addr >> 16), (uint8_t)(addr >> 24),
          blocks);
  endCmd();
}

// 继续操作USB磁盘写入
void CH376::diskWriteGo() {
  sendCmd(CMD0H_DISK_WR_GO);
  endCmd();
}

// 从当前缓冲区读取数据块,返回长度
uint8_t CH376::readData0(uint8_t *buf) {
  sendCmd(CMD01_RD_USB_DATA0);
  uint8_t n = recvByte();
  recvData(buf, n);
  endCmd();
  return n;
}

// 向USB主机端点的发送缓冲区写入数据块
void CH376::writeData0(const uint8_t *buf, uint8_t len) {
  sendCmd(CMD10_WR_HOST_DATA, 1, len);
  sendData(buf, len);
  endCmd();
}

bool CH376::init() {
  setupPins();
  disableChipSelect();

  spiBegin();
  spiInit(SD_SPI_SPEED);

  // 检测不到芯片
  if (!checkChip()) {
    ch376_state = CH376_STATUS_ERROR_COMM;
    resetChip();
    return false;
  }

    #if CH376_USE_INT_PIN
  setSDOInt(false);  // 设置中断方式为中断引脚
    #else
  setSDOInt(true);                             // 设置中断方式为SDO引脚
    #endif

  // 取消可能存在的中断
  if (queryInterrupt()) {
    getInterrupt();
    // 中断信号仍然存在
    if (queryInterrupt()) {
      ch376_state = CH376_STATUS_ERROR_INT;
      resetChip();
      return false;
    }
  }

  // 初始化设置模式为 USB主机模式不发送SOF
  if (!setWorkMode(CH376_MODE_USB_HOST_READY_NO_SOF)) {
    ch376_state = CH376_STATUS_ERROR_MODE;
    resetChip();
    return false;
  }

  ch376_state = CH376_STATUS_READY;

  return true;
}

  #endif  // CH376_USE_SPI

#endif  // USB_FLASH_DRIVE_SUPPORT
