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
#include <math.h>
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
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t rxData;

//Bluetooth
uint8_t rxData;
uint8_t rx_buffer[100] = { 0 };
uint8_t rx_index = 0;
int len;
char buffer[100];

//sensors
uint16_t readValue1;
uint16_t readValue2;
ADC_ChannelConfTypeDef sConfigPrivate = { 0 };

//TDS
int analogBuffer[30];
int analogBufferIndex = 0;
float tdsValue = 0;
char TDSstring[5];
float tdsValue_User = 0;
int tdsIntervalM = 0;
int tdsInt = 0;

//Preasure
float preasure = 0;
char preasureString[5];

//PH
float ph = 0;
char phString[5];
float calibration_value = 27.34;
int phval = 0;
unsigned long int avgval;
int ph_buffer_arr[10], temp;
int phBufferIndex = 0;

//Time
int second, minute, hour;
int interval, interval1, interval2, interval3, interval4, intervalT;

//Sequence
int sequence = 0;

char intString[5];
int integerValue;

//RELAYS
bool relay1;
bool relay2;
bool relay3;
bool relay4;
bool relay5;
bool relay6;
bool relay7;
bool relay8;
bool relay9;
bool relay10;
bool relay11;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
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
	float calculateAverageVoltage() {
		long sum = 0;
		for (int i = 0; i < 30; i++) {
			sum += analogBuffer[i];
		}
		return (float) sum / 30 / 1200.0;
	}

	float calculateCompensationCoefficient() {
		float temperature = 19;  // Default temperature (adjust as needed)
		return 1.0 + 0.02 * (temperature - 25.0);
	}

	float calculateTDS(float voltage, float coefficient) {
		float tds = (170.42 * voltage * voltage * voltage
				- 253.86 * voltage * voltage + 850.39 * voltage) * 0.5
				/ coefficient;
		return tds / 500; // Adjust this factor as needed
	}
	void floatToParts(float value, char *TDSstring) {
		int integerPart = (int) value;
		int decimalPart = (int) (value * pow(10, 4)) % (int) pow(10, 4);

		// Convert the integer part to a string
		char intString[10];
		sprintf(intString, "%d", integerPart);

		// Convert the decimal part to a string
		char decString[10];
		sprintf(decString, "%0*d", 4, decimalPart);

		// Combine the integer and decimal parts into TDSstring
		strcpy(TDSstring, intString);
		strcat(TDSstring, decString);
	}
	float readPhSensor() {
//		for (int i = 0; i < 9; i++) {
//			for (int j = i + 1; j < 10; j++) {
//				if (buffer_arr[i] > buffer_arr[j]) {
//					temp = buffer_arr[i];
//					buffer_arr[i] = buffer_arr[j];
//					buffer_arr[j] = temp;
//				}
//			}
//		}

		avgval = 0;
		for (int i = 2; i < 8; i++)
			avgval += ph_buffer_arr[i];

		return -5.70 * avgval * 5.0 / 4096 / 6 + calibration_value;

	}
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
	MX_USART1_UART_Init();
	MX_USART3_UART_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_UART_Receive_IT(&huart1, &rxData, 1);
	HAL_UART_Receive_IT(&huart3, &rxData, 1);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		sConfigPrivate.Rank = ADC_REGULAR_RANK_1;
		sConfigPrivate.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
		sConfigPrivate.Channel = ADC_CHANNEL_0;
		HAL_ADC_ConfigChannel(&hadc1, &sConfigPrivate);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		readValue1 = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
		sConfigPrivate.Channel = ADC_CHANNEL_1;
		HAL_ADC_ConfigChannel(&hadc1, &sConfigPrivate);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		readValue2 = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		preasure = (((readValue2 * 5) / 1024) - 0.483) * 250;

		ph_buffer_arr[phBufferIndex] = readValue2;
		phBufferIndex = (phBufferIndex + 1) % 10;

		//ph = readPhSensor(); //ar gvinda memgoni aq gadaqceva ha ras ityv?
		ph = 7.0;

		analogBuffer[analogBufferIndex] = readValue1;
		analogBufferIndex = (analogBufferIndex + 1) % 30;

		tdsValue = calculateTDS(calculateAverageVoltage(),
				calculateCompensationCoefficient());
		floatToParts(tdsValue, TDSstring);
		//floatToParts(preasure, preasureString);
		floatToParts(ph, phString);
		if (tdsValue_User > 1) {
			tdsValue_User = -1;
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
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 7200;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 10000;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

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
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		if (interval > 0) {
			interval--;
		}
		second++;
		if (second == 60) {
			second = 0;
			minute++;
		}
		if (minute == 60) {
			minute = 0;
			hour++;
		}
		if (hour == 24) {
			hour = 0;
		}

		if (intervalT > 0) {
			intervalT--;
		}

		if (sequence == 3 || sequence == 6 || sequence == 9 || sequence == 12) {
			if (tdsValue > 1.3 && intervalT == 0) {
				sprintf(buffer, "M0 \r\n");
				len = strlen(buffer);
				HAL_UART_Transmit(&huart1, buffer, len, 100);
				intervalT = 2;
			} else if (tdsValue < 1 && intervalT == 0) {
				sprintf(buffer, "M1 \r\n");
				len = strlen(buffer);
				HAL_UART_Transmit(&huart1, buffer, len, 100);
				intervalT = 2;
			} else if (tdsValue < 1.3 && tdsValue > 1) {
				intervalT = 2;
			}
		}

		if (sequence == 1) {
			sprintf(buffer, "101 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sprintf(buffer, "101 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			interval = 5;
			if (relay3) {
				sprintf(buffer, "103 \r\n"); // Format the string for relay3 == true
			} else if (relay4) {
				sprintf(buffer, "104 \r\n"); // Format the string for relay4 == true
			} else if (relay5) {
				sprintf(buffer, "105 \r\n"); // Format the string for relay5 == true
			} else if (relay6) {
				sprintf(buffer, "106 \r\n"); // Format the string for relay6 == true
			} else if (relay7) {
				sprintf(buffer, "107 \r\n"); // Format the string for relay7 == true
			} else if (relay8) {
				sprintf(buffer, "108 \r\n"); // Format the string for relay8 == true
			} else if (relay9) {
				sprintf(buffer, "109 \r\n"); // Format the string for relay9 == true
			} else if (relay10) {
				sprintf(buffer, "110 \r\n"); // Format the string for relay10 == true
			} else if (relay11) {
				sprintf(buffer, "111 \r\n"); // Format the string for relay11 == true
			}
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 2;
		}
		if (sequence == 2 && interval == 0) {
			interval = interval1;
			sprintf(buffer, "102 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 3;
		}
		if (sequence == 3 && interval == 0) {
			interval = 5;
			sprintf(buffer, "002 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			len = strlen(buffer);
			sequence = 4;
		}
		if (sequence == 4 && interval == 0) {
			interval = 5;
			if (relay3) {
				sprintf(buffer, "003 \r\n"); // Format the string for relay3 == true
				relay3 = false;
			} else if (relay4) {
				sprintf(buffer, "004 \r\n"); // Format the string for relay4 == true
				relay4 = false;
			} else if (relay5) {
				sprintf(buffer, "005 \r\n"); // Format the string for relay5 == true
				relay5 = false;
			} else if (relay6) {
				sprintf(buffer, "006 \r\n"); // Format the string for relay6 == true
				relay6 = false;
			} else if (relay7) {
				sprintf(buffer, "007 \r\n"); // Format the string for relay7 == true
				relay7 = false;
			} else if (relay8) {
				sprintf(buffer, "008 \r\n"); // Format the string for relay8 == true
				relay8 = false;
			} else if (relay9) {
				sprintf(buffer, "009 \r\n"); // Format the string for relay9 == true
				relay9 = false;
			} else if (relay10) {
				sprintf(buffer, "010 \r\n"); // Format the string for relay10 == true
				relay10 = false;
			} else if (relay11) {
				sprintf(buffer, "011 \r\n"); // Format the string for relay11 == true
				relay11 = false;
			}
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			if (relay3) {
				sprintf(buffer, "103 \r\n"); // Format the string for relay3 == true
			} else if (relay4) {
				sprintf(buffer, "104 \r\n"); // Format the string for relay4 == true
			} else if (relay5) {
				sprintf(buffer, "105 \r\n"); // Format the string for relay5 == true
			} else if (relay6) {
				sprintf(buffer, "106 \r\n"); // Format the string for relay6 == true
			} else if (relay7) {
				sprintf(buffer, "107 \r\n"); // Format the string for relay7 == true
			} else if (relay8) {
				sprintf(buffer, "108 \r\n"); // Format the string for relay8 == true
			} else if (relay9) {
				sprintf(buffer, "109 \r\n"); // Format the string for relay9 == true
			} else if (relay10) {
				sprintf(buffer, "110 \r\n"); // Format the string for relay10 == true
			} else if (relay11) {
				sprintf(buffer, "111 \r\n"); // Format the string for relay11 == true
			}
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 5;
		}
		if (sequence == 5 && interval == 0) {
			interval = interval2;
			sprintf(buffer, "102 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 6;
		}
		if (sequence == 6 && interval == 0) {
			interval = 5;
			sprintf(buffer, "002 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 7;
		}
		if (sequence == 7 && interval == 0) {
			interval = 5;
			if (relay3) {
				sprintf(buffer, "003 \r\n"); // Format the string for relay3 == true
				relay3 = false;
			} else if (relay4) {
				sprintf(buffer, "004 \r\n"); // Format the string for relay4 == true
				relay4 = false;
			} else if (relay5) {
				sprintf(buffer, "005 \r\n"); // Format the string for relay5 == true
				relay5 = false;
			} else if (relay6) {
				sprintf(buffer, "006 \r\n"); // Format the string for relay6 == true
				relay6 = false;
			} else if (relay7) {
				sprintf(buffer, "007 \r\n"); // Format the string for relay7 == true
				relay7 = false;
			} else if (relay8) {
				sprintf(buffer, "008 \r\n"); // Format the string for relay8 == true
				relay8 = false;
			} else if (relay9) {
				sprintf(buffer, "009 \r\n"); // Format the string for relay9 == true
				relay9 = false;
			} else if (relay10) {
				sprintf(buffer, "010 \r\n"); // Format the string for relay10 == true
				relay10 = false;
			} else if (relay11) {
				sprintf(buffer, "011 \r\n"); // Format the string for relay11 == true
				relay11 = false;
			}
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			if (relay3) {
				sprintf(buffer, "103 \r\n"); // Format the string for relay3 == true
			} else if (relay4) {
				sprintf(buffer, "104 \r\n"); // Format the string for relay4 == true
			} else if (relay5) {
				sprintf(buffer, "105 \r\n"); // Format the string for relay5 == true
			} else if (relay6) {
				sprintf(buffer, "106 \r\n"); // Format the string for relay6 == true
			} else if (relay7) {
				sprintf(buffer, "107 \r\n"); // Format the string for relay7 == true
			} else if (relay8) {
				sprintf(buffer, "108 \r\n"); // Format the string for relay8 == true
			} else if (relay9) {
				sprintf(buffer, "109 \r\n"); // Format the string for relay9 == true
			} else if (relay10) {
				sprintf(buffer, "110 \r\n"); // Format the string for relay10 == true
			} else if (relay11) {
				sprintf(buffer, "111 \r\n"); // Format the string for relay11 == true
			}
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 8;
		}
		if (sequence == 8 && interval == 0) {
			interval = interval3;
			sprintf(buffer, "102 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 9;
		}
		if (sequence == 9 && interval == 0) {
			interval = 5;
			sprintf(buffer, "002 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 10;
		}
		if (sequence == 10 && interval == 0) {
			interval = 5;
			if (relay3) {
				sprintf(buffer, "003 \r\n"); // Format the string for relay3 == true
				relay3 = false;
			} else if (relay4) {
				sprintf(buffer, "004 \r\n"); // Format the string for relay4 == true
				relay4 = false;
			} else if (relay5) {
				sprintf(buffer, "005 \r\n"); // Format the string for relay5 == true
				relay5 = false;
			} else if (relay6) {
				sprintf(buffer, "006 \r\n"); // Format the string for relay6 == true
				relay6 = false;
			} else if (relay7) {
				sprintf(buffer, "007 \r\n"); // Format the string for relay7 == true
				relay7 = false;
			} else if (relay8) {
				sprintf(buffer, "008 \r\n"); // Format the string for relay8 == true
				relay8 = false;
			} else if (relay9) {
				sprintf(buffer, "009 \r\n"); // Format the string for relay9 == true
				relay9 = false;
			} else if (relay10) {
				sprintf(buffer, "010 \r\n"); // Format the string for relay10 == true
				relay10 = false;
			} else if (relay11) {
				sprintf(buffer, "011 \r\n"); // Format the string for relay11 == true
				relay11 = false;
			}
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			if (relay3) {
				sprintf(buffer, "103 \r\n"); // Format the string for relay3 == true
			} else if (relay4) {
				sprintf(buffer, "104 \r\n"); // Format the string for relay4 == true
			} else if (relay5) {
				sprintf(buffer, "105 \r\n"); // Format the string for relay5 == true
			} else if (relay6) {
				sprintf(buffer, "106 \r\n"); // Format the string for relay6 == true
			} else if (relay7) {
				sprintf(buffer, "107 \r\n"); // Format the string for relay7 == true
			} else if (relay8) {
				sprintf(buffer, "108 \r\n"); // Format the string for relay8 == true
			} else if (relay9) {
				sprintf(buffer, "109 \r\n"); // Format the string for relay9 == true
			} else if (relay10) {
				sprintf(buffer, "110 \r\n"); // Format the string for relay10 == true
			} else if (relay11) {
				sprintf(buffer, "111 \r\n"); // Format the string for relay11 == true
			}
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 11;
		}
		if (sequence == 11 && interval == 0) {
			interval = interval4;
			sprintf(buffer, "102 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 12;
		}
		if (sequence == 12 && interval == 0) {
			interval = 5;
			sprintf(buffer, "002 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 13;
		}
		if (sequence == 13 && interval == 0) {
			if (relay3) {
				sprintf(buffer, "003 \r\n"); // Format the string for relay3 == true
				relay3 = false;
			} else if (relay4) {
				sprintf(buffer, "004 \r\n"); // Format the string for relay4 == true
				relay4 = false;
			} else if (relay5) {
				sprintf(buffer, "005 \r\n"); // Format the string for relay5 == true
				relay5 = false;
			} else if (relay6) {
				sprintf(buffer, "006 \r\n"); // Format the string for relay6 == true
				relay6 = false;
			} else if (relay7) {
				sprintf(buffer, "007 \r\n"); // Format the string for relay7 == true
				relay7 = false;
			} else if (relay8) {
				sprintf(buffer, "008 \r\n"); // Format the string for relay8 == true
				relay8 = false;
			} else if (relay9) {
				sprintf(buffer, "009 \r\n"); // Format the string for relay9 == true
				relay9 = false;
			} else if (relay10) {
				sprintf(buffer, "010 \r\n"); // Format the string for relay10 == true
				relay10 = false;
			} else if (relay11) {
				sprintf(buffer, "011 \r\n"); // Format the string for relay11 == true
				relay11 = false;
			}
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sprintf(buffer, "001 \r\n");
			len = strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 100);
			sequence = 0;
		}
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART3) {
		if (rxData != 10) {
			rx_buffer[rx_index++] = rxData;
//		} else if (strstr((char*) rx_buffer, "001")) {
//			sprintf(buffer, "001 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "101")) {
//			sprintf(buffer, "101 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "002")) {
//			sprintf(buffer, "002 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "102")) {
//			sprintf(buffer, "102 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "003")) {
//			sprintf(buffer, "003 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "103")) {
//			sprintf(buffer, "103 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "004")) {
//			sprintf(buffer, "004 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "104")) {
//			sprintf(buffer, "104 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "005")) {
//			sprintf(buffer, "005 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "105")) {
//			sprintf(buffer, "105 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "006")) {
//			sprintf(buffer, "006 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "106")) {
//			sprintf(buffer, "106 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "007")) {
//			sprintf(buffer, "007 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "107")) {
//			sprintf(buffer, "107 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "008")) {
//			sprintf(buffer, "008 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "108")) {
//			sprintf(buffer, "108 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "009")) {
//			sprintf(buffer, "009 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "109")) {
//			sprintf(buffer, "109 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "010")) {
//			sprintf(buffer, "010 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "110")) {
//			sprintf(buffer, "110 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "011")) {
//			sprintf(buffer, "011 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
//		} else if (strstr((char*) rx_buffer, "111")) {
//			sprintf(buffer, "111 \r\n");
//			len = strlen(buffer);
//			HAL_UART_Transmit(&huart1, buffer, len, 100);
//			rx_index = 0;
//			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "999")) {

			sprintf(buffer, "%s%s\n", TDSstring, phString);
			len = strlen(buffer);
			HAL_UART_Transmit(&huart3, buffer, len, 100);
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (strstr((char*) rx_buffer, "801")) {
			sequence = 1;
			rx_index = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (rx_buffer[0] == 'S') {
			strncpy(intString, &rx_buffer[2], 4); // Copy the characters to intString
			intString[4] = '\0'; // Null-terminate the string
			integerValue = atoi(intString);
			if (rx_buffer[1] == '1') {
				interval1 = integerValue;
			} else if (rx_buffer[1] == '2') {
				interval2 = integerValue;
			} else if (rx_buffer[1] == '3') {
				interval3 = integerValue;
			} else if (rx_buffer[1] == '4') {
				interval4 = integerValue;
			} else if (rx_buffer[1] == '5') {
				interval1 = integerValue;
			}
			rx_index = 0;
			memset(intString, 0, sizeof(intString));
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (rx_buffer[0] == 'E') {
			strncpy(intString, &rx_buffer[1], 4); // Copy the characters to intString
			intString[4] = '\0'; // Null-terminate the string
			integerValue = atoi(intString);
			tdsValue_User = integerValue / 1000;
			rx_index = 0;
			memset(intString, 0, sizeof(intString));
			memset(rx_buffer, 0, sizeof(rx_buffer));
		} else if (rx_buffer[0] == 'R') {
			strncpy(intString, &rx_buffer[1], 2); // Copy the characters to intString
			intString[2] = '\0'; // Null-terminate the string
			integerValue = atoi(intString);
			relay3 = (integerValue == 3) ? true : relay3;
			relay4 = (integerValue == 4) ? true : relay4;
			relay5 = (integerValue == 5) ? true : relay5;
			relay6 = (integerValue == 6) ? true : relay6;
			relay7 = (integerValue == 7) ? true : relay7;
			relay8 = (integerValue == 8) ? true : relay8;
			relay9 = (integerValue == 9) ? true : relay9;
			relay10 = (integerValue == 10) ? true : relay10;
			relay11 = (integerValue == 11) ? true : relay11;
			rx_index = 0;
			memset(intString, 0, sizeof(intString));
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
