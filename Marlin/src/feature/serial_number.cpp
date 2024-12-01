/**
 *  serial_number.cpp
 *
 *  Created on: 2024/02/19
 *      Author: CreatBot LYN
 */

#include "../inc/MarlinConfigPre.h"

#if ENABLED(HAS_CHIP_ID)
  #include "serial_number.h"

  #ifdef USE_DS2401
uint32_t getChipID32b() { return 0; }
uint64_t getChipID64b() { return 0; }
  #elif defined(STM32F446xx)

    #define DEVICE_ID_BASE_ADDERSS 0X1FFF7A10  // STM32F446的唯一标识符地址

static uint8_t chipLot  = 0;  // 芯片批号
static uint8_t chipPosX = 0;  // 晶元X坐标
static uint8_t chipPosY = 0;  // 晶元Y坐标
static uint8_t chipNum  = 0;  // 晶元流水号

static void getChipIDInfo() {
  u_chipId chipId;

  // 读取芯片96位唯一ID
  chipId.chipId_32[0] = *(uint32_t *)(DEVICE_ID_BASE_ADDERSS);
  chipId.chipId_32[1] = *(uint32_t *)(DEVICE_ID_BASE_ADDERSS + 4);
  chipId.chipId_32[2] = *(uint32_t *)(DEVICE_ID_BASE_ADDERSS + 8);

  // 计算芯片批号掩码
  chipLot = 0;
  for (uint8_t i = 5; i < 12; i++) {  // 芯片ID批号 高7个字节位与运算
    chipLot += chipId.chipId_8[i];
  }

  // 取得芯片晶元信息
  chipPosX = chipId.chipId_8[0];  // 晶元坐标X/Y, 16位, 高位总是0x00
  chipPosY = chipId.chipId_8[2];  // 晶元坐标X/Y, 16位, 高位总是0x00
  chipNum  = chipId.chipId_8[4];  // 晶元流水号, 8位
}

uint32_t getChipID32b() {
  getChipIDInfo();

  // 合成32位ID信息
  u_chipId chipId;
  chipId.chipId_8[3] = chipLot;
  chipId.chipId_8[2] = chipPosX;
  chipId.chipId_8[1] = chipPosY;
  chipId.chipId_8[0] = chipNum;

  return chipId.chipId_32[0];
}

uint64_t getChipID64b() {
  getChipIDInfo();

  // 合成64位ID信息
  u_chipId chipId;
  chipId.chipId_8[7] = 0x00;
  chipId.chipId_8[6] = 0x00;
  chipId.chipId_8[5] = chipPosX;
  chipId.chipId_8[4] = chipPosY;
  chipId.chipId_8[3] = chipNum;
  chipId.chipId_8[2] = chipLot ^ chipPosX;
  chipId.chipId_8[1] = chipLot ^ chipPosY;
  chipId.chipId_8[0] = chipLot ^ chipNum;

  return chipId.chipId_64[0];
}
  #else
uint32_t getChipID32b() { return 0; }
uint64_t getChipID64b() { return 0; }
  #endif

#endif  // HAS_CHIP_ID
