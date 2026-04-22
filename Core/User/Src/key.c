#include "key.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"

uint8_t Key_Data = 0;

void KeyScan(void)
{
    static uint8_t temState = 0;
    uint8_t temKeyFlg = 0;

    temKeyFlg = GetKey();

    switch (temState)
    {
        case 0:if(temKeyFlg) temState = 1;
            break;
        case 1:if(temKeyFlg) 
                {
                    Key_Data = temKeyFlg;
                    temState = 2;
                }
                else temState = 0;
            break;
        case 2:if(!temKeyFlg) temState = 0;
            break;
        default:
            break;
    }
}

uint8_t GetKey(void)
{
	uint8_t temp = 0;

    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0) temp = 1;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0) temp = 2;
	
	return temp;
}
