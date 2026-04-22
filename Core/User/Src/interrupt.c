#include "interrupt.h"

uint16_t cnt_20ms = 0;
uint8_t f_20ms = 0;
uint16_t cnt_300ms = 0;
uint8_t f_300ms = 0;
uint16_t cnt_400ms = 0;
uint8_t f_400ms = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2)
  {		
		if(++cnt_400ms == 400)
		{
			cnt_400ms = 0;
			f_400ms = 1;
		}
		if(++cnt_300ms == 300)
		{
			cnt_300ms = 0;
			f_300ms = 1;
		}
		if(++cnt_20ms == 20)
		{
			KeyScan();
			cnt_20ms = 0;
			f_20ms = 1;
		}
  }
}



