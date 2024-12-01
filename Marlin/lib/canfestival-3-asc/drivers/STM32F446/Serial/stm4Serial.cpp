#include <config.h>

#ifdef DRIVER_USE_SERIAL

  #include "../../../inc/data.h"
  #include "../../../inc/timer.h"
  #include "canfestival.h"
  #include "stm4Serial.h"

  #ifdef __cplusplus
extern "C" {
  #endif  // __cplusplus

  static CO_Data       *co_data    = NULL;
  static UNS8           revLength  = 0;
  static CanSerialState canRxState = CAN_IDLE;
  static TIMER_HANDLE   runTimer   = TIMER_NONE;

  static void canLoop(CO_Data *d, UNS32 id);

  void canClose(void) {
    DelAlarm(runTimer);
    co_data    = NULL;
    revLength  = 0;
    canRxState = CAN_IDLE;
    runTimer   = TIMER_NONE;
  }

  UNS8 canInit(CO_Data *d, uint32_t bitrate) {
    co_data = d;
    CANOPEN_SERIAL.begin(bitrate);
    initTimer();

    DelAlarm(runTimer);
    runTimer = SetAlarm(co_data, 0, &canLoop, MS_TO_TIMEVAL(500), US_TO_TIMEVAL(500));

    return 0;
  }

  UNS8 canSend(CAN_PORT notused, Message *m) {
    if (m->len > 8) return 1;

    CANOPEN_SERIAL.write(CANOPEN_HEADER1);
    CANOPEN_SERIAL.write(CANOPEN_HEADER2);
    CANOPEN_SERIAL.write(m->len + 3);
    CANOPEN_SERIAL.write((uint8_t *)&m->cob_id, 2);
    CANOPEN_SERIAL.write(m->rtr);
    CANOPEN_SERIAL.write(m->data, m->len);

    return 0;
  }

  static void canLoop(CO_Data *d, UNS32 id) {
    while (CANOPEN_SERIAL.available()) {
      switch (canRxState) {
        case CAN_IDLE:
          if (CANOPEN_SERIAL.read() == CANOPEN_HEADER1) canRxState = CAN_HEADER1_SEEN;
          break;
        case CAN_HEADER1_SEEN:
          if (CANOPEN_SERIAL.read() == CANOPEN_HEADER2)
            canRxState = CAN_HEADER2_SEEN;
          else
            canRxState = CAN_IDLE;
          break;
        case CAN_HEADER2_SEEN:
          revLength = CANOPEN_SERIAL.read();
          if (revLength > 11 || revLength < 3)
            canRxState = CAN_IDLE;
          else
            canRxState = CAN_WAIT_TELEGRAM;
          break;
        case CAN_WAIT_TELEGRAM:
          if (CANOPEN_SERIAL.available() < revLength) return;

          Message rxm = {0};
          CANOPEN_SERIAL.readBytes((UNS8 *)&rxm.cob_id, 2);
          rxm.rtr = CANOPEN_SERIAL.read();
          rxm.len = revLength - 3;
          CANOPEN_SERIAL.readBytes(rxm.data, rxm.len);

          canDispatch(co_data, &rxm);
          canRxState = CAN_IDLE;
          break;
      }
    }
  }

  #ifdef __cplusplus
}  // extern "C"
  #endif  // __cplusplus

#endif  // DRIVER_USE_SERIAL
