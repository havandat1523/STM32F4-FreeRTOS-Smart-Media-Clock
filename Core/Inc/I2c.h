
#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

void I2C_Init ();
void gui_du_lieu (uint8_t dia_chi, uint8_t* du_lieu, int so_lan_gui);
uint8_t nhan_du_lieu(uint8_t dia_chi, uint8_t* du_lieu, int so_lan_doc);

#endif /* INC_I2C_H_ */
