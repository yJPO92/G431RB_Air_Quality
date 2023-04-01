/*
 * ySPIprogsSD.c
 *
 *  @brief: Gestion SPI pour SD Card on vma202
 *  Created on: 14 avr. 2019
 *      Author: Jean
 */

#include "stm32g4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "ySPIprogsSD.h"
//#include "fatfs.h"

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef hlpuart1;
extern char aTxBuffer[2048];		//buffer d'emission

//mesurer temps d'un bout de code
uint32_t start, end;

/* Create aliases for *printf to integer variants *iprintf */
//extern __attribute__ ((alias("iprintf"))) int printf(const char *fmt, ...);

/* Size of buffer */
#define BUFFERSIZEsTX                       (COUNTOF(sTxBuffer) - 1)
#define BUFFERSIZEsRX                       (COUNTOF(sRxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Buffer used for transmission */
uint8_t sTxBuffer[] = "****SPI - SD carte write based on Interrupt **** SPI Message ******** SPI Message ******** SPI Message ****";

/* Buffer used for reception */
uint8_t sRxBuffer[256];

FIL fpw;	//file en ecriture
FIL fpr;	//file en lecture

//vu ds l'example
FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t sdtotal, sdfree;
char sdbuffer[100];

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(const char *function, char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	//snprintf(aTxBuffer, 1024, "\nHandler error in %s at %d\n\t...infinite loop...\n", __FILE__,__LINE__);
	snprintf(aTxBuffer, 1024, "\nHandler error in %s of %s at %d\t...on continu...\n",function, file,line);
	HAL_UART_Transmit(&hlpuart1,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	//	while(1)
	//	{
	//	}
	/* USER CODE END Error_Handler_Debug */
}

/*
 * @brief  Ecrire sur la carte SD via SPI
 * @param  Nom du fichier sur carte SD
 * @param  Texte à écrire
 * @retval status
 */
int ySPI_SD_Write(TCHAR* SDfichier, TCHAR* leTexte) {

	start = HAL_GetTick(); 	// Lancement de la mesure du temps

	/* Mount SD Card */
	if(f_mount(&fs, "0", 0) != FR_OK)
		_Error_Handler(__FUNCTION__, __FILE__, __LINE__);

	/* Open file to write */
	if(f_open(&fil, SDfichier, FA_OPEN_ALWAYS | FA_READ | FA_WRITE | FA_OPEN_APPEND) != FR_OK)
		_Error_Handler(__FUNCTION__,__FILE__, __LINE__);

	// Ne pas tester 'free space' car tres tres long 1100ms!!
//	/* Check free space */
//	if(f_getfree("0", &fre_clust, &pfs) != FR_OK)
//		_Error_Handler(__FUNCTION__,__FILE__, __LINE__);
//
//	sdtotal = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
//	sdfree = (uint32_t)(fre_clust * pfs->csize * 0.5);
//
//	/* Free space is less than 1kb */
//	if(sdfree < 1)
//		_Error_Handler(__FUNCTION__,__FILE__, __LINE__);

	/* Writing text */
	//f_puts("STM32 SD Card I/O Example via SPI\n", &fil);
	//f_puts("Save the world!!!\n", &fil);
	f_puts(leTexte, &fil);

	/* Close file */
	if(f_close(&fil) != FR_OK)
		_Error_Handler(__FUNCTION__,__FILE__, __LINE__);

	/* Unmount SDCARD */
	if(f_mount(NULL, "", 1) != FR_OK)
		_Error_Handler(__FUNCTION__,__FILE__, __LINE__);

	end = HAL_GetTick();	// Arret de la mesure

	return (end - start);
}

/*
 * @brief  Lire sur la carte SD via SPI
 * @param  Nom du fichier sur carte SD
 * @retval status
 */
int ySPI_SD_Read(TCHAR* SDfichier) {
	start = HAL_GetTick(); 	// Lancement de la mesure du temps

	/* Mount SD Card */
	if(f_mount(&fs, "0", 1) != FR_OK)
		_Error_Handler(__FUNCTION__,__FILE__, __LINE__);

	  /* Open file to read */
	  if(f_open(&fil, SDfichier, FA_READ) != FR_OK)
			_Error_Handler(__FUNCTION__,__FILE__, __LINE__);

	  while(f_gets(sdbuffer, sizeof(sdbuffer), &fil))
	  {
		HAL_UART_Transmit(&hlpuart1,(uint8_t *) sdbuffer, strlen(sdbuffer), 5000);
	  }

	  /* Close file */
	  if(f_close(&fil) != FR_OK)
			_Error_Handler(__FUNCTION__,__FILE__, __LINE__);

	  /* Unmount SDCARD */
	  if(f_mount(NULL, "0", 1) != FR_OK)
			_Error_Handler(__FUNCTION__,__FILE__, __LINE__);

	  end = HAL_GetTick();	// Arret de la mesure

	  return (end - start);
}


//FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);			/* Open or create a file */
//FRESULT f_close (FIL* fp);										/* Close an open file object */
//FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from the file */
//FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to the file */

///* File access mode and open method flags (3rd argument of f_open) */
//#define	FA_READ				0x01
//#define	FA_WRITE			0x02
//#define	FA_OPEN_EXISTING	0x00
//#define	FA_CREATE_NEW		0x04
//#define	FA_CREATE_ALWAYS	0x08
//#define	FA_OPEN_ALWAYS		0x10
//#define	FA_OPEN_APPEND		0x30

/* That's all folks! */
