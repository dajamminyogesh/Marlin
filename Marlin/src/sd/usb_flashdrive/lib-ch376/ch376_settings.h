/*
 * ch376_settings.h
 *
 *  Created on: 2022/04/13
 *      Author: CreatBot LYN
 *
 *  CH376 communication settings
 */
#pragma once

#include "../../../inc/MarlinConfig.h"

#define CH376_USE_SPI
// #define CH376_USE_UART
// #define CH376_USE_PART

#if MANY(CH376_USE_SPI, CH376_USE_UART, CH376_USE_PART)
  #error "Please choose one connect mode for CH376."
#endif

#define CH376_RST_PIN      -1
#define CH376_BUSY_PIN     -1
#define CH376_INT_PIN      -1

#define CH376_USE_RST_PIN  false
#define CH376_USE_BUSY_PIN false
#define CH376_USE_INT_PIN  true

#ifdef CH376_USE_SPI
  #define CH376_MOSI_PIN UDISK_MOSI_PIN
  #define CH376_MISO_PIN UDISK_MISO_PIN
  #define CH376_CS_PIN   UDISK_SS_PIN
  #define CH376_CLK_PIN  UDISK_SCK_PIN

#endif  // CH376_USE_SPI

#ifdef CH376_USE_UART
  #error "todo this."
#endif

#ifdef CH376_USE_PART
  #error "todo this."
#endif
