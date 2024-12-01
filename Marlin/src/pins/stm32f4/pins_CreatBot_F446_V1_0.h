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

#include "env_validate.h"

#if HOTENDS > 2 || E_STEPPERS > 2
  #error "CreatBot F446 V1.0 support up to 2 hotends / E-steppers."
#endif

#ifndef BOARD_INFO_NAME
  #define BOARD_INFO_NAME "CreatBot F446 V1.0"
#endif

#define DEFAULT_MACHINE_NAME BOARD_INFO_NAME

#define STEP_TIMER 10
#define TEMP_TIMER 14

//
// EEPROM Settings
//
#if NO_EEPROM_SELECTED
  #define I2C_EEPROM
  #define MARLIN_EEPROM_SIZE                0x1000  // 32KB (AT24C32)
#endif

//
// Limit Switches
//
#define X_MIN_PIN                           PC10    //EXTI10
#define Y_MIN_PIN                           PC11    //EXTI11
#define Z_MIN_PIN                           PC12    //EXTI12
#define X_MAX_PIN                           PC10    //EXTI10
#define Y_MAX_PIN                           PC11    //EXTI11
#define Z_MAX_PIN                           PC12    //EXTI12

//
// Filament Runout Sensor
//
#define FIL_RUNOUT_PIN                      PC14    // EXTI14
#define FIL_RUNOUT2_PIN                     PC15    // EXTI15

//
// Steppers
//
#define X_STEP_PIN                          PD1
#define X_DIR_PIN                           PD0
#define X_ENABLE_PIN                        PA15

#define Y_STEP_PIN                          PD4
#define Y_DIR_PIN                           PD3
#define Y_ENABLE_PIN                        PD2

#define Z_STEP_PIN                          PD7
#define Z_DIR_PIN                           PD6
#define Z_ENABLE_PIN                        PD5

#define E0_STEP_PIN                         PD9
#define E0_DIR_PIN                          PD10
#define E0_ENABLE_PIN                       PD11

#define E1_STEP_PIN                         PE15
#define E1_DIR_PIN                          PB10
#define E1_ENABLE_PIN                       PD8

#define X_CS_PIN                            PE2
#define Y_CS_PIN                            PE3
#define Z_CS_PIN                            PE4
#define E0_CS_PIN                           PE0
#define E1_CS_PIN                           PE1

#if HAS_TMC_SPI
#if ENABLED(TMC_USE_SW_SPI)
  #if !defined(TMC_SPI_MOSI) || TMC_SPI_MOSI == -1
    #define TMC_SPI_MOSI                    PB5
  #endif
  #if !defined(TMC_SPI_MISO) || TMC_SPI_MISO == -1
    #define TMC_SPI_MISO                    PB4
  #endif
  #if !defined(TMC_SPI_SCK) || TMC_SPI_SCK == -1
    #define TMC_SPI_SCK                     PB3
  #endif
  #endif
#endif

#if HAS_TMC_UART
  #define X_SERIAL_TX_PIN                     PE7
  #define Y_SERIAL_TX_PIN                     PE8
  #define Z_SERIAL_TX_PIN                     PE9
  #define E0_SERIAL_TX_PIN                    PE10
  #define E1_SERIAL_TX_PIN                    PE11
  #define X_SERIAL_RX_PIN                     PE2
  #define Y_SERIAL_RX_PIN                     PE3
  #define Z_SERIAL_RX_PIN                     PE4
  #define E0_SERIAL_RX_PIN                    PE0
  #define E1_SERIAL_RX_PIN                    PE1
#endif

//
// Temperature Sensors
//
#define TEMP_BED_PIN                        PC0   //ADC1_IN10
#define TEMP_0_PIN                          PC1   //ADC1_IN11
#define TEMP_1_PIN                          PC2   //ADC1_IN12
#define TEMP_C_PIN                          PC3   //ADC1_IN13   Chamber
#define TEMP_F_PIN                          PC5   //ADC1_IN15   Fliament  TODO check this 
#define TEMP_W_PIN                          PB0   //ADC1_IN8    Water
#define TEMP_R_PIN                          PB1   //ADC1_IN9    Reserve

#define TEMP_CHAMBER_PIN                    TEMP_C_PIN

// Temperature Sensors with SPI
#define TEMP_0_SCK_PIN                      PE12  //SPI4_SCK
#define TEMP_0_MISO_PIN                     PE13  //SPI4_MISO
#define TEMP_0_MOSI_PIN                     PE14  //SPI4_MOSI

#define TEMP_1_SCK_PIN                      TEMP_0_SCK_PIN
#define TEMP_1_MISO_PIN                     TEMP_0_MISO_PIN
#define TEMP_1_MOSI_PIN                     TEMP_0_MOSI_PIN

#define TEMP_0_CS_PIN                       TEMP_0_PIN
#define TEMP_1_CS_PIN                       TEMP_1_PIN


//
// Heaters / Fans
//
#define HEATER_BED_PIN                      PD12  //TIM4_CH1
#define HEATER_0_PIN                        PD13  //TIM4_CH2
#define HEATER_1_PIN                        PD14  //TIM4_CH3
#define HEATER_C_PIN                        PD15  //TIM4_CH4    Chamber
#define HEATER_F_PIN                        PB14  //TIM8_CH2N   Fliament
#define HEATER_R_PIN                        PB15  //TIM8_CH3N   Reserve
#define FAN_T_PIN                           PC6   //TIM3_CH1    Sink  (T Fan)
#define FAN_G_PIN                           PC7   //TIM3_CH2    Model (G Fan)
#define FAN_A_PIN                           PC8   //TIM3_CH3    Air
#define FAN_S_PIN                           PC9   //TIM3_CH4    System

#define HEATER_CHAMBER_PIN                  HEATER_C_PIN
#define E0_AUTO_FAN_PIN                     FAN_T_PIN
#define E1_AUTO_FAN_PIN                     FAN_T_PIN
#define FAN0_PIN                            FAN_G_PIN
#define FAN1_PIN                            FAN_A_PIN
#define FAN2_PIN                            FAN_S_PIN

//
// SPI
//
#define UDISK_SCK_PIN                       PA5
#define UDISK_MISO_PIN                      PA6
#define UDISK_MOSI_PIN                      PA7
#define UDISK_SS_PIN                        PA4
#define UDISK_INT_PIN                       PC4

//
// Power
//
#define KILL_PIN                            PA0   //EXTI0
#define SUICIDE_PIN                         PA8
#define PS_ON_PIN                           PA8
#define POWER_LOSS_PIN                      PC13  //EXTI13
#define SUICIDE_PIN_STATE                   HIGH  //power off while high level
#define POWER_LOSS_STATE                    LOW   //power loss while low level

//
// Misc. Functions
//
#define LED_PIN                             PA1
#define BEEPER_PIN                          PB2

#define LCD_TX                              PA2
#define LCD_RX                              PA3
#define WIFI_TX                             PA9
#define WIFI_RX                             PA10


//
// Servos
//
#define SERVO0_PIN                          PE5
#define SERVO1_PIN                          PE6

//
// I2C (EEPROM)
// Comment this, because it's incompatible with Wire Lib.
// Function "Wire.begin()" has overloading problem.
// Default use PIN_WIRE that defined in varinant file.
//
// #define I2C_SCL_PIN                         PB6   //I2C1_SCL
// #define I2C_SDA_PIN                         PB7   //I2C1_SDA

//
// LED
//
#define NEOPIXEL_PIN                        PB8

// RESERVE
#define TEST_PIN                            PB9
#define CAN_RX                              PB12  //CAN2_RX
#define CAN_TX                              PB13  //CAN2_TX

//
// System Functions (Just write here, DO NOT USE THEM)
//
// #define USB_DM                              PA11
// #define USB_DP                              PA12
// #define SWDIO                               PA13
// #define SWCLK                               PA14

#define BOARD_PREINIT() {         \
  SET_OUTPUT(HEATER_BED_PIN);     \
  SET_OUTPUT(HEATER_0_PIN);       \
  SET_OUTPUT(HEATER_1_PIN);       \
  SET_OUTPUT(HEATER_C_PIN);       \
  SET_OUTPUT(FAN_T_PIN);          \
  SET_OUTPUT(FAN_G_PIN);          \
  SET_OUTPUT(FAN_A_PIN);          \
  SET_OUTPUT(FAN_S_PIN);          \
  OUT_WRITE(X_ENABLE_PIN, HIGH);  \
  OUT_WRITE(Y_ENABLE_PIN, HIGH);  \
  OUT_WRITE(Z_ENABLE_PIN, HIGH);  \
}
