/**
 *  LinuxScreenHelper.h
 *
 *  Created on: 2023/08/25
 *      Author: CreatBot LYN
 *
 *  helper for linux lcd's canOpen.
 */
#pragma once

#include "../../../../inc/MarlinConfigPre.h"
#include "../../ui_api.h"

// 温度范围:-300.00 ~ 1000.00; 精度:0.02度; ==> uint16 = (float + 300) * 50
#define CANOPEN_CURTEMP(index) (uint16_t)((ExtUI::getActualTemp_celsius(ExtUI::heater_t::index) + 300) * 50)
#define CANOPEN_TARTEMP(index) (uint16_t)((ExtUI::getTargetTemp_celsius(ExtUI::heater_t::index) + 300) * 50)

// 温度范围:-300.00 ~ 1000.00; 精度:0.02度; ==> float = (uint16 / 50.0) - 300
#define CANOPEN_SETTEMP(index, value) ExtUI::setTargetTemp_celsius(value / 50.0 - 300, ExtUI::heater_t::index)

inline static uint8_t percent_to_ui8(uint8_t percent) { return map(constrain(percent, 0, 100), 0, 100, 0, 255); }
