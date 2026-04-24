#ifndef __OLED_USE_H
#define __OLED_USE_H

#include "all.h"

void OLED_INITSHOW(void);
void OLED_ClearScreen(void);
void OLED_ShowMonitorPage(uint8_t temperature, uint8_t humidity, uint16_t dust_density,
                          uint16_t dust_adc_raw, const char *air_text, uint8_t fan_on);
void OLED_ShowTempSettingPage(uint8_t temp_threshold);
void OLED_ShowDustSettingPage(uint16_t dust_threshold);

#endif
