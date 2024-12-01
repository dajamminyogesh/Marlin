/**
 *  printer_event_leds.h
 *
 *  Created on: 2024/06/01
 *      Author: CreatBot WRP
 *
 *  The annealing function for the peek series.
 */

#pragma once

#include "../inc/MarlinConfigPre.h"

typedef struct {
  celsius_t temperature;
  millis_t  time;
} Annealing_Setting_t;

class PrintDoneAnnealing {
 public:
  static bool                enabled;
  static bool                isneed;
  static Annealing_Setting_t AnnealingSet[ANNEALING_PROCESS_MAX];

  static void enable(const bool onoff);
  static void reset();
  static void set(const uint8_t progress, const celsius_t temp, const millis_t time);
  static void setdefualt();
  static void task();

 private:
  static bool ongoing;
};

extern PrintDoneAnnealing printDoneAnnealing;
