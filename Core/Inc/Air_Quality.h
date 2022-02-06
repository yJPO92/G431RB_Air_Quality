/**
 *******************************************************************************
 * @file    Air_Quality.h
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

#ifndef INC_AIR_QUALITY_H_
#define INC_AIR_QUALITY_H_

#include "main.h"

/* Interface with Air Quality Kit */
// UART4
#define aqRxBufferSize 1
char bTxBuffer[1];						//buffer d'emission (only 1 char cf. doc)
uint8_t bRxBuffer[aqRxBufferSize];		//buffer de reception
#define yAirQualSize 15
uint8_t yAirQual[yAirQualSize];			//code to interface with AirQual file
uint8_t yTempCos[yAirQualSize];

/* Menu principal */
#define mmenu1 "\n\
----------------------------------\n\
    Air Quality Kit commandes\n\
\n\
 t   returns temperature in deg.C\n\
 c   returns eCOS level  in ppm\n\
 b   buzzer turn on\n\
 q   buzzer turn off\n\
 a   auto mode\n\
 m   manual mode\n\
 1-6 turn on LED level\n\
 0   turn off LED\n\
 r   affichage continu\n\
----------------------------------\n\
 Display this menu | Shutdown\n\
 Faire un choix ...\n"

/*
 *  (Re)Afficher menu
 */
void yAirQualMenu(void);

/*
 * Affiche la temperature en 11-40 (lg,col)
 */
void yAirQualTemp(uint8_t* airqual);

/*
 * Affiche le niveau de eCOS en 12-40 (lg,col)
 */
void yAirQualeCOS(uint8_t* airqual);

/*
 * Traite le message recu du Air Quality Kit
 */
void yAirQualReceived(uint8_t* airqual);


#endif /* INC_AIR_QUALITY_H_ */
//That's all folks!!
