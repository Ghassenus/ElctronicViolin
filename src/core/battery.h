// src/core/battery.h
#pragma once
#include <cstdint>

// broche ADC reliée à la mesure de batterie
static constexpr uint8_t PIN_BAT_VOLT = 34;
void     battery_init();
uint32_t battery_get_voltage_mV();
uint8_t  battery_get_percent();
