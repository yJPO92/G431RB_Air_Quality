/*
 * ySPIprogsSD.h
 *
 *  @brief: Gestion SPI pour SD Card on vma202
 *  Created on: 14 avr. 2019
 *  Modified  : 05 mar 2021, ajout params
 *      Author: Jean
 */

#ifndef INC_YSPIPROGSSD_H_
#define INC_YSPIPROGSSD_H_

#define SDfileBoot	"Nucleo.txt"
#define SDfileTrace "aqk.txt"

#include "stm32g4xx_hal.h"
#include "fatfs.h"

int ySPI_SD_Read(TCHAR* SDfichier);

int ySPI_SD_Write(TCHAR* SDfichier, TCHAR* leTexte);

#endif /* INC_YSPIPROGSSD_H_ */
