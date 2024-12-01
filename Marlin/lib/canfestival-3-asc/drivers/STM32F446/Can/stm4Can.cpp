#include <config.h>

#ifdef DRIVER_USE_CAN

  #include "../../../inc/data.h"
  #include "../../../inc/timer.h"
  #include "canfestival.h"
  #include "stm4Can.h"

// 扩展库函数
// - 允许设置中断优先级
// - 对CAN邮箱或者队列写数据时,禁用可能调用写数据的定时器
// - 发送队列有数据时,强制缓存新发送数据
class STM32_canOpen : public STM32_CAN {
 public:
  STM32_canOpen(CAN_TypeDef *canPort, CAN_PINS pins)
      : _canPort(canPort), _canTimer(NULL), STM32_CAN(canPort, pins, RX_SIZE_16, TX_SIZE_64){};

  void setPriority(uint32_t preemptPriority, uint32_t subPriority) {
    if (_canPort == CAN1) {
      HAL_NVIC_SetPriority(CAN1_RX0_IRQn, preemptPriority, subPriority);
      HAL_NVIC_SetPriority(CAN1_TX_IRQn, preemptPriority, subPriority);
    }

  #ifdef CAN2
    if (_canPort == CAN2) {
      HAL_NVIC_SetPriority(CAN2_RX0_IRQn, preemptPriority, subPriority);
      HAL_NVIC_SetPriority(CAN2_TX_IRQn, preemptPriority, subPriority);
    }
  #endif

  #ifdef CAN3
    if (_canPort == CAN2) {
      HAL_NVIC_SetPriority(CAN3_RX0_IRQn, preemptPriority, subPriority);
      HAL_NVIC_SetPriority(CAN3_TX_IRQn, preemptPriority, subPriority);
    }
  #endif
  }

  void setTimer(TIM_HandleTypeDef *timer) { _canTimer = timer; }

  void disableTimerIT() {
    if (_canTimer) __HAL_TIM_DISABLE_IT(_canTimer, TIM_IT_UPDATE);
  }
  void enableTimerIT() {
    if (_canTimer) __HAL_TIM_ENABLE_IT(_canTimer, TIM_IT_UPDATE);
  }

  bool write(CAN_message_t &CAN_tx_msg) {
    bool success;
    disableTimerIT();
    if (txRing.head == txRing.tail) {  // 发送队列是空的
      success = STM32_CAN::write(CAN_tx_msg);
    } else {
      success = addToRingBuffer(txRing, CAN_tx_msg);
    }
    enableTimerIT();
    return success;
  }

 private:
  CAN_TypeDef       *_canPort;
  TIM_HandleTypeDef *_canTimer;
};

static STM32_canOpen stm32_can(CANx, CANp);
static TIMER_HANDLE  runTimer = TIMER_NONE;

static void canLoop(CO_Data *d, UNS32 id);

void canClose(void) { DelAlarm(runTimer); }

// CAN协议的中断优先级要高于canOpen定时器的优先级
// 避免定时器打断CAN中断回调, 插入新的发送数据, 导致数据发送乱序
UNS8 canInit(CO_Data *d, uint32_t bitrate) {
  stm32_can.begin(true);
  stm32_can.setPriority(13, 0);
  stm32_can.setBaudRate(bitrate);

  HardwareTimer *timer = (HardwareTimer *)initTimer();
  stm32_can.setTimer(timer->getHandle());

  d->canHandle = &stm32_can;

  DelAlarm(runTimer);
  runTimer = SetAlarm(d, 0, &canLoop, MS_TO_TIMEVAL(500), US_TO_TIMEVAL(50));

  return 0;
}

UNS8 canSend(CAN_PORT canPort, Message *m) {
  CAN_message_t msg;

  msg.id             = m->cob_id;
  msg.flags.extended = 0;
  msg.flags.remote   = m->rtr;
  msg.len            = m->len;
  memcpy(msg.buf, m->data, 8);

  STM32_canOpen *canHandle = (STM32_canOpen *)canPort;
  return !canHandle->write(msg);
}

static void canLoop(CO_Data *d, UNS32 id) {
  CAN_message_t msg;
  Message       m;

  STM32_canOpen *canHandle = (STM32_canOpen *)d->canHandle;
  while (canHandle->read(msg)) {
    if (msg.flags.extended == 1) continue;

    m.cob_id = msg.id;
    m.rtr    = msg.flags.remote;
    m.len    = msg.len;
    memcpy(m.data, msg.buf, 8);

    canDispatch(d, &m);
  }
}

#endif  // DRIVER_USE_CAN
