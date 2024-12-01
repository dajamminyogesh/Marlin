#include "applicfg.h"
#include "inc/data.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

  void debug(const char *format, ...);
  void* initTimer();
  UNS8 canSend(CAN_PORT notused, Message *m);
  UNS8 canInit(CO_Data *d, uint32_t bitrate);
  void canClose(void);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
