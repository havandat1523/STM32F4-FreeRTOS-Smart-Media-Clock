// Flash.h - Thêm enum vào đây!
#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

// ✅ Chuyển enum lên đây
typedef enum {
    secter_0, secter_1, secter_2, secter_3,
    secter_4, secter_5, secter_6, secter_7
} secter_t;

#define FLASH_HEN_GIO_ADDR  0x0800C000
#define MAX_HEN_GIO         5

typedef struct {
    uint8_t gio, phut, ngay, thang, nam;
    uint8_t da_dat;
    uint8_t bai_hat;
   // uint8_t du_phong[2];
} HenGio_t;

//typedef struct {
//    uint8_t  so_luong;
//   // uint8_t  du_phong[3];
//    HenGio_t ds[MAX_HEN_GIO];
//} Flash_HenGio_t;

void Erase_Secter(secter_t secter);
void Programing(void* dia_chi, char* data, int size);
void Read_Flash(void* dia_chi, char* doc, int size);

#endif
