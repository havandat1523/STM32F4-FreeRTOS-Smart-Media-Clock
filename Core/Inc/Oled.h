/*
 * Oled.h
 *
 *  Created on: Apr 2, 2026
 *      Author: havandat
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "I2c.h" // Thư viện chứa hàm gui_du_lieu() của bạn

#define AdOled 0x3C

// --- CÁC HÀM ĐIỀU KHIỂN CƠ BẢN ---
void I2C_write_lenh(uint8_t lenh);
void I2C_write_dulieu(uint8_t du_lieu);
void OLED_init(void);
void Clear_all(void);  // Đã sửa tên cho chuẩn
void Clear_page(void);
void pos(int page, int column);

// --- CÁC HÀM HIỂN THỊ FONT TO (12x16) ---
// Dùng 2 Page, thích hợp làm đồng hồ, thông báo lớn
void poit_1(int page, int column, char ky_tu);
void point_many(int page, int column, char* str);

// --- CÁC HÀM HIỂN THỊ FONT NHỎ (5x7) ---
// Dùng 1 Page, thích hợp ghi thông số FFT, ngày tháng
void oled_write_5x7(int page, int column, char ky_tu);
void oled_string_5x7(int page, int column, char* str);

#endif /* INC_OLED_H_ */
