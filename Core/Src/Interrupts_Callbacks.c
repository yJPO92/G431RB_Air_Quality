/**
 *******************************************************************************
 * @file    Interrupts_Callbacks.c
 * @author  Jean
 * @brief   Callback: BP 1, Keyboard, ADC, RTC_AlarmA...
 * @version    
 *******************************************************************************
 * Modified : 31-01-2022
 * Created  : 29-08-2020
 *******************************************************************************
 * @note    
 *
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include "tim.h"
#include "usart.h"


//extern UART_HandleTypeDef huart2;
//extern DMA_HandleTypeDef hdma_usart2_rx;
//extern DMA_HandleTypeDef hdma_usart2_tx;

//extern yMENU_t mnuSTM;

//extern uint8_t aTxBuffer[];

//pour debug print
char aTxBuffer[1024];
extern uint8_t aRxBuffer[3];		//buffer de reception
extern uint16_t uart2NbCar;			//nb de byte attendu
extern uint8_t yCarRecu;			//caractere recu

extern uint8_t bRxBuffer[20];		//2nd uart4 buffer de reception
extern char bTxBuffer[1];			//2nd uart4 buffer d'emission
extern uint8_t yAirQual;			//code to interface with AirQual file

/* Real Time Clock */
//RTC_TimeTypeDef nr_Time;
//RTC_DateTypeDef nr_Date;

/*
  * @brief  Initialiser routes les interrupts du projet
  * @param  none
  * @retval none
*/
void Interrputs_Init(void) {
	//--- start LPUART1
	//** Activer la reception sur interrupt
	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);	//UART read data register not empty interruption (uart2NbCar =1|2|??)
	HAL_UART_Receive_IT(&hlpuart1, aRxBuffer, uart2NbCar);

	//--- start UART4
	//** Activer reception juqu'au char '\n'
    //UART4->CR2 |= 0x0A000000;
	__HAL_UART_CLEAR_IT(&huart4, UART_CLEAR_CMF);
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_CM);
	HAL_UART_Receive_IT(&huart4, bRxBuffer, 5);
	//HAL_UART_Receive_DMA(&huart4, bRxBuffer, 20);

	//--- enable DMA on USART2
	//??
//	__HAL_DMA_ENABLE_IT(&huart2, DMA???)

	//--- disable RTC Alarm
//    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);

    //--- start ADC acquisition via DMA
	//HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adcbuf,2);

    //--- start TIM1 (ADC1 schedule)
	//HAL_TIM_Base_Start(&htim1);

    //--- demarrer les timers
    HAL_TIM_Base_Start_IT(&htim1);

	return;
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/** PC13 GPIO_EXTI13 (B1 blue button) */
	if(GPIO_Pin == BP1_Pin) {
		snprintf(aTxBuffer, 1024, "\tdebug: BP1 down\r\n");
		HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	}

	/** autre entree interrupt */
} //callback gpio

/*
 * USARTs callback
 * don't forget to start!
 * 		 __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);	//start USART1
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	/** USART4 - Air Quality Kit interface */
	if (huart->Instance == UART4) {
		__NOP();
		if (UART4->ISR & USART_ISR_CMF) {
			snprintf(aTxBuffer, 1024, "\tdebug: recu from uart4 %s\r\n", bRxBuffer);
			HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
			//* re activer la reception jusqu'a "'\n'
			__HAL_UART_CLEAR_IT(&huart4, UART_CLEAR_CMF);
			__HAL_UART_ENABLE_IT(&huart4, UART_IT_CM);
			memset(bRxBuffer,0,5);		//- Zero Receiving Buffer
			HAL_UART_Receive_IT(&huart4, bRxBuffer, 5);
		}
	}//if usart4

	/** LPUART1 - console interface */
	if (huart->Instance == LPUART1) {
		snprintf(aTxBuffer, 1024, "\tdebug: keydown %c\r\n", aRxBuffer[0]);
		HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
		yCarRecu = aRxBuffer[0];	//send back to main prog
		//** Re Activer la reception sur interrupt
		__HAL_UART_CLEAR_IT(&hlpuart1, UART_CLEAR_CTSF);
		__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);
		HAL_UART_Receive_IT(&hlpuart1, aRxBuffer, uart2NbCar);
	}

	/** manage an other usart if any! */

}	//callback uart RX

/*
 * @brief  UART transfer complete callback
 * @param  None
 * @retval None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	/** USART4 - Air Quality Kit interface */
	if (huart->Instance == UART4) {
			__NOP();
	}//if usart4

	/** LPUART1 - console interface */
	if (huart->Instance == LPUART1) {
		__NOP();
		//** Re Activer la reception sur interrupt
		__HAL_UART_CLEAR_IT(&hlpuart1, UART_CLEAR_CTSF);
		__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);
		HAL_UART_Receive_IT(&hlpuart1, aRxBuffer, uart2NbCar);
	}//if lpuart1

	/** manage an other usart if any! */

}	//callback uart TX


///**
//* @brief This function handles DMA RX interrupt request.
//* @param None
//* @retval None
//*/
//void USARTx_DMA_RX_IRQHandler(void)
//{
//HAL_DMA_IRQHandler(huart2.hdmarx);
//}


///**
//  * @brief  Alarm A callback.
//  * @param  hrtc RTC handle
//  * @retval None
//  */
//void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
//{
//	//--- reveill� par RTC AlarmA
//	snprintf(mnuSTM.Buffer, 1024, CUP(9,50) "RTC Alarm A flag  ");
//	HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
//	RTC_AlarmA_flag = 1;
//} //callback rtc alarm

/**
  * @brief  TIMers callback
  * @param
  * @retval status
*/
//yToCheck ==> timer callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	/** TIM1 - clignotement LD2 */
	if(htim->Instance == TIM1) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
	} //TIM1

	/** TIM11 - affichage heure sur LCD */
//	if(htim->Instance == TIM11) {
//		//Afficher date & heure
////		yDisplayDate();
////		yDisplayHeure();
//	} //TIM11

	/* manage an other TIM if any! */
}	//TIMs callback

//That's all folks!!
