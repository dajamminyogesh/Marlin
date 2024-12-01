/**
 *  serial_number.h
 *
 *  Created on: 2024/02/19
 *      Author: CreatBot LYN
 */
#pragma once

// support max 128bit chipID
// DS2401 use 64 bit
// STM32 use 96 bit
typedef union {
  uint8_t  chipId_8[16];
  uint32_t chipId_32[4];
  uint64_t chipId_64[2];
} u_chipId;

uint32_t getChipID32b();
uint64_t getChipID64b();
