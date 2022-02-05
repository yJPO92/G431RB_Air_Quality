/**
 *******************************************************************************
 * @file    Air_Quality.h
 * @author  Jean
 * @brief   
 * @version 
 *******************************************************************************
 * Modified :
 * Created  : 1 févr. 2022
 *******************************************************************************
 * @note    
 *******************************************************************************
 */

#ifndef INC_AIR_QUALITY_H_
#define INC_AIR_QUALITY_H_

#include "main.h"

//UART interface with Air Quality Kit
//UART4
#define aqRxBufferSize 1
char bTxBuffer[1];						//buffer d'emission (only 1 char cf. doc)
uint8_t bRxBuffer[aqRxBufferSize];		//buffer de reception
#define yAirQualSize 15
uint8_t yAirQual[yAirQualSize];			//code to interface with AirQual file

/* Menu principal */
#define mmenu1 "\n\
----------------------------------\n\
    Air Quality Kit commndes\n\
\n\
 t   returns tempereture in deg.Cn\
 c   returns eCOS level in ppm\n\
 b   buzzer turn on\n\
 q   buzzer turn off\n\
 a   auto mode\n\
 m   manual mode\n\
 1-6 turn on LED level\n\
 0   turn off LED\n\
----------------------------------\n\
 (W)menu | (E)quit\n\
 Faire un choix ...\n"

void yAirQualMenu(void);

#endif /* INC_AIR_QUALITY_H_ */
//That's all folks!!
