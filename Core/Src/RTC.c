/*
 * RTC.c
 *
 * Created on: Apr 2, 2026
 * Author: havandat
 */
#include "RTC.h"

// --- PHẢI CÓ ĐỊNH NGHĨA BIẾN Ở ĐÂY THÌ MAIN.C MỚI LẤY ĐƯỢC DỮ LIỆU ---
const char* mang_thu[] = {"ERR", "02", "03", "04", "05", "06", "07", "CN"};

RTC_1307_time timer = {
	.register_seconds=10,
	.register_Minutes=37,
	.register_hours =9,
	.register_Day = 7,
	.register_Date =21,
	.register_Month = 3,
	.register_Year = 26,
};
// -------------------------------------------------------------------

uint8_t Nhi_BCD (uint8_t gia_tri){
	return (((gia_tri/10)<<4)|(gia_tri % 10));
}

uint8_t BCD_Nhi(uint8_t gia_tri){
	return (((gia_tri>>4)*10) + (gia_tri&0x0f));
}

void cau_hinh(RTC_1307_time* time){
	uint8_t tempTime[8];
	tempTime[0]=0x00;
	tempTime[1]= Nhi_BCD(time->register_seconds);
	tempTime[2]=Nhi_BCD(time->register_Minutes);
	tempTime[3]=Nhi_BCD(time->register_hours);
	tempTime[4]=Nhi_BCD(time->register_Day);
	tempTime[5]=Nhi_BCD(time->register_Date);
	tempTime[6]=Nhi_BCD(time->register_Month);
	tempTime[7]=Nhi_BCD(time->register_Year);
	gui_du_lieu(0x68, tempTime, 8);
}

void doc (RTC_1307_time* doc){
	uint8_t tempTime[7];
	uint8_t dia_chi = 0x00;
	gui_du_lieu(Adress, &dia_chi, 1);
    nhan_du_lieu(Adress, tempTime, 7);
	doc->register_seconds = BCD_Nhi(tempTime[0]);
	doc->register_Minutes = BCD_Nhi(tempTime[1]);
	doc->register_hours = BCD_Nhi(tempTime[2]);
	doc->register_Day = BCD_Nhi(tempTime[3]);
	doc->register_Date = BCD_Nhi(tempTime[4]);
	doc->register_Month = BCD_Nhi(tempTime[5]);
	doc->register_Year = BCD_Nhi(tempTime[6]);
}
