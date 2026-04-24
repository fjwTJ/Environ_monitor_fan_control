#include "interrupt.h"

uint16_t cnt_20ms = 0;
uint8_t f_20ms = 0;
uint16_t cnt_300ms = 0;
uint8_t f_300ms = 0;
uint16_t cnt_400ms = 0;
uint8_t f_400ms = 0;
uint16_t pwm_cnt = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2)
  {
	Fan_LED_CONTROL();
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);
		__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_4,pwm_cnt);
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

void Fan_LED_CONTROL(void)
{
	if(data[2] > 30)
		pwm_cnt = 500;
	else
		pwm_cnt = 0;
}

