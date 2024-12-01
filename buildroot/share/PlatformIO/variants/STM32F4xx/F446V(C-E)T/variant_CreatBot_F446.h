/*
 *******************************************************************************
 * Copyright (c) 2020-2021, STMicroelectronics
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#pragma once

/*----------------------------------------------------------------------------
 *        STM32 pins number
 *----------------------------------------------------------------------------*/
                                        //| DIGITAL | FUNCTIONS
#define PA0                     0       //|   D0    |
#define PA1                     1       //|   D1    |
#define PA2                     2       //|   D2    | TX   (USART2)
#define PA3                     3       //|   D3    | RX   (USART2)
#define PA4                     4       //|   D4    | NSS  (SPI1)
#define PA5                     5       //|   D5    | SCK  (SPI1)
#define PA6                     6       //|   D6    | MISO (SPI1)
#define PA7                     7       //|   D7    | MOSI (SPI1)
#define PA8                     8       //|   D8    |
#define PA9                     9       //|   D9    | TX   (USART1)
#define PA10                    10      //|   D10   | RX   (USART1)
#define PA11                    11      //|   D11   | DM   (USB)
#define PA12                    12      //|   D12   | DP   (USB)
#define PA13                    13      //|   D13   | DIO  (SW)
#define PA14                    14      //|   D14   | CLK  (SW)
#define PA15                    15      //|   D15   |
#define PB0                     PIN_A0  //|   D16   | A0   (ADC1)
#define PB1                     PIN_A1  //|   D17   | A1   (ADC1)
#define PB2                     18      //|   D18   |
#define PB3                     19      //|   D19   | SCK  (SPI3)
#define PB4                     20      //|   D20   | MISO (SPI3)
#define PB5                     21      //|   D21   | MOSI (SPI3)
#define PB6                     22      //|   D22   | SCL  (I2C1)
#define PB7                     23      //|   D23   | SDA  (I2C1)
#define PB8                     24      //|   D24   |
#define PB9                     25      //|   D25   |
#define PB10                    26      //|   D26   |
#define PB12                    27      //|   D27   | RX   (CAN2)
#define PB13                    28      //|   D28   | TX   (CAN2)
#define PB14                    29      //|   D29   | CH2N (TIM8)
#define PB15                    30      //|   D30   | CH3N (TIM8)
#define PC0                     PIN_A2  //|   D31   | A2   (ADC1)
#define PC1                     PIN_A3  //|   D32   | A3   (ADC1)
#define PC2                     PIN_A4  //|   D33   | A4   (ADC1)
#define PC3                     PIN_A5  //|   D34   | A5   (ADC1)
#define PC4                     PIN_A6  //|   D35   | A6   (ADC1)
#define PC5                     PIN_A7  //|   D36   | A7   (ADC1)
#define PC6                     37      //|   D37   | CH1  (TIM3)
#define PC7                     38      //|   D38   | CH2  (TIM3)
#define PC8                     39      //|   D39   | CH3  (TIM3)
#define PC9                     40      //|   D40   | CH4  (TIM3)
#define PC10                    41      //|   D41   |
#define PC11                    42      //|   D42   |
#define PC12                    43      //|   D43   |
#define PC13                    44      //|   D44   |
#define PC14                    45      //|   D45   |
#define PC15                    46      //|   D46   |
#define PD0                     47      //|   D47   |
#define PD1                     48      //|   D48   |
#define PD2                     49      //|   D49   |
#define PD3                     50      //|   D50   |
#define PD4                     51      //|   D51   |
#define PD5                     52      //|   D52   |
#define PD6                     53      //|   D53   |
#define PD7                     54      //|   D54   |
#define PD8                     55      //|   D55   |
#define PD9                     56      //|   D56   |
#define PD10                    57      //|   D57   |
#define PD11                    58      //|   D58   |
#define PD12                    59      //|   D59   | CH1  (TIM4)
#define PD13                    60      //|   D60   | CH2  (TIM4)
#define PD14                    61      //|   D61   | CH3  (TIM4)
#define PD15                    62      //|   D62   | CH4  (TIM4)
#define PE0                     63      //|   D63   |
#define PE1                     64      //|   D64   |
#define PE2                     65      //|   D65   |
#define PE3                     66      //|   D66   |
#define PE4                     67      //|   D67   |
#define PE5                     68      //|   D68   | CH1  (TIM9)
#define PE6                     69      //|   D69   | CH2  (TIM9)
#define PE7                     70      //|   D70   |
#define PE8                     71      //|   D71   |
#define PE9                     72      //|   D72   |
#define PE10                    73      //|   D73   |
#define PE11                    74      //|   D74   |
#define PE12                    75      //|   D75   | SCK  (SPI4)
#define PE13                    76      //|   D76   | MISO (SPI4)
#define PE14                    77      //|   D77   | MOSI (SPI4)
#define PE15                    78      //|   D78   |
#define PH0                     79      //|   D79   | OSC IN
#define PH1                     80      //|   D80   | OSC OUT

// Alternate pins number
#define PA0_ALT1                (PA0  | ALT1)
#define PA0_ALT2                (PA0  | ALT2)
#define PA1_ALT1                (PA1  | ALT1)
#define PA1_ALT2                (PA1  | ALT2)
#define PA2_ALT1                (PA2  | ALT1)
#define PA2_ALT2                (PA2  | ALT2)
#define PA3_ALT1                (PA3  | ALT1)
#define PA3_ALT2                (PA3  | ALT2)
#define PA4_ALT1                (PA4  | ALT1)
#define PA5_ALT1                (PA5  | ALT1)
#define PA6_ALT1                (PA6  | ALT1)
#define PA7_ALT1                (PA7  | ALT1)
#define PA7_ALT2                (PA7  | ALT2)
#define PA7_ALT3                (PA7  | ALT3)
#define PA15_ALT1               (PA15 | ALT1)
#define PB0_ALT1                (PB0  | ALT1)
#define PB0_ALT2                (PB0  | ALT2)
#define PB1_ALT1                (PB1  | ALT1)
#define PB1_ALT2                (PB1  | ALT2)
#define PB3_ALT1                (PB3  | ALT1)
#define PB4_ALT1                (PB4  | ALT1)
#define PB5_ALT1                (PB5  | ALT1)
#define PB8_ALT1                (PB8  | ALT1)
#define PB8_ALT2                (PB8  | ALT2)
#define PB9_ALT1                (PB9  | ALT1)
#define PB9_ALT2                (PB9  | ALT2)
#define PB14_ALT1               (PB14 | ALT1)
#define PB14_ALT2               (PB14 | ALT2)
#define PB15_ALT1               (PB15 | ALT1)
#define PB15_ALT2               (PB15 | ALT2)
#define PC0_ALT1                (PC0  | ALT1)
#define PC0_ALT2                (PC0  | ALT2)
#define PC1_ALT1                (PC1  | ALT1)
#define PC1_ALT2                (PC1  | ALT2)
#define PC2_ALT1                (PC2  | ALT1)
#define PC2_ALT2                (PC2  | ALT2)
#define PC3_ALT1                (PC3  | ALT1)
#define PC3_ALT2                (PC3  | ALT2)
#define PC4_ALT1                (PC4  | ALT1)
#define PC5_ALT1                (PC5  | ALT1)
#define PC6_ALT1                (PC6  | ALT1)
#define PC7_ALT1                (PC7  | ALT1)
#define PC8_ALT1                (PC8  | ALT1)
#define PC9_ALT1                (PC9  | ALT1)
#define PC10_ALT1               (PC10 | ALT1)
#define PC11_ALT1               (PC11 | ALT1)

#define NUM_DIGITAL_PINS        81
#define NUM_ANALOG_INPUTS       8

// On-board LED pin number
#ifndef LED_BUILTIN
  #define LED_BUILTIN           PA1
#endif

// On-board user button
#ifndef USER_BTN
  #define USER_BTN              PNUM_NOT_DEFINED
#endif

// SPI definitions
#ifndef PIN_SPI_SS
  #define PIN_SPI_SS            PA4
#endif
#ifndef PIN_SPI_SS1
  #define PIN_SPI_SS1           PNUM_NOT_DEFINED
#endif
#ifndef PIN_SPI_SS2
  #define PIN_SPI_SS2           PNUM_NOT_DEFINED
#endif
#ifndef PIN_SPI_SS3
  #define PIN_SPI_SS3           PNUM_NOT_DEFINED
#endif
#ifndef PIN_SPI_MOSI
  #define PIN_SPI_MOSI          PA7
#endif
#ifndef PIN_SPI_MISO
  #define PIN_SPI_MISO          PA6
#endif
#ifndef PIN_SPI_SCK
  #define PIN_SPI_SCK           PA5
#endif

// I2C definitions
#ifndef PIN_WIRE_SDA
  #define PIN_WIRE_SDA          PB7
#endif
#ifndef PIN_WIRE_SCL
  #define PIN_WIRE_SCL          PB6
#endif

// Timer Definitions
// Use TIM6/TIM7 when possible as servo and tone don't need GPIO output pin
#ifndef TIMER_TONE
  #define TIMER_TONE            TIM6
#endif
#ifndef TIMER_SERVO
  #define TIMER_SERVO           TIM7
#endif
#ifndef TIMER_SERIAL
  #define TIMER_SERIAL          TIM11
#endif

// UART Definitions
#ifndef SERIAL_UART_INSTANCE
  #define SERIAL_UART_INSTANCE  1
#endif

#define ENABLE_HWSERIAL2

// Default pin used for generic 'Serial' instance
// Mandatory for Firmata
#ifndef PIN_SERIAL_RX
  #define PIN_SERIAL_RX         PA10
#endif
#ifndef PIN_SERIAL_TX
  #define PIN_SERIAL_TX         PA9
#endif

// Extra HAL modules
#if !defined(HAL_DAC_MODULE_DISABLED)
  #define HAL_DAC_MODULE_ENABLED
#endif
#if !defined(HAL_QSPI_MODULE_DISABLED)
  #define HAL_QSPI_MODULE_ENABLED
#endif
#if !defined(HAL_SD_MODULE_DISABLED)
  #define HAL_SD_MODULE_ENABLED
#endif


// PWM resolution
#define PWM_FREQUENCY           20000 // >= 20 Khz => inaudible noise for fans
#define PWM_MAX_DUTY_CYCLE      255

#define HSE_VALUE               12000000U

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus
  // These serial port names are intended to allow libraries and architecture-neutral
  // sketches to automatically default to the correct port name for a particular type
  // of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
  // the first hardware serial port whose RX/TX pins are not dedicated to another use.
  //
  // SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
  //
  // SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
  //
  // SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
  //
  // SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
  //
  // SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
  //                            pins are NOT connected to anything by default.
  #ifndef SERIAL_PORT_MONITOR
    #define SERIAL_PORT_MONITOR   Serial
  #endif
  #ifndef SERIAL_PORT_HARDWARE
    #define SERIAL_PORT_HARDWARE  Serial
  #endif
#endif


/*----------------------------------------------------------------------------
 *        Arduino objects - BootLoader
 *----------------------------------------------------------------------------*/
  
#define BOOTLODER_CHEK_ADDR    (*((uint32_t *)0x2001FFFC))
#define BOOTLODER_CHEK_VAL     (0xDEADBEEF)
#define BOOTLODER_Into_ADDR    (*((uint32_t *) (0x1FFF0004)))
#define REBOOT_PIN             PB9

#ifdef __cplusplus
extern "C" {
#endif
  void SystemResetToBootloader();
#ifdef __cplusplus
}
#endif