/*
 * DFPLAYER.h
 *
 *  Created on: Apr 12, 2026
 *      Author: havandat
 */

#ifndef INC_DFPLAYER_H_
#define INC_DFPLAYER_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

typedef struct
{
	UART_HandleTypeDef* DFP_UART;
	uint8_t SendBuff[10];
	uint16_t Checksum;
}DFPLAYER_Name;

#define DFP_PLAYTRACK 				0X12
#define DFP_NEXT 							0X01
#define DFP_PREV		  				0X02
#define DFP_SETVOLUME 				0X06
#define DFP_PLAY		 				 	0X0D
#define DFP_PAUSE		 				  0X0E
#define DFP_STOP					  	0X16
#define DFP_RANDOM 						0X18
#define DFP_PLAYFILEINFOLDER  0X0F

void DFPLAYER_Init(DFPLAYER_Name* MP3, UART_HandleTypeDef* UART);
void DFPLAYER_PlayTrack(DFPLAYER_Name* MP3, uint16_t num);
void DFPLAYER_Next(DFPLAYER_Name* MP3);
void DFPLAYER_Prev(DFPLAYER_Name* MP3);
void DFPLAYER_SetVolume(DFPLAYER_Name* MP3, uint16_t volume);
void DFPLAYER_Play(DFPLAYER_Name* MP3);
void DFPLAYER_Pause(DFPLAYER_Name* MP3);
void DFPLAYER_Stop(DFPLAYER_Name* MP3);
void DFPLAYER_RandomPlay(DFPLAYER_Name* MP3);
void DFPLAYER_PlayFileInFolder(DFPLAYER_Name* MP3, uint8_t folder, uint32_t num);
void DFPLAYER_Quet_all(DFPLAYER_Name* MP3);




#endif /* INC_DFPLAYER_H_ */
