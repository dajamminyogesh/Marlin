/**
 *  printer_event_leds.cpp
 *
 *  Created on: 2024/04/23
 *      Author: CreatBot LYN
 *
 *  LED color changing based on printer work status
 */

#include "../../inc/MarlinConfigPre.h"

#if ENABLED(PRINTER_STATUS_LEDS)

  #include "../../MarlinCore.h"
  #include "../../module/temperature.h"
  #include "../../sd/cardreader.h"
  #include "../runout.h"
  #include "leds.h"
  #include "printer_status_leds.h"

PrinterStatusLEDs printerStatusLEDs;

PrinterStatusLEDs::PrinterStatus_t PrinterStatusLEDs::state = PrinterStatus_t::OFF;

static bool isPrinterHeating() {
  HOTEND_LOOP() if (thermalManager.degTargetHotend(e) > 0) return true;
  TERN_(HAS_HEATED_BED, if (thermalManager.degTargetBed() > 0) return true);
  TERN_(HAS_TEMP_CHAMBER, if (thermalManager.degTargetChamber() > 0) return true);
  return false;
}

void PrinterStatusLEDs::update(bool error) {
  PrinterStatus_t newState = state;

  if (error) {                                            // 异常报警
    newState = PrinterStatus_t::ERR;                      //
  } else if (printingIsPaused()) {                        // 正在暂停或耗材异常(耗材异常一定暂停?)
    newState = PrinterStatus_t::WARN;                     //
  } else if (isPrinterHeating() || printingIsActive()) {  // 加热中或正在打印
    newState = PrinterStatus_t::WORK;                     //
  } else {                                                // 空闲中
    newState = PrinterStatus_t::IDLE;
    // TODO 区分 空闲中 与 打印完成
  }

  if (state != newState) {
    switch (newState) {
      case PrinterStatus_t::IDLE: leds.set_color(LEDCOLOR_IDLE); break;
      case PrinterStatus_t::WORK: leds.set_color(LEDCOLOR_WORK); break;
      case PrinterStatus_t::WARN: leds.set_color(LEDCOLOR_WARN); break;
      case PrinterStatus_t::ERR: leds.set_color(LEDCOLOR_ERR); break;
      case PrinterStatus_t::DONE: leds.set_color(LEDCOLOR_DONE); break;
    }
    state = newState;
  }
}

#endif  // PRINTER_STATUS_LEDS
