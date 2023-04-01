/*
 * yI2CprogsRTC.h
 *
 *  Dallas DS1307 real-time clock minimalistic driver
 *  on Vellman vma202 shield
 *  I2C3 (PC0/PC1, A4/A5)
 *  Created on: 24 mars 2019
 *      Author: Jean
 */


#ifndef INC_YI2CPROGSRTC_H_
#define INC_YI2CPROGSRTC_H_

#include <time.h>

/* HAL_I2C_function need 8-bits address in uint16_t data */
/* RTC on vma202 */
#define RTC_add (0x68 << 1)		// = 0xD0 (L-Shift 1)
#define DS1307_FREQ 100000      // bus speed
#define DS1307_SEC  0x00        // seconds (00...59)
#define DS1307_MIN  0x01        // min (00...59)
#define DS1307_HOUR 0x02        // hours (00...23)
#define DS1307_DAY  0x03        // day (01...07, 01=Sunday/dimanche)
#define DS1307_DATE 0x04        // date (01...31)
#define DS1307_MONTH 0x05       // month (01..12)
#define DS1307_YEAR 0x06        // year (00...99)
#define DS1307_SQROUT 0x07      // square output register
#define DS1307_RAMSTART 0x08    // register address that ram starts at
#define DS1307_LASTREG 0x3F     // this is the last register in the device (note also this register is used to address everything so it gets clobbered)
#define DS1307_lastram 0x3E     // last usable ram by this class as the lastreg is clobbered by code for normal operation

// time_t entier temps Unix depuis 1 jan 1970
// tm structure decompose le temps en entier sec, min, ...

struct BCDtime_t {
    int JJ;
    int MM;
    int AA;
    int HR;
    int MN;
    int SC;
    //string Date;
    char Date[10];
    char Heure[8];
};
struct BCDtime_t RTCtimeBCD;

int bcdToDecimal();

int decimalToBcd();

int asciiToBcd(char car1, char car2);

time_t yI2C_RTC_GetDH();

int yI2C_RTC_SetDH(time_t t);

int yI2C_RTC_start();

int yI2C_RTC_stop();

int yI2C_RTC_GetRegisters(char* buffer, int premElem, int nbElem);

int yI2C_RTC_WriteDH(char* buffer);

int yI2C_RTC_SetRegisters(char* buffer, int premElem, int nbElem);

int yI2C_RTC_ClearRAM(int clsVal);

#endif /* INC_YI2CPROGSRTC_H_ */
