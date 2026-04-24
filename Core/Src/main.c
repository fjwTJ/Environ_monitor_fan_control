/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "all.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FAN_PWM_RUN_DUTY         500U
#define TEMP_THRESHOLD_DEFAULT   25U
#define TEMP_THRESHOLD_MIN       20U
#define TEMP_THRESHOLD_MAX       50U
#define DUST_THRESHOLD_DEFAULT   100U
#define DUST_THRESHOLD_MIN       50U
#define DUST_THRESHOLD_MAX       300U
#define DUST_THRESHOLD_STEP      10U
#define PM25_SAMPLE_COUNT        5U
#define PM25_VOLTAGE_SCALE       2.0f
#define PM25_MAX_DENSITY         500U
#define PM25_LED_ACTIVE_LEVEL    GPIO_PIN_RESET

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
typedef enum
{
  PAGE_MONITOR = 0,
  PAGE_TEMP_THRESHOLD,
  PAGE_DUST_THRESHOLD,
  PAGE_COUNT
} AppPage_t;

typedef enum
{
  AIR_EXCELLENT = 0,
  AIR_GOOD,
  AIR_LIGHT,
  AIR_MODERATE,
  AIR_HEAVY
} AirQuality_t;

static AppPage_t current_page = PAGE_MONITOR;
static uint8_t temperature_threshold = TEMP_THRESHOLD_DEFAULT;
static uint16_t dust_threshold = DUST_THRESHOLD_DEFAULT;
static uint16_t dust_density = 0;
static uint16_t dust_adc_raw = 0;
static uint8_t fan_enabled = 0;
static AirQuality_t air_quality = AIR_EXCELLENT;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void APP_Init(void);
static void APP_ProcessSensors(void);
static void APP_ProcessKeys(void);
static void APP_UpdateOutputs(void);
static void APP_UpdateDisplay(void);
static uint16_t PM25_ReadADC(void);
static uint16_t PM25_ReadDensity(uint16_t *adc_raw);
static uint16_t PM25_ReadAverageDensity(uint8_t sample_count);
static const char *APP_GetAirQualityText(AirQuality_t quality);
static AirQuality_t APP_EvaluateAirQuality(uint16_t dust);
static void APP_SetStatusLed(uint8_t alarm_on);
static void PM25_SetLed(GPIO_PinState state);
static void PM25_LedOn(void);
static void PM25_LedOff(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void APP_Init(void)
{
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
  PM25_LedOff();
  OLED_INITSHOW();
  APP_SetStatusLed(0);
}

static void APP_ProcessSensors(void)
{
  DHT11();

  if(f_300ms)
  {
    f_300ms = 0;
    dust_density = PM25_ReadAverageDensity(PM25_SAMPLE_COUNT);
    air_quality = APP_EvaluateAirQuality(dust_density);
  }
}

static void APP_ProcessKeys(void)
{
  uint8_t key_value = 0;

  if(Key_Data == 0)
  {
    return;
  }

  __disable_irq();
  key_value = Key_Data;
  Key_Data = 0;
  __enable_irq();

  if(key_value == 1U)
  {
    current_page = (AppPage_t)((current_page + 1U) % PAGE_COUNT);
  }
  else if(key_value == 2U)
  {
    if(current_page == PAGE_TEMP_THRESHOLD)
    {
      temperature_threshold++;
      if(temperature_threshold > TEMP_THRESHOLD_MAX)
      {
        temperature_threshold = TEMP_THRESHOLD_MIN;
      }
    }
    else if(current_page == PAGE_DUST_THRESHOLD)
    {
      dust_threshold += DUST_THRESHOLD_STEP;
      if(dust_threshold > DUST_THRESHOLD_MAX)
      {
        dust_threshold = DUST_THRESHOLD_MIN;
      }
    }
  }
}

static void APP_UpdateOutputs(void)
{
  uint8_t over_temp = (data[2] >= temperature_threshold) ? 1U : 0U;
  uint8_t over_dust = (dust_density >= dust_threshold) ? 1U : 0U;

  fan_enabled = (over_temp || over_dust) ? 1U : 0U;
  fan_pwm_target = fan_enabled ? FAN_PWM_RUN_DUTY : 0U;
  APP_SetStatusLed(fan_enabled);
}

static void APP_UpdateDisplay(void)
{
  static uint32_t last_refresh_tick = 0;
  static AppPage_t last_page = PAGE_COUNT;
  uint32_t current_tick = HAL_GetTick();

  if(current_page != last_page)
  {
    OLED_ClearScreen();
    last_page = current_page;
  }

  if((current_tick - last_refresh_tick) < 200U)
  {
    return;
  }

  last_refresh_tick = current_tick;

  if(current_page == PAGE_MONITOR)
  {
    OLED_ShowMonitorPage(data[2], data[0], dust_density, dust_adc_raw,
                         APP_GetAirQualityText(air_quality), fan_enabled);
  }
  else if(current_page == PAGE_TEMP_THRESHOLD)
  {
    OLED_ShowTempSettingPage(temperature_threshold);
  }
  else
  {
    OLED_ShowDustSettingPage(dust_threshold);
  }
}

static uint16_t PM25_ReadADC(void)
{
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 10);
  {
    uint16_t adc_value = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return adc_value;
  }
}

static uint16_t PM25_ReadDensity(uint16_t *adc_raw)
{
  uint16_t adc_value = 0;
  float voltage = 0.0f;
  float density = 0.0f;

  PM25_LedOn();
  Coarse_delay_us(280U);
  adc_value = PM25_ReadADC();
  if(adc_raw != NULL)
  {
    *adc_raw = adc_value;
  }
  Coarse_delay_us(19U);
  PM25_LedOff();
  Coarse_delay_us(9680U);

  voltage = (3.3f * (float)adc_value / 4095.0f) * PM25_VOLTAGE_SCALE;
  density = (0.17f * voltage - 0.1f) * 1000.0f;

  if(density < 0.0f)
  {
    density = 0.0f;
  }
  else if(density > (float)PM25_MAX_DENSITY)
  {
    density = (float)PM25_MAX_DENSITY;
  }

  return (uint16_t)(density + 0.5f);
}

static uint16_t PM25_ReadAverageDensity(uint8_t sample_count)
{
  uint32_t sum = 0;
  uint32_t adc_sum = 0;
  uint8_t i = 0;
  uint16_t adc_value = 0;

  if(sample_count == 0U)
  {
    return 0;
  }

  for(i = 0; i < sample_count; i++)
  {
    sum += PM25_ReadDensity(&adc_value);
    adc_sum += adc_value;
  }

  dust_adc_raw = (uint16_t)(adc_sum / sample_count);
  return (uint16_t)(sum / sample_count);
}

static const char *APP_GetAirQualityText(AirQuality_t quality)
{
  switch(quality)
  {
    case AIR_EXCELLENT:
      return "EXCEL";
    case AIR_GOOD:
      return "GOOD";
    case AIR_LIGHT:
      return "LIGHT";
    case AIR_MODERATE:
      return "MID";
    case AIR_HEAVY:
    default:
      return "HEAVY";
  }
}

static AirQuality_t APP_EvaluateAirQuality(uint16_t dust)
{
  if(dust <= 35U)
  {
    return AIR_EXCELLENT;
  }

  if(dust <= 75U)
  {
    return AIR_GOOD;
  }

  if(dust <= 115U)
  {
    return AIR_LIGHT;
  }

  if(dust <= 150U)
  {
    return AIR_MODERATE;
  }

  return AIR_HEAVY;
}

static void APP_SetStatusLed(uint8_t alarm_on)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, alarm_on ? GPIO_PIN_RESET : GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, alarm_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void PM25_SetLed(GPIO_PinState state)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, state);
}

static void PM25_LedOn(void)
{
  PM25_SetLed(PM25_LED_ACTIVE_LEVEL);
}

static void PM25_LedOff(void)
{
  PM25_SetLed(PM25_LED_ACTIVE_LEVEL == GPIO_PIN_SET ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  APP_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    APP_ProcessSensors();
    APP_ProcessKeys();
    APP_UpdateOutputs();
    APP_UpdateDisplay();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
