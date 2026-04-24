#include "oled_use.h"

static void OLED_ShowLine16(uint8_t y, const char *text)
{
	char line_buffer[17] = {0};
	snprintf(line_buffer, sizeof(line_buffer), "%-16.16s", text);
	OLED_ShowStr(0, y, (unsigned char *)line_buffer, 2);
}

void OLED_ClearScreen(void)
{
	OLED_CLS();
}

void OLED_INITSHOW(void)
{
	OLED_Init();
	OLED_CLS();
	OLED_ON();
	OLED_ShowStr(8, 1, (unsigned char *)"Env Monitor", 2);
	OLED_ShowStr(12, 4, (unsigned char *)"Starting...", 2);
	HAL_Delay(700);
	OLED_CLS();
}

void OLED_ShowMonitorPage(uint8_t temperature, uint8_t humidity, uint16_t dust_density,
						  uint16_t dust_adc_raw, const char *air_text, uint8_t fan_on)
{
	char line_buffer[17] = {0};

	(void)dust_adc_raw;

	snprintf(line_buffer, sizeof(line_buffer), "TEMP:%2u C", temperature);
	OLED_ShowLine16(0, line_buffer);

	snprintf(line_buffer, sizeof(line_buffer), "HUMI:%2u %%", humidity);
	OLED_ShowLine16(2, line_buffer);

	snprintf(line_buffer, sizeof(line_buffer), "DUST:%3uUG/M3", dust_density);
	OLED_ShowLine16(4, line_buffer);

	snprintf(line_buffer, sizeof(line_buffer), "AQ:%-5.5s %s", air_text, fan_on ? "ON" : "OFF");
	OLED_ShowLine16(6, line_buffer);
}

void OLED_ShowTempSettingPage(uint8_t temp_threshold)
{
	char line_buffer[17] = {0};

	OLED_ShowLine16(0, "TEMP LIMIT");
	snprintf(line_buffer, sizeof(line_buffer), "TH:%2u C", temp_threshold);
	OLED_ShowLine16(2, line_buffer);
	OLED_ShowLine16(4, "K1:PAGE");
	OLED_ShowLine16(6, "K2:+1 LOOP");
}

void OLED_ShowDustSettingPage(uint16_t dust_threshold)
{
	char line_buffer[17] = {0};

	OLED_ShowLine16(0, "DUST LIMIT");
	snprintf(line_buffer, sizeof(line_buffer), "TH:%3u UG", dust_threshold);
	OLED_ShowLine16(2, line_buffer);
	OLED_ShowLine16(4, "K1:PAGE");
	OLED_ShowLine16(6, "K2:+10 LOOP");
}
