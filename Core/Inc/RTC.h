#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "stm32f4xx_hal.h"
#include "I2c.h"

#define Adress 0x68

// Struct định nghĩa giữ nguyên
typedef struct {
	uint8_t register_seconds;
	uint8_t register_Minutes;
	uint8_t register_hours;
	uint8_t register_Day;
	uint8_t register_Date;
	uint8_t register_Month;
	uint8_t register_Year;
} RTC_1307_time;

// THÊM CHỮ "extern" VÀ XÓA PHẦN GÁN GIÁ TRỊ {...}
extern const char* mang_thu[];
extern RTC_1307_time timer;

uint8_t Nhi_BCD (uint8_t gia_tri);
uint8_t BCD_Nhi(uint8_t gia_tri);
void cau_hinh(RTC_1307_time* time);
void doc (RTC_1307_time* doc);

#endif /* INC_RTC_H_ */
