#ifndef STM4SERIAL_H
#define STM4SERIAL_H

#include "../F446Arduino.h"

#define CANOPEN_SERIAL  Serial2

#define CANOPEN_HEADER1 0x5A
#define CANOPEN_HEADER2 0xA5

enum CanSerialState : UNS8 {
  CAN_IDLE,
  CAN_HEADER1_SEEN,
  CAN_HEADER2_SEEN,
  CAN_WAIT_TELEGRAM,
};

#endif  // STM4SERIAL_H
