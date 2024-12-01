#ifndef F446ARDUINO_H
#define F446ARDUINO_H

#ifdef ARDUINO
  #if (ARDUINO >= 100)
    #include <Arduino.h>
  #else
    #include <WProgram.h>
    #include <pins_arduino.h>
  #endif
#endif

#endif //F446ARDUINO_H
