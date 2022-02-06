/**
 *******************************************************************************
 * @file    Air_Quality.c
 * @author  Jean
 * @brief   ensemble de fonctions pour traitement Air Quality Kit
 * @version 1.0
 *******************************************************************************
 * Modified :
 * Created  : 1 févr. 2022
 *******************************************************************************
 * @note    
 *******************************************************************************
 */

#include "Air_Quality.h"
#include "VT100.h"
#include <stdio.h>
#include <string.h>

//lpuart1 PC console
extern char aTxBuffer[1024];
extern UART_HandleTypeDef hlpuart1;

void yAirQualMenu(void){
	  snprintf(aTxBuffer, 1024, mmenu1 DECSC);
	  HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
}

void yAirQualTemp(uint8_t* airqual) {
	snprintf(aTxBuffer, 1024, CUP(11,40) "%s" ERASELINE DECRC, airqual);
	HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
}

void yAirQualeCOS(uint8_t* airqual) {
	snprintf(aTxBuffer, 1024, CUP(12,40) "%s" ERASELINE DECRC, airqual);
	HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
}

void yAirQualReceived(uint8_t* airqual) {
	//-- debug
	//snprintf(aTxBuffer, 1024, DECRC "\t\tfunc yAirQualReceived %s\r\n", airqual);
	//HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	memset(yTempCos,0,yAirQualSize);		//- Zero Interface Buffer

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
	}
	if (airqual[0] == 'c') {
		yAirQualeCOS(yTempCos);
	}
}


//That's all folks!!
