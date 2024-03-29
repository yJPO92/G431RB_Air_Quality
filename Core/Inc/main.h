/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define yCubeMX "3-5"
#define BP1_Pin GPIO_PIN_13
#define BP1_GPIO_Port GPIOC
#define BP1_EXTI_IRQn EXTI15_10_IRQn
#define LPUART1_TX_Pin GPIO_PIN_2
#define LPUART1_TX_GPIO_Port GPIOA
#define LPUART1_RX_Pin GPIO_PIN_3
#define LPUART1_RX_GPIO_Port GPIOA
#define vma202LD1_Pin GPIO_PIN_8
#define vma202LD1_GPIO_Port GPIOC
#define vma202sw_Pin GPIO_PIN_8
#define vma202sw_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define UART4_TX_Pin GPIO_PIN_10
#define UART4_TX_GPIO_Port GPIOC
#define UART4_RX_Pin GPIO_PIN_11
#define UART4_RX_GPIO_Port GPIOC
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB
#define LCD_i2c1scl_Pin GPIO_PIN_8
#define LCD_i2c1scl_GPIO_Port GPIOB
#define LCD_i2c1sda_Pin GPIO_PIN_9
#define LCD_i2c1sda_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define yPROG "*** G431 Air Qual ***"
#define yVER  "3.4"
#define yDATE "15-Sep-2023"
//#define yDBG_tk 1        //debug display on terminal (ou commenter ligne)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
