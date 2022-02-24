/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "VT100.h"
#include <stdio.h>
#include <string.h>
#include "Air_Quality.h"
#include "yI2CprogsLCD.h"

//Quelle est la cible?
//with '-fno-diagnostics-show-caret'
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#if defined(STM32F401xx)
#pragma message("Compiling for NUCLEAO_F401RE")
//Grove_LCD_RGB_Backlight rgbLCD(P0_27, P0_28);     //I2C0 SDA & SCL (I2C @ in ....LCD_RBG...h)
//----- end F401RE -----
#elif defined(STM32G431xx)
#pragma message("***************************")
#pragma message("Compiling for NUCLEO_G431RB")
#pragma message("le " __STR1__(__DATE__)" "__STR1__(__TIME__))
#pragma message("---------------------------")
#pragma message("program " __STR1__(yPROG))
#pragma message("version " __STR1__(yVER))
#pragma message("CubeMX  " __STR1__(yCubeMX))
#pragma message("***************************\n")
//Grove_LCD_RGB_Backlight rgbLCD(PB_9, PB_8);     	//I2C1 SDA & SCL (I2C @ in ....LCD_RBG...h)
//----- end L476RG ------
#elif defined(STM32L476xx)
#pragma message("***************************")
#pragma message("Compiling for NUCLEO_L476RG")
#else
#warning "Unknown TARGET"
#endif

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
//Console interface (et debug print)
uint8_t aRxBuffer[3];		//lpuart1 debug buffer de reception
char aTxBuffer[1024];		//lpuart1 debug buffer d'emission
uint16_t uart2NbCar;		//nb de byte attendu
uint8_t yCarRecu;			//caractere recu (echange entre ISR uart et main)

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
extern void Interrputs_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
 * General Welcome and signing display
 */
void DisplayWelcome(void) {
	snprintf(aTxBuffer, 1024, clrscr homescr
			"\nBonjour maitre!"
			"\n(c)Jean92, " yDATE
			"\n" yPROG " " yVER " (" yCubeMX ")"
			"\nCompil: " __TIME__ " le " __DATE__
			"\nSTmicro NUCLEO_G431RB"
			"\n" DECSC);
	if (HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000) != HAL_OK) {
		Error_Handler();
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	yCarRecu = '*';
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
  MX_TIM1_Init();
  MX_LPUART1_UART_Init();
  MX_UART4_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

	/* message de bienvenue */
	snprintf(aTxBuffer, 1024, "\n---- (re)Start prog ---\n");
	HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);

	//--- test led Led on Nucleo
	for (int ii = 0; ii < 30; ++ii) {
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		HAL_Delay(30);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		HAL_Delay(30);
	}

	//--- display welcome & menu
	DisplayWelcome();
	yAirQualMenu();
	yFlagRepeatVT = 0;

	//--- initialize interrupts & start uart receive it
	uart2NbCar = 1;
	Interrputs_Init();

	//--- init LCD, msg de bienvenue
	yI2C_LCD_init();
	HAL_Delay(50);
	yI2C_LCD_locate(0,0); yI2C_LCD_Affich_Txt("* Air Quality *");
	yI2C_LCD_locate(0,1); yI2C_LCD_Affich_Txt(yVER);
	yI2C_LCD_locate(7,1); yI2C_LCD_Affich_Txt(__TIME__);
	HAL_Delay(2000);

	//--- petit delai final
	HAL_Delay(500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		/* check command received (see Air_Quality_Spec.txt) */
		if (yCarRecu != '*') {
			switch (yCarRecu) {
			//--- Air Quality cmd
			case 't': case 'T':
				snprintf(aTxBuffer, 1024, "%c", yCarRecu);
				HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			case 'c': case 'C':
				snprintf(aTxBuffer, 1024, "%c", yCarRecu);
				HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			case 'b': case 'B':
				snprintf(aTxBuffer, 1024, "%c", yCarRecu);
				HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			case 'q': case 'Q':
				snprintf(aTxBuffer, 1024, "%c", yCarRecu);
				HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			case 'a': case 'A':
				snprintf(aTxBuffer, 1024, "%c", yCarRecu);
				HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			case 'm': case 'M':
				snprintf(aTxBuffer, 1024, "%c", yCarRecu);
				HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6':
				snprintf(aTxBuffer, 1024, "%c", yCarRecu);
				HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
				//--- cmd supplementaire
			case 'r': case 'R':		//-- lecture repetitive de T et eCO2
				snprintf(aTxBuffer, 1024, "\tLecture continue ('r' pour arreter)" ERASELINE DECRC);
				HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				yFlagRepeatVT = !yFlagRepeatVT;
				break;
			case 'l': case 'L':
				snprintf(aTxBuffer, 1024, "\tLecture continue ('l' pour arreter)" ERASELINE DECRC);
				HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				yFlagRepeatLCD = !yFlagRepeatLCD;
				break;

				//--- cmd console
			case 'd': case 'D':		//-- re afficher le menu
				DisplayWelcome();
				yAirQualMenu();
				break;
			case 's': case 'S':		//-- shutdown
				//todo cmd Shutbown
				snprintf(aTxBuffer, 1024, "\tin progress" ERASELINE DECRC);
				HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			default:
				snprintf(aTxBuffer, 1024, "\tcommande erronee" ERASELINE "\n" ERASELINE DECRC);
				HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			}
			yCarRecu = '*';
			//HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);	//echo console
		}

		/* lecture repetitve demandee? */
		if (yFlagRepeatVT == 1) {
			if (yFlagTIM1 == 1) {
				yAirQualRepeatVT();
				yFlagTIM1 = 0; 	//reset
			}
		}
		if (yFlagRepeatLCD == 1 || HAL_GPIO_ReadPin(vma202sw_GPIO_Port, vma202sw_Pin) == 1) {
			if (yFlagTIM1 == 1) {
				yAirQualRepeatLCD();
				yFlagTIM1 = 0; 	//reset
			}
		}

		/* check vma202 switch */
		// display on LCD (at end of 2nd line)
		//snprintf(aTxBuffer, 4, "sw%d",HAL_GPIO_ReadPin(vma202SW_GPIO_Port, vma202SW_Pin));
		//yI2C_LCD_locate(13,1); yI2C_LCD_Affich_Txt(aTxBuffer);


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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
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

