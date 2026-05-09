/*
 * I2c.c
 *
 *  Created on: Mar 21, 2026
 *      Author: havandat
 */
#include "I2c.h"

void I2C_Init (){
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();
	uint16_t* CR1 = (uint16_t*)0x40005400;
	uint32_t* MODER = (uint32_t*)0x40020400;
	uint32_t* AFRL = (uint32_t*)0x40020420;
	uint16_t* CR2 = (uint16_t*)0x40005404;
	uint16_t* CCR = (uint16_t*)0x4000541c;
	uint32_t* PUPDR = (uint32_t*)0x4002040c;
	uint32_t* OTYPER = (uint32_t*)0x40020404;
	uint32_t* OSPEEDR = (uint32_t*)0x40020408;
	uint16_t* TRISE = (uint16_t*)0x40005420;
	*MODER &=~ 0xf<<12;
	*MODER |= 0b1010 <<12;
	*PUPDR &=~ 0xff <<12;
	*PUPDR |= 0b0101<<12;
	*OSPEEDR &=~ 0xff<<12;
	*OSPEEDR |= 0b1111<<12;
	*OTYPER |= (1 << 6) | (1 << 7);
	*AFRL &=~ (0xff << 24);
	*AFRL |= 0b0100<<24;
	*AFRL |= 0b100<<28;

	*CR1 |= (1 << 15); // Bật cờ SWRST (Software Reset)
	 for(int i=0; i<1000; i++);
	 *CR1 &= ~(1 << 15); // Tắt cờ SWRST để nó hoạt động bình thường
	*CR1 &=~ 1<<0;

	*CR2 = 42;
	*CCR = 210;
	//*CCR = (1 << 15) | 35;
	*TRISE &=~ (0x3F<<0);
	*TRISE = 43;
	//*TRISE = 14;
	*CR1 |= 1<<0;
}
void gui_du_lieu (uint8_t dia_chi, uint8_t* du_lieu, int so_lan_gui){
	uint8_t dia_chi1 = (dia_chi<<1);
	uint16_t* CR1 = (uint16_t*)0x40005400;
	uint16_t* SR1 = (uint16_t*)0x40005414;
	uint16_t* SR2 = (uint16_t*)0x40005418;
	uint16_t*DR = (uint16_t*)0x40005410;
	*CR1 |= 1<<8;
	while ((*SR1&1)!=1);
	*DR = dia_chi1;
	while (((*SR1>>1)&1)!=1);
	uint32_t temp = *SR2;
	for(int i=0;i<so_lan_gui;i++){
		while (((*SR1>>7)&1)!=1);
		*DR = du_lieu[i];
		while (((*SR1>>2)&1)!=1);
		while (((*SR1>>10)&1)==1);
		*SR1 &=~ 1<<10;
	}
		*CR1 |= 1<<9;
}
uint8_t nhan_du_lieu(uint8_t dia_chi, uint8_t* du_lieu, int so_lan_doc) {
    uint8_t dia_chi1 = (dia_chi << 1) | 1; // Bit cuối là 1 (READ)
    uint16_t* CR1 = (uint16_t*)0x40005400;
    uint16_t* SR1 = (uint16_t*)0x40005414;
    uint16_t* SR2 = (uint16_t*)0x40005418;
    uint16_t* DR  = (uint16_t*)0x40005410;

    // 1. CHUẨN BỊ: Bật sẵn cờ ACK (Bit 10) trước khi làm bất cứ việc gì
    *CR1 |= (1 << 10);

    // 2. Tạo START và gửi địa chỉ
    *CR1 |= (1 << 8);
    while (((*SR1 >> 0) & 1)!= 1); // Chờ SB

    *DR = dia_chi1;
    while (((*SR1 >> 1) & 1) != 1); // Chờ ADDR (Slave đã nhận diện đúng tên)

    // ==========================================
    // TRƯỜNG HỢP 1: CHỈ ĐỌC ĐÚNG 1 BYTE
    // ==========================================
    if (so_lan_doc == 1) {
        // LUẬT CỦA ST: Phải tắt ACK TRƯỚC KHI xóa cờ ADDR
        *CR1 &= ~(1 << 10);

        // Đọc SR2 để xóa cờ ADDR
        uint32_t temp = *SR2;
        (void)temp;

        // LUẬT CỦA ST: Phải đặt lệnh STOP ngay lập tức
        *CR1 |= (1 << 9);

        // Cuối cùng mới thong thả chờ cờ RxNE (Bit 6) và hốt dữ liệu về
        while (((*SR1 >> 6) & 1) == 0);
        du_lieu[0] = *DR;
    }
    // ==========================================
    // TRƯỜNG HỢP 2: ĐỌC NHIỀU BYTE (so_lan_doc > 1)
    // ==========================================
    else {
        // Xóa cờ ADDR bình thường
        uint32_t temp = *SR2;
        (void)temp;

        for (int i = 0; i < so_lan_doc; i++) {
            // NẾU ĐÂY LÀ BYTE CUỐI CÙNG CHUẨN BỊ NHẬN VỀ
            if (i == (so_lan_doc - 1)) {
                *CR1 &= ~(1 << 10); // Tắt ACK (Tạo NACK)
                *CR1 |= (1 << 9);   // Bật STOP
            }

            // Chờ RxNE bật lên (đã có dữ liệu trong DR)
            while (((*SR1 >> 6) & 1) == 0);

            // Lấy dữ liệu ra khỏi DR ném vào mảng
            du_lieu[i] = *DR;
        }
    }

    return 0;
}

