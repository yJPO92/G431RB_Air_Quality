/**
 *******************************************************************************
 * @file    Interrupts_Callbacks.c
 * @author  Jean
 * @brief   Callback: BP 1, Keyboard, ADC, RTC_AlarmA...
 * @version    
 *******************************************************************************
 * Modified : 19-02-2022 (deactiver/reactiver lpuart1)
 * Modified : 31-01-2022
 * Created  : 29-08-2020
 *******************************************************************************
 * @note    le prog se bloque s'il n'y a pas cnx lpuart1 avec alim externe
 *			parade: deactiver/reactiver lpuart1 via BP1
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include "tim.h"
#include "usart.h"
#include "Air_Quality.h"
#include "VT100.h"

//Console interface (et debug print)
extern char aTxBuffer[1024];
extern uint8_t aRxBuffer[3];		//buffer de reception
extern uint16_t uart2NbCar;			//nb de byte attendu
extern uint8_t yCarRecu;			//caractere recu
uint8_t i = 0;

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
	//UART4->CR2 |= 0x0A000000;	//pas ici ou syntaxe differente!!!
	//huart4.Instance->CR2 |= 0x0A000000;	//pas mieux, faut-il stopper l'uart?
	__HAL_UART_ENABLE_IT(&huart4,UART_IT_RXNE + UART_IT_CM);
	HAL_UART_Receive_IT(&huart4, bRxBuffer, aqRxBufferSize);

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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	/** PC13 GPIO_EXTI13 (B1 blue button) */
	if(GPIO_Pin == BP1_Pin) {
		snprintf(aTxBuffer, 1024, DECRC "\tdebug: BP1 down\r\n");
		HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
		//-- changer frequence de blink
		if (htim1.Instance->PSC == 4999) {
			//- blink lent
			htim1.Instance->PSC = 20000 - 1;
			//- deactiver lpuart1
			HAL_UART_DeInit(&hlpuart1);
		} else {
			//- blink rapide
			htim1.Instance->PSC = 5000 - 1;
			//- re init et re activer lpuart1
			HAL_UART_Init(&hlpuart1);
			uart2NbCar = 1;
			__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);	//UART read data register not empty interruption (uart2NbCar =1|2|??)
			HAL_UART_Receive_IT(&hlpuart1, aRxBuffer, uart2NbCar);
		}
	}

	/** autre entree interrupt */
} //callback gpio

/*
 * USARTs callback
 * don't forget to start!
 * 		 __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);	//start USART1
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	/** USART4 - Air Quality Kit interface */
	if (huart->Instance == UART4) {
		__NOP();
		if (UART4->ISR & USART_ISR_CMF) {
			yAirQual[i] = bRxBuffer[0];
			i =0;
//			snprintf(aTxBuffer, 1024, DECRC "\tdebug: recu from uart4 %s\r\n", yAirQual);		//debug
//			HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);	//debug
			//-- traiter la reponse
			yAirQualReceived(yAirQual);
			memset(yAirQual,0,yAirQualSize);		//- Zero Interface Buffer
			//-- re activer la reception jusqu'a '\n'
			huart4.Instance->CR1 |= 0x00004000;		// set CMIE
			__HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_TC + UART_FLAG_CMF + UART_FLAG_RXNE);
			__HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_CMF);
			__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE + UART_IT_CM);
			HAL_UART_Receive_IT(&huart4, bRxBuffer, 1);
		} else {
			yAirQual[i] = bRxBuffer[0];
			i++;
			__HAL_UART_ENABLE_IT(&huart4,UART_IT_RXNE + UART_IT_CM);
			HAL_UART_Receive_IT(&huart4, bRxBuffer, 1);
		}
	}//if usart4

	/** LPUART1 - console interface */
	if (huart->Instance == LPUART1) {
		snprintf(aTxBuffer, 1024, DECRC "\tchoix %c " ERASELINE, aRxBuffer[0]);
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
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

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
//	//--- reveill par RTC AlarmA
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
		yFlagTIM1 = 1;	//timer fired
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
