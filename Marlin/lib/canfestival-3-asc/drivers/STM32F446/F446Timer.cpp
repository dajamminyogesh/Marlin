#include <canfestival.h>

#include "F446Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

  static HardwareTimer canTimer(TIM13);
  static TIMEVAL       elapsed_time = 0;
  static bool          reset_time   = true;

  void canTimerIRQ() {
    reset_time = true;
    TimeDispatch();
  }

  void* initTimer() {
    uint32_t prescaler = canTimer.getTimerClkFreq() / 1000000;  // 1us
    canTimer.setPrescaleFactor(prescaler);
    canTimer.setPreloadEnable(false);
    canTimer.setInterruptPriority(15, 0);  // 设置canOpen定时器的中断优先级为最低
    canTimer.attachInterrupt(canTimerIRQ);
    canTimer.resume();
    return &canTimer;
  }

  void setTimer(TIMEVAL value) {
    if (reset_time) {
      reset_time   = false;
      elapsed_time = canTimer.getCount();
    } else {
      elapsed_time += canTimer.getCount();
    }
    canTimer.setCount(0);
    canTimer.setOverflow(value + 1);
  }

  TIMEVAL getElapsedTime(void) {
    TIMEVAL curCounter = canTimer.getCount();
    return (elapsed_time + curCounter);
  }

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
