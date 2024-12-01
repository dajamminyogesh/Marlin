/**
 *  printer_event_leds.cpp
 *
 *  Created on: 2024/06/01
 *      Author: CreatBot WRP
 *
 */

#include "../../../inc/MarlinConfig.h"

#if ENABLED(ANNEALING_SUPPORT)

  #include "../../../feature/annealing.h"
  #include "../../../module/temperature.h"
  #include "../../../MarlinCore.h"
  #include "../../gcode.h"

/**
 * M1010: Enable / Disable annealing support.
 *
 * Parameters
 *   S[bool] - Flag to enable / disable.
 *             If omitted, report current state.
 * M1010 上报当前退火状态（开启/关闭）及参数
 * S1: S[非零值] 开启退火功能
 * S0: S[零值]   关闭退火功能，并清除设置的参数
 * M1010 P0 T180 D120 : 设置退火第一阶段温度180℃，时间120min。
 * */
void GcodeSuite::M1010() {
  if (parser.seen('S')) {
    printDoneAnnealing.enable(parser.value_bool());
    if(!parser.intval('S')) printDoneAnnealing.reset();
  }

  if (!(parser.seen('S') || parser.seen('K') || parser.seen('P'))) M1010_report();

  if (parser.seen('K')) {
    printDoneAnnealing.setdefualt();
    printDoneAnnealing.isneed = true;
  }

  if (parser.seenval('P') && parser.seenval('T') && parser.seenval('D')) {
    printDoneAnnealing.set(parser.intval('P'), parser.intval('T'), parser.intval('D'));
  }

}

void GcodeSuite::M1010_report(const bool forReplay/*=true*/) {
  report_heading_etc(forReplay, F(STR_ANNEALING_SUPPORT));
  SERIAL_ECHOPGM(" M1010 S", AS_DIGIT(printDoneAnnealing.enabled), " : ");
  serialprintln_onoff(printDoneAnnealing.enabled);
  for(uint8_t i; i< ANNEALING_PROCESS_MAX; i++){
    if(printDoneAnnealing.AnnealingSet[i].temperature && printDoneAnnealing.AnnealingSet[i].time) {
      SERIAL_ECHOLNPGM(
      " P", int(i),
      " T", int(printDoneAnnealing.AnnealingSet[i].temperature),
      " D", int(printDoneAnnealing.AnnealingSet[i].time/60000),
      );
    }
  }
}

#endif  // ANNEALING_SUPPORT



