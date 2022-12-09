/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float Temp = 0;
volatile static uint16_t odczyt[2];
float voltage[2];
double res;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LED_line(void)
{
	if(voltage[0] < 1.0f)
	{
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0);
	}
	else if (voltage[0] >= 1.0f && voltage[0] < 2.0f)
	{
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0);
	}
	else if (voltage[0] >= 2.0f && voltage[0] < 3.0f)
	{
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0);
	}
	else if (voltage[0] > 3.0f)
	{
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
	}
}

float calculate_voltage(uint32_t measure)
{
	// wpisać 6.0f zeby sprawdzić diody
	return 3.27f * measure / 4096.0f;
}

float calculate_temp(float sensor_voltage)
{
	return (sensor_voltage - 0.76f) / 0.0025f + 25.0f;
}

float calculate_resistance(float sensor_voltage)
{
	sensor_voltage = 3.3f - sensor_voltage;
	return (5000 / sensor_voltage)*(3.3f - sensor_voltage);
}

// timers callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	 //nie blokuajcy przerwania
//	HAL_ADC_Start_IT(&hadc1);

	// nie blokujacy DMA
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)odczyt, 2);
}

// GPIO callbacks
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

//	// tryb blokujacy
//	HAL_ADC_Start(&hadc1);
//
//	// odczyt z konwerterów
//	hadc1.NbrOfCurrentConversionRank = 0;
//	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	odczyt[0] = HAL_ADC_GetValue(&hadc1);
//	voltage[0] = calculate_voltage(odczyt[0]);
//
//	hadc1.NbrOfCurrentConversionRank = 1;
//	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	odczyt[1] = HAL_ADC_GetValue(&hadc1);
//	voltage[1] = calculate_voltage(odczyt[1]);
//	Temp = calculate_temp(voltage[1]);
//
//	hadc1.NbrOfCurrentConversionRank = 0;

	// tryb nieblokujacy przerwanie

//	HAL_ADC_Start_IT(&hadc1);


	char str_buffer[32];
	sprintf(str_buffer, "Volage: ADC = %u  (%.3f V)\r\n", odczyt[0], voltage[0]);
	send_string(str_buffer);

}

void send_string(char* s)
{
	HAL_UART_Transmit_IT(&huart3, (uint8_t*)s, strlen(s));
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc == &hadc1)
	{
//		if(hadc->NbrOfCurrentConversionRank == 0)
//		{
//			odczyt[0] = HAL_ADC_GetValue(&hadc1);
//			voltage[0] = calculate_voltage(odczyt[0]);
//
//		}
//
//		if(hadc->NbrOfCurrentConversionRank == 1)
//		{
//			odczyt[1] = HAL_ADC_GetValue(&hadc1);
//			voltage[1] = calculate_voltage(odczyt[1]);
//			Temp = calculate_temp(voltage[1]);
//
//		}
//
//		if(++hadc->NbrOfCurrentConversionRank == 2)
//		{
//			hadc->NbrOfCurrentConversionRank = 0;
//			LED_line();
//		}

		// DMA
		for (int i = 0; i < 2; i++ )
		{
			voltage[i] = calculate_voltage(odczyt[i]);
		}
		Temp = calculate_temp(voltage[1]);
		res = calculate_resistance(voltage[0]);
		LED_line();


	}
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART3_UART_Init();
  MX_DMA_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI4_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_TIM_Base_Start_IT(&htim3);
  while (1)
  {

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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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

#ifdef  USE_FULL_ASSERT
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
