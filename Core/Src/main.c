/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define txPacketCount 3
#define rxPacketCount 2
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN PV */
UCAN_HandleTypeDef ucan1;
UCAN_Packet txpackets[txPacketCount];
UCAN_Packet rxpackets[rxPacketCount];

uint32_t clients[3] = { 0x100U, 0x200U, 0x300U };

uint8_t byte1;
uint8_t byte1_2;
uint16_t byte2;
uint32_t byte4;
uint8_t byte1_3;
uint16_t byte2_3;
uint32_t byte4_3;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */
static void UCAN1_Init(void);
static void UCAN1_Config(void);
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
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
	UCAN1_Init();

	UCAN1_Config();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		byte4_3++;
		byte1_3++;
		byte4++;
		HAL_Delay(50);
		uCAN_SendAll(&ucan1);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 21;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void UCAN1_Init(void) {
	ucan1.hcan = &hcan1;
	ucan1.node.masterId = 0x000;
	ucan1.node.selfId = 0x000;
	ucan1.node.role = UCAN_ROLE_MASTER;
	ucan1.node.clientIdList = clients;
	ucan1.node.clientCount = UCAN_CLIENT_COUNT(clients);
	ucan1.txHolder.packets = txpackets;
	ucan1.txHolder.count = UCAN_PACKET_COUNT(txpackets);
	ucan1.rxHolder.packets = rxpackets;
	ucan1.rxHolder.count = UCAN_PACKET_COUNT(rxpackets);

	if (uCAN_Init(&ucan1) != UCAN_OK) {
		Error_Handler();
	}
}

static void UCAN1_Config(void) {

	UCAN_PacketConfig txConfigPacket[txPacketCount] = {
			{ .id = 0x245U, .item_count = 2,
					.items = {
					//{.type = UCAN_U32, .ptr = &byte4 },
					//{.type = UCAN_U16, .ptr = &byte2 },
					{ .type = UCAN_U32, .ptr = &byte4 },
					{ .type = UCAN_U8, .ptr = &byte1_2 },
			}

			},

			{ .id = 0x250U, .item_count = 2,
					.items = {
							{ .type = UCAN_U8, .ptr = &byte1_3 },
							//{.type = UCAN_U16, .ptr = &byte2},
							{ .type = UCAN_U32, .ptr = &byte4_3 },
							//{.type = UCAN_U8,  .ptr = &byte1_2},
			}

			},

			{ .id = 0x240U, .item_count = 3,
					.items = {
							{ .type = UCAN_U32, .ptr = &byte4},
							{ .type = UCAN_U8,  .ptr = &byte1},
							{ .type = UCAN_U8,  .ptr = &byte1_2},
					}
			},
	};

	UCAN_PacketConfig rxConfigPacket[rxPacketCount] = {
			{ .id = 0x360U, .item_count = 3,
					.items = {
							{ .type = UCAN_U8, .ptr = &byte1_2 },
							{ .type = UCAN_U16, .ptr = &byte2 },
							{ .type = UCAN_U32, .ptr = &byte4_3 },
			}

			},

			{ .id = 0x350U, .item_count = 2,
					.items = {
							{ .type = UCAN_U32, .ptr = &byte4 },
							//{.type = UCAN_U16, .ptr = &byte2},
							{ .type = UCAN_U32, .ptr = &byte4_3 },
							//{.type = UCAN_U8,  .ptr = &byte1_2},
					}
			},
	};

	UCAN_Config ucanConfig;

	ucanConfig.txPacketList = txConfigPacket;
	ucanConfig.rxPacketList = rxConfigPacket;

	if (uCAN_Start(&ucan1, &ucanConfig) != UCAN_OK) {
		Error_Handler();
	}
}
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
	while (1) {
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
