// src/core/battery.cpp
#include "battery.h"
#include <esp_adc_cal.h>
#include <Arduino.h>

static constexpr uint32_t DEFAULT_VREF = 1100; // mV de référence
static esp_adc_cal_characteristics_t adc_chars;

void battery_init() {
    // Remplit adc_chars avec la caractérisation (utilise DEFAULT_VREF en mV)
    esp_adc_cal_characterize(
      ADC_UNIT_1,
      ADC_ATTEN_DB_11,
      ADC_WIDTH_BIT_12,
      DEFAULT_VREF,
      &adc_chars
    );
}

uint32_t battery_get_voltage_mV() {
    uint32_t raw = analogRead(PIN_BAT_VOLT);
    // Conversion selon les caractéristiques
    return esp_adc_cal_raw_to_voltage(raw, &adc_chars) * 2; // si pont diviseur 1:2
}

uint8_t battery_get_percent() {
    uint32_t mv = battery_get_voltage_mV();
    if (mv <= 2500)        return 0;
    else if (mv >= 4200)   return 100;
    else                   return (uint8_t)((mv - 2500) * 100 / (4200 - 2500));
}