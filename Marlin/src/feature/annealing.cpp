/**
 *  annealing.h
 *
 *  Created on: 2024/06/01
 *      Author: CreatBot WRP
 *
 *  The annealing function for the peek series.
 */

#include "../inc/MarlinConfig.h"

#if ENABLED(ANNEALING_SUPPORT)

  #include "../MarlinCore.h"
  #include "../gcode/gcode.h"
  #include "../module/planner.h"
  #include "../module/temperature.h"
  #include "annealing.h"

constexpr Annealing_Setting_t annealing_preset[] = ANNEALING_PRESETS;

#define PROSET_COUNT COUNT(annealing_preset)
static_assert(WITHIN(PROSET_COUNT, 1, ANNEALING_PROCESS_MAX),
              "ANNEALING_PRESETS requires between 1 and ANNEALING_PROCESS_MAX.");

PrintDoneAnnealing  printDoneAnnealing;
Annealing_Setting_t PrintDoneAnnealing::AnnealingSet[ANNEALING_PROCESS_MAX];
bool                PrintDoneAnnealing::enabled = false;
bool                PrintDoneAnnealing::isneed  = false;
bool                PrintDoneAnnealing::ongoing = false;

static uint8_t  progressCount                   = 0;
static millis_t timeout                         = 0;
/**
 * Enable or disable annealing.
 */
void PrintDoneAnnealing::enable(const bool onoff) { enabled = onoff; }

void PrintDoneAnnealing::set(const uint8_t progress, const celsius_t temp, const millis_t time) {
  if (progress > ANNEALING_PROCESS_MAX) return;
  if (!time) return;
  if (!enabled) enable(true);
  AnnealingSet[progress].temperature = temp;
  AnnealingSet[progress].time        = time * 60 * 1000;
}

void PrintDoneAnnealing::reset() {
  memset(&AnnealingSet, 0, sizeof(AnnealingSet));
  progressCount = 0;
  timeout       = 0;
  enabled       = false;
  isneed        = false;
  ongoing       = false;
  thermalManager.cooldown();
}

void PrintDoneAnnealing::setdefualt() {
  if(!enabled) return;
  
  char cmd[MAX_CMD_SIZE + 16];

  for (u_int8_t i = 0; i < ANNEALING_PROCESS_MAX; i++) {
    if (AnnealingSet[i].temperature && AnnealingSet[i].time) return;
  }

  for (uint8_t i = 0; i < PROSET_COUNT; i++) {
    sprintf_P(cmd, PSTR("M1010 P%i T%i D%i"), i, annealing_preset[i].temperature, annealing_preset[i].time);
    gcode.process_subcommands_now(cmd);
  }
}

void PrintDoneAnnealing::task() {
  if (!(enabled && isneed)) return;

  const millis_t ms   = millis();
  const millis_t time = AnnealingSet[progressCount].time;
  if(ongoing && (!thermalManager.degTargetChamber()))  return reset();
  if (ELAPSED(ms, timeout)) {
    if (time) {
      thermalManager.setTargetChamber(AnnealingSet[progressCount].temperature);
      timeout = millis() + time;
      ongoing = true;
    }
    progressCount++;
    if (progressCount > ANNEALING_PROCESS_MAX) {
      reset();
    }
  }
}

#endif  // ANNEALING_SUPPORT
