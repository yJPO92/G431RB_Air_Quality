/*
 * yI2CprogsRTC.c
 *
 *  Dallas DS1307 real-time clock minimalistic driver
 *  on Vellman vma202 shield
 *  I2C3 (PC0/PC1, A4/A5)
 *  Created on: 24 mars 2019
 *      Author: Jean
 */

#include "stm32g4xx_hal.h"
#include "yI2CprogsRTC.h"

extern I2C_HandleTypeDef hi2c3;

/* BCD & decimal format */
int bcdToDecimal(int bcd) {
    return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}

int decimalToBcd(int dec) {
    return (dec % 10) + ((dec / 10) << 4);
}

/*
 * @brief  Conversion d'un nombre de 2 caracters ascii
 * 			(saisis ds terminal) en codage BCD
 * @param  ascii caractere 1 & 2
 * @retval BCD code
 * @note   !! Verifier qu'il s'agit de chiffre avant d'appeler cette fonction !!
 */
int asciiToBcd(char car1, char car2) {
	return ((car1 & 0x0F) << 4) | (car2 & 0x0F);
}

/*
 * @brief  Lire date & heure dans l'horloge vma202 RTC
 * @param  none
 * @retval date au format Unix
 */
time_t yI2C_RTC_GetDH() {
	// time_t entier temps Unix depuis 1 jan 1970
	// tm structure decompose le temps en entier sec, min, ...
    struct tm now;
    char buffer[7];

    //placer le pointeur de lecture au début de la zone RTC
    buffer[0] = 0x00;
    if (HAL_I2C_Master_Transmit(&hi2c3, RTC_add, (uint8_t *)buffer, 1, 1000) != 0) return 0;
    //lire date & heure
    if (HAL_I2C_Master_Receive(&hi2c3, RTC_add|0x01, (uint8_t *)buffer, 7, 1000) != 0) return 0;

    //extraire info du buffer BCD recu, les mettre en decimal
    if (buffer[0] & 0x80) return 0; // clock stopped
    if (buffer[2] & 0x40) return 0; // 12-hour format not supported
    now.tm_sec = bcdToDecimal(buffer[0] & 0x7F);
    now.tm_min = bcdToDecimal(buffer[1]);
    now.tm_hour = bcdToDecimal(buffer[2] & 0x3F);
    now.tm_mday = bcdToDecimal(buffer[4]);
    now.tm_mon = bcdToDecimal(buffer[5]);
    now.tm_year = bcdToDecimal(buffer[6]) - 1900 + 2000;

    //The C library function time_t mktime(struct tm *timeptr)
    //converts the structure pointed to by timeptr into a time_t value
    //according to the local time zone.
    return mktime(&now);	//fonction retourne uniquement un int ou float, pas de structure ou array
}

/*
 * @brief  Mettre RTC a l'heure
 * @param  char buffer de 12 caractere ascii 'AAMMJJHHMNSC'
 * @retval status
 * @note   Verifier que buffer contient des chiffres avant d'appeler cette fonction !!
 */
int yI2C_RTC_WriteDH(char* buffer) {
	char data[8];

	data[0] = 0x00;	   								//placer le pointeur d'ecriture dans la zone RTC
	data[1] = asciiToBcd(buffer[10], buffer[11]);	//second
	data[2] = asciiToBcd(buffer[8], buffer[9]);		//minute
	data[3] = asciiToBcd(buffer[6], buffer[7]);		//heure
	data[4] = 0;
	data[5] = asciiToBcd(buffer[4], buffer[5]);		//jour
	data[6] = asciiToBcd(buffer[2], buffer[3]);		//mois
	data[7] = asciiToBcd(buffer[0], buffer[1]);		//ann�e

    if (HAL_I2C_Master_Transmit(&hi2c3, RTC_add, (uint8_t *)data, 8, 1000) != 0) return 4;

	return 1;
}

/*
 * @brief  Initialiser RTC : Date & Heure
 * @param  Date Unix
 * @retval integer 1=OK, -1=error
 */
int yI2C_RTC_SetDH(time_t t) {
	    struct tm *now;
	    char buffer[9];

	    now = localtime(&t);

	    buffer[0] = 0x08; // memory address
	    buffer[1] = decimalToBcd(now->tm_sec) & 0x7F; // CH = 0
	    buffer[2] = decimalToBcd(now->tm_min);
	    buffer[3] = decimalToBcd(now->tm_hour) & 0x3F; // 24-hour format
	    buffer[4] = now->tm_wday + 1;
	    buffer[5] = decimalToBcd(now->tm_mday);
	    //buffer[6] = decimalToBcd(now->tm_mon+1);
	    buffer[6] = decimalToBcd(now->tm_mon);
	    //buffer[7] = decimalToBcd(now->tm_year + 1900 - 2000);
	    buffer[7] = decimalToBcd(now->tm_year);
	    buffer[8] = 0x00; // OUT = 0
	    if (HAL_I2C_Master_Transmit(&hi2c3, RTC_add, (uint8_t *)buffer, 9, 1000) != 0) return -1;

	    return 1;
	}

/*
 * @brief  (re)start clock � 00sec
 * @param  none
 * @retval status
 */
int yI2C_RTC_start() {      // start the clock
    char buffer[7];

    buffer[0] = 0x00;		//placer le pointeur de lecture au début de la zone RTC
    buffer[1] = 0x00;		//MZ bit7 CH ==> start clock + second = 0
    if (HAL_I2C_Master_Transmit(&hi2c3, RTC_add, (uint8_t *)buffer, 2, 1000) != HAL_OK) return -4;

    return 1;
}

/*
 * @brief  stop the clock
 * @param  none
 * @retval status
 */
int yI2C_RTC_stop() {       // stop the clock
    char buffer[7];

    buffer[0] = 0x00;		//placer le pointeur de lecture au début de la zone RTC
    buffer[1] = 0x80;		//MU bit7 CH ==> stop clock
    if (HAL_I2C_Master_Transmit(&hi2c3, RTC_add, (uint8_t *)buffer, 2, 1000) != HAL_OK) return -3;

    return 1;
}

/*
 * @brief  Lire les registres dans l'horloge vma202 RTC
 * @param  pointeur sur buffer de retour des valeurs
 * @param  addresse du 1er element
 * @param  nombre d'elements
 * @retval 1=ok, 4ou6=erreur
 */
int yI2C_RTC_GetRegisters(char* buffer, int premElem, int nbElem) {
	char data[nbElem];
	for (int var = 0; var <= nbElem; ++var) {
		data[var] = buffer[var];
	}
    //placer le pointeur de lecture au début de la zone RTC
	data[0] = premElem;
    if (HAL_I2C_Master_Transmit(&hi2c3, RTC_add, (uint8_t *)data, 1, 1000) != 0) return 4;
    //lire les registres
    if (HAL_I2C_Master_Receive(&hi2c3, RTC_add|0x01, (uint8_t *)data, nbElem, 1000) != 0) return 6;
    //ranger les valeurs lues dans le buffer de transmission
	for (int var = 0; var < nbElem; ++var) {
		buffer[var] = data[var];
	}
    return 1;
}

/*
 * @brief  Ecrire les registres dans l'horloge vma202 RTC
 * @param  pointeur sur buffer de retour des valeurs
 * @param  addresse du 1er element
 * @param  nombre d'elements
 * @retval status
 */
int yI2C_RTC_SetRegisters(char* buffer, int premElem, int nbElem) {
	char data[nbElem + 1];
	for (int var = 1; var <= nbElem +1 ; ++var) {
		data[var] = buffer[var -1];
	}
	data[0] = premElem;	    //placer le pointeur d'écriture dans la zone RTC qui commence à l'@ 0x0A
    if (HAL_I2C_Master_Transmit(&hi2c3, RTC_add, (uint8_t *)data, nbElem + 1, 1000) != 0) return 4;
    return 1;
}

/*
 * @brief  RAZ des registres utilisateur
 * @param  value to write in registers
 * @retval status
 */
int yI2C_RTC_ClearRAM(int clsVal) {
	char buffer[2];
	for (int var = DS1307_RAMSTART; var <= DS1307_LASTREG; ++var) {
	    buffer[0] = var;		//placer le pointeur
	    buffer[1] = clsVal;		//valeur a ecrire ds le registre
	    if (HAL_I2C_Master_Transmit(&hi2c3, RTC_add, (uint8_t *)buffer, 2, 1000) != HAL_OK) return -1;
	    HAL_Delay(5);
	}
    return 1;
}

/* That's all folks! */
