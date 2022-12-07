/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
	uint8_t x;
	uint8_t y;
} Coordinates;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BOARD_SIZE 3
// Square size in pixels:
#define SQUARE_SIZE 240/BOARD_SIZE
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
// An array representing a gamefield. '0' means area is clear and '1' denotes mines:
uint8_t gamefield[BOARD_SIZE][BOARD_SIZE] = {{0}};
// Array 'visited' keeps track of visited fields:
uint8_t visited[BOARD_SIZE][BOARD_SIZE] = {{0}};
// 'fields_to_visit' keeps track of the number of fields left to visit:
uint16_t fields_to_visit = BOARD_SIZE*BOARD_SIZE;
// A variable for storing current minesweeper position:
Coordinates players_position;
// A variable for tracking game status. '0' -- game running, '1' -- player 1, '2' -- player 2, '3' -- draw:
uint8_t game_status = 0;
uint8_t active_player = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
int8_t ADC1_Init(void);
uint32_t Get_Seed_Value(void);
void Game_Setup(void);
void Move_Up(void);
void Move_Down(void);
void Move_Left(void);
void Move_Right(void);
void Mark_Field(void);
void Game_Over(void);
void Draw_Square(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Color);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
void Game_Setup(void)
{
	uint16_t i = 0;

	// Clear the screen:
	BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);

	// Draw the grid:
	// Important: have to use '<' instead of '<=' in the first loop below,
	// to avoid drawing an horizontal line at y = 240 (beyond the screen).
	for (i = 0;i < 240;i += SQUARE_SIZE)
	{
		BSP_LCD_DrawHLine(0, i, 240);
	}
	for (i = 0;i <= 240;i += SQUARE_SIZE)
	{
		BSP_LCD_DrawVLine(i, 0, 240);
	}

	// Set minesweeper's initial position:
	players_position.x = 0;
	players_position.y = 0;

	// Mark minesweeper's position:
	Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGRAY);

}

int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t seed = 0;
  JOYState_TypeDef JoyState = JOY_NONE;
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
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize the LEDs */
  // BSP_LED_Init(LED_GREEN);
  // BSP_LED_Init(LED_ORANGE);
  BSP_LED_Init(LED_RED);
  // BSP_LED_Init(LED_BLUE);
  
  /* Configure the Key push-button in GPIO Mode */
  // BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

  /*## Initialize the LCD #################################################*/
  if (BSP_LCD_Init() != LCD_OK)
  {
	  Error_Handler();
  }
  BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_SetFont(&Font12);
  if (BSP_JOY_Init(JOY_MODE_GPIO) != IO_OK)
  {
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 95, (uint8_t *)"ERROR", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 80, (uint8_t *)"Joystick cannot be initialized", CENTER_MODE);
    Error_Handler();
  }
  if (ADC1_Init() != HAL_OK)
  {
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 95, (uint8_t *)"ERROR", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 80, (uint8_t *)"ADC1 cannot be initialized", CENTER_MODE);
    Error_Handler();
  }
  /*## Initialize random number generator #################################*/
  seed = Get_Seed_Value();
  srand(seed);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	Game_Setup();
	while (game_status == 0)
	{
		JoyState = BSP_JOY_GetState();
		switch(JoyState)
		{
		case JOY_UP:
			Move_Up();
			HAL_Delay(200);
			break;
		case JOY_DOWN:
			Move_Down();
			HAL_Delay(200);
			break;
		case JOY_LEFT:
			Move_Left();
			HAL_Delay(200);
			break;
		case JOY_RIGHT:
			Move_Right();
			HAL_Delay(200);
			break;
		case JOY_SEL:
			Mark_Field();
			if (active_player == 1){
				active_player = 2;
			} else active_player = 1;
			HAL_Delay(200);
			break;
		default:
			break;
		}
	}
	Game_Over();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC
                              |RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  // HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);

  /** Configure the Systick interrupt time
  */
 //  __HAL_RCC_PLLI2S_ENABLE();
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/* USER CODE BEGIN 4 */
int8_t ADC1_Init(void)
{
  uint8_t ret = HAL_OK;
  ADC_HandleTypeDef hadc1 = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Common configuration */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  ret = HAL_ADC_Init(&hadc1);
  if (ret != HAL_OK)
  {
    return ret;
  }

  /** Configure regular channel group */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  ret = HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  return ret;
}

uint32_t Get_Seed_Value(void)
{
	uint32_t ret = 0;

	ADC1->CR2 = ADC1->CR2|0x00000001;
	// Wait for the ADC to stabilize:
	HAL_Delay(500);
	// Start conversion:
	ADC1->CR2 |= 0x00000001;
	// Wait for end of the conversion:
	while ((ADC1->SR & 0x00000002) == 0);
	ret = (uint32_t)ADC1->DR;
	// Clear the STRT bit:
	ADC1->SR &=~0x00000010;

	return ret;
}


void Move_Up(void)
{
	// Erase the current mark:
	if (visited[players_position.x][players_position.y] == 1)
	{
		Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_RED);
//		Display_No_Of_Mines(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, mine_numbers[players_position.x][players_position.y], CENTER_MODE);
	}
	else if (visited[players_position.x][players_position.y] == 2)
		{
			Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGREEN);
	//		Display_No_Of_Mines(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, mine_numbers[players_position.x][players_position.y], CENTER_MODE);
		}
	else
	{
		Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_LIGHTGRAY);
	}

	// Move the minesweeper:
	if (players_position.y != 0)
		players_position.y--;
	else
		players_position.y = BOARD_SIZE-1;

	// Mark minesweeper's new position:
	Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGRAY);
}

void Move_Down(void)
{
	// Erase the current mark:
	if (visited[players_position.x][players_position.y] == 1)
	{
		Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_RED);
//		Display_No_Of_Mines(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, mine_numbers[players_position.x][players_position.y], CENTER_MODE);
	}
	else if (visited[players_position.x][players_position.y] == 2)
		{
			Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGREEN);
	//		Display_No_Of_Mines(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, mine_numbers[players_position.x][players_position.y], CENTER_MODE);
		}
	else
	{
		Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_LIGHTGRAY);
	}

	// Move the minesweeper:
	if (players_position.y != (BOARD_SIZE-1))
		players_position.y++;
	else
		players_position.y = 0;

	// Mark minesweeper's new position:
	Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGRAY);
}

void Move_Left(void)
{
	// Erase the current mark:
	if (visited[players_position.x][players_position.y] == 1)
	{
		Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_RED);
//		Display_No_Of_Mines(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, mine_numbers[players_position.x][players_position.y], CENTER_MODE);
	}
	else if (visited[players_position.x][players_position.y] == 2)
		{
			Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGREEN);
	//		Display_No_Of_Mines(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, mine_numbers[players_position.x][players_position.y], CENTER_MODE);
		}
	else
	{
		Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_LIGHTGRAY);
	}

	// Move the minesweeper:
	if (players_position.x != 0)
		players_position.x--;
	else
		players_position.x = BOARD_SIZE-1;

	// Mark minesweeper's new position:
	Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGRAY);
}

void Move_Right(void)
{
	// Erase the current mark:
	if (visited[players_position.x][players_position.y] == 1)
	{
		Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_RED);
//		Display_No_Of_Mines(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, mine_numbers[players_position.x][players_position.y], CENTER_MODE);
	}
	else if (visited[players_position.x][players_position.y] == 2)
		{
			Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGREEN);
	//		Display_No_Of_Mines(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, mine_numbers[players_position.x][players_position.y], CENTER_MODE);
		}
	else
	{
		Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_LIGHTGRAY);
	}

	// Move the minesweeper:
	if (players_position.x != (BOARD_SIZE-1))
		players_position.x++;
	else
		players_position.x = 0;

	// Mark minesweeper's new position:
	Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGRAY);
}

void Mark_Field(void)
{
	// If visited do nothing
	if (visited[players_position.x][players_position.y] != 0)
	{
		return;
	}
	// If not, display the player's mark:
	else
	{
		visited[players_position.x][players_position.y] = active_player;
		fields_to_visit--;
		if (active_player == 2) {
			Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_DARKGREEN);
		}else if (active_player == 1) {
			Draw_Square(players_position.x*SQUARE_SIZE+1, players_position.y*SQUARE_SIZE+1, SQUARE_SIZE-1, LCD_COLOR_RED);
		}
	}
	//check for win
	Check_Win();
}

void Check_Win(void)
{

	if(visited[0][0]==active_player && visited[1][1]==active_player && visited[2][2]==active_player)
	{
		game_status = active_player;
	}
	else if (visited[0][2]==active_player && visited[1][1]==active_player && visited[2][0]==active_player)
	{
		game_status = active_player;
	}
	else if (visited[0][0]==active_player && visited[0][1]==active_player && visited[0][2]==active_player)
	{
		game_status = active_player;
	}
	else if (visited[1][0]==active_player && visited[1][1]==active_player && visited[1][2]==active_player)
	{
		game_status = active_player;
	}
	else if (visited[2][0]==active_player && visited[2][1]==active_player && visited[2][2]==active_player)
	{
		game_status = active_player;
	}
	else if (visited[0][0]==active_player && visited[1][0]==active_player && visited[2][0]==active_player)
	{
		game_status = active_player;
	}
	else if (visited[1][0]==active_player && visited[1][1]==active_player && visited[1][2]==active_player)
	{
		game_status = active_player;
	}
	else if (visited[2][0]==active_player && visited[2][1]==active_player && visited[2][2]==active_player)
	{
		game_status = active_player;
	}


	if (game_status == 0 && fields_to_visit == 0)
	{
		game_status = 3;
	}


}

void Game_Over(void)
{


	// Clear the screen to get rid of game artifacts:
	BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);

	if (game_status == 1)
	{
	    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 95, (uint8_t *)"Player 1 wins.", CENTER_MODE);
	}
	if (game_status == 2)
	{
	    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 95, (uint8_t *)"Player 2 wins.", CENTER_MODE);
	}
	if (game_status == 3)
	{
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 95, (uint8_t *)"Draw", CENTER_MODE);
	}
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 80, (uint8_t *)"To play again press 'Reset'.", CENTER_MODE);
    while (1);
}


void Draw_Square(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Color)
{
	uint16_t Height = Width;
	uint16_t backup_color = BSP_LCD_GetTextColor();

	BSP_LCD_SetTextColor(Color);
	while(Height--)
	{
		BSP_LCD_DrawHLine(Xpos, Ypos++, Width);
	}
	BSP_LCD_SetTextColor(backup_color);
}

/**
  * @brief Toggle Leds
  * @param  None
  * @retval None
  */
/*
void Toggle_Leds(void)
{
  static uint8_t ticks = 0;
  
  if(ticks++ > 100)
  {
    // BSP_LED_Toggle(LED_BLUE);
    ticks = 0;
  }
}
*/
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  BSP_LED_On(LED_RED);
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
