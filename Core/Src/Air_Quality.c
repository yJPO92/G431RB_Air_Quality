/**
 *******************************************************************************
 * @file    Air_Quality.c
 * @author  Jean
 * @brief   ensemble de fonctions pour traitement Air Quality Kit
 * @version 1.0
 *******************************************************************************
 * Modified :
 * Created  : 1 fevr. 2022
 *******************************************************************************
 * @note    
 *******************************************************************************
 */

#include "VT100.h"
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "yI2CprogsLCD.h"
#include "Air_Quality.h"

//lpuart1 PC console
extern char aTxBuffer[1024];
extern UART_HandleTypeDef hlpuart1;

void yAirQualMenu(void){
	  snprintf(aTxBuffer, 1024, mmenu1 DECSC);
	  HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
}

void yAirQualTemp(uint8_t* airqual) {
	//save value
	//yAirQual_Temp[0] = 'T'; yAirQual_Temp[1] = ':';
	for (int j = 0; j < 5; ++j) {
		yAirQual_Temp[j] = airqual[j];
	}
}

void yAirQualeCOS(uint8_t* airqual) {
	//save value
	//yAirQual_eCO2[0] = 'C'; yAirQual_eCO2[1] = ':';
	for (int j = 0; j < 7; ++j) {
		yAirQual_eCO2[j] = airqual[j];
	}
}

void yAirQualReceived(uint8_t* airqual) {
	//-- debug
	//snprintf(aTxBuffer, 1024, DECRC "\t\tfunc yAirQualReceived %s\r\n", airqual);
	//HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);

	//- Zero Interface Buffer
	memset(yTempCos,0,yAirQualSize);

	//-- Calculer longueur de la chaine utile, sans "t=", "c=" et '\r\n'
	int posN = strlen((char*)airqual) - 4;
	//-- recopier la partie utile de la chaine recue
	int c = 0;
	while (c < posN) {
		yTempCos[c] = airqual[3+c-1];
		c++;
	}
	//-- en fonction du premier caractere
	if (airqual[0] == 't') {
		yAirQualTemp(yTempCos);
		snprintf(aTxBuffer, 1024, CUP(11,40) "%s" ERASELINE DECRC, yAirQual_Temp);
		HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	}
	if (airqual[0] == 'c') {
		yAirQualeCOS(yTempCos);
		snprintf(aTxBuffer, 1024, CUP(12,40) "%s" ERASELINE DECRC, yAirQual_eCO2);
		HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	}
}

void yAirQualRepeatVT(void){
	//-- lire temp
	snprintf(aTxBuffer, 10, "t");
	HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	//-- attendre reponse et traitement
	HAL_Delay(200);
	//-- lire eCO2
	snprintf(aTxBuffer, 10, "c");
	HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
}

void yAirQualRepeatLCD(void) {
	//-- lire temp
	snprintf(aTxBuffer, 10, "t");
	HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	//-- attendre reponse et traitement
	HAL_Delay(100);
	//-- lire eCO2
	snprintf(aTxBuffer, 10, "c");
	HAL_UART_Transmit(&huart4,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	// -- afficher
	yI2C_LCD_locate(0,1);
	snprintf(aTxBuffer, 16, "T:%s C:%s", yAirQual_Temp, yAirQual_eCO2);
	yI2C_LCD_Affich_Txt(aTxBuffer);
}

//That's all folks!!
