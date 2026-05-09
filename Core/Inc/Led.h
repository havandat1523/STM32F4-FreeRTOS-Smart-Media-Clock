/*
 * Led.h
 *
 *  Created on: Apr 2, 2026
 *      Author: havandat
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

void set1_0(int hig_low);
void gui_1_led(uint8_t r, uint8_t g, uint8_t b);
void chot_du_lieu_led();
void tat_den();
void gui(int a);

#endif /* INC_LED_H_ */
