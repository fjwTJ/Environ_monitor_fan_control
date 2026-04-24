#include "dht11.h"

#define DHT11_TIMEOUT_US        100U
#define DHT11_START_LOW_MS      20U
#define DHT11_RESPONSE_WAIT_US  40U
#define DHT11_BIT_SAMPLE_US     40U

static uint8_t dwt_ready = 0;

static void DHT11_GPIO_MODE_SET(uint8_t mode);
static void DHT11_DWT_Init(void);
static uint32_t DHT11_GetMicros(void);
static uint8_t DHT11_WaitForLevel(GPIO_PinState level, uint32_t timeout_us);

uint8_t data[5] = {0};

void DHT11(void)
{
	if(!f_400ms) return;
	f_400ms = 0;
	(void)DHT11_READ_DATA();
}

void DHT11_START(void)
{
    DHT11_GPIO_MODE_SET(0);
    DHT11_PIN_RESET;
    HAL_Delay(DHT11_START_LOW_MS);
    DHT11_PIN_SET;
    Coarse_delay_us(30);
    DHT11_GPIO_MODE_SET(1);
}

unsigned char DHT11_READ_BIT(void)
{
    if(!DHT11_WaitForLevel(GPIO_PIN_SET, DHT11_TIMEOUT_US))
    {
        return 0;
    }

    Coarse_delay_us(DHT11_BIT_SAMPLE_US);

    if(DHT11_READ_IO == GPIO_PIN_SET)
    {
        if(!DHT11_WaitForLevel(GPIO_PIN_RESET, DHT11_TIMEOUT_US))
        {
            return 0;
        }
        return 1;
    }

    return 0;
}

unsigned char DHT11_READ_BYTE(void)
{
    uint8_t i, temp = 0;

    for(i = 0; i < 8; i++)
    {
        temp <<= 1;
        if(DHT11_READ_BIT())
        {
            temp |= 1;
        }
    }

    return temp;
}

unsigned char DHT11_READ_DATA(void)
{
    uint8_t i;
    uint8_t sum;

    DHT11_START();

    if(!DHT11_Check())
    {
        return 2;
    }

    if(!DHT11_WaitForLevel(GPIO_PIN_SET, DHT11_TIMEOUT_US))
    {
        return 2;
    }

    if(!DHT11_WaitForLevel(GPIO_PIN_RESET, DHT11_TIMEOUT_US))
    {
        return 2;
    }

    for(i = 0; i < 5; i++)
    {
        data[i] = DHT11_READ_BYTE();
    }

    sum = data[0] + data[1] + data[2] + data[3];
    if(sum == data[4])
    {
        return 1;
    }

    return 0;
}

unsigned char DHT11_Check(void)
{
    Coarse_delay_us(DHT11_RESPONSE_WAIT_US);
    if(DHT11_READ_IO == GPIO_PIN_RESET)
    {
        return 1;
    }

    return 0;
}

static void DHT11_GPIO_MODE_SET(uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_9;

    if(mode)
    {
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    }
    else
    {
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    }

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Coarse_delay_us(uint32_t us)
{
    uint32_t start_us;

    DHT11_DWT_Init();
    start_us = DHT11_GetMicros();

    while((DHT11_GetMicros() - start_us) < us)
    {
    }
}

static void DHT11_DWT_Init(void)
{
    if(dwt_ready)
    {
        return;
    }

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    dwt_ready = 1;
}

static uint32_t DHT11_GetMicros(void)
{
    return DWT->CYCCNT / (HAL_RCC_GetHCLKFreq() / 1000000U);
}

static uint8_t DHT11_WaitForLevel(GPIO_PinState level, uint32_t timeout_us)
{
    uint32_t start_us;

    DHT11_DWT_Init();
    start_us = DHT11_GetMicros();

    while(DHT11_READ_IO != level)
    {
        if((DHT11_GetMicros() - start_us) >= timeout_us)
        {
            return 0;
        }
    }

    return 1;
}
