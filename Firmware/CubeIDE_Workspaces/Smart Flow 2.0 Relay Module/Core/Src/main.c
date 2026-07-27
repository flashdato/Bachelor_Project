/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include <stdbool.h>
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
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

//Bluetooth
uint8_t rxData;
uint8_t rx_buffer[100] = { 0 };
uint8_t rx_index = 0;

//tap
uint8_t position = 0;
bool decrease;
bool increase;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
	/* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart3, &rxData, 1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0); //1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0); //3
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0); //4
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0); //8
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0); //10
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0); //11
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0); //6
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0); //5
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0); //2
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0); //7
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0); //9
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		if (position != 0 && decrease) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
			decrease=false;
			position--;
		}
		if (position != 90 && increase) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1);
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
			increase=false;
			position++;
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void) {

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 9600;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
					| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8
					| GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
	GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8,
			GPIO_PIN_RESET);

	/*Configure GPIO pins : PA0 PA1 PA2 PA3
	 PA4 PA5 PA6 PA7
	 PA8 PA9 PA12 PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
			| GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8
			| GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB3 PB4 PB5 PB6
	 PB7 PB8 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
			| GPIO_PIN_7 | GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART3) {
		if (rxData != 10) {
			rx_buffer[rx_index++] = rxData;
		} else if (strstr((char*) rx_buffer, "001")) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "101")) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "002")) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "102")) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "003")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "103")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "004")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "104")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "005")) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "105")) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "006")) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "106")) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "007")) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "107")) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "008")) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "108")) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "009")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "109")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "010")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "110")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "011")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "111")) {
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 1);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "M0")) {
			if(position !=0)
			decrease = true;
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "M1")) {
			if(position!=90)
			increase = true;
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else {
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		}

		HAL_UART_Receive_IT(&huart3, &rxData, 1);

	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
