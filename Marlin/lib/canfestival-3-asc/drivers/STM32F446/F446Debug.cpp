#include <canfestival.h>

#include "F446Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

  void debug(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    Serial.vprintf(format, ap);
    va_end(ap);
  }

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus