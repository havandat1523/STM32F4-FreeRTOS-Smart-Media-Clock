/*
 * Flash.c
 *
 *  Created on: Apr 1, 2026
 *      Author: havandat
 */

#include "Flash.h"
void Erase_Secter (secter_t secter){
	uint32_t*Flash_SR = (uint32_t*)0x40023c0c;
	uint32_t*Flash_CR = (uint32_t*)0x40023c10;

	while (((*Flash_SR>>16)&1)!=0);
	if ((*Flash_CR>>31)==1){
		uint32_t*Flash_KEYR = (uint32_t*)0x40023c04;
		*Flash_KEYR = 0x45670123;
		*Flash_KEYR = 0xCDEF89AB;
	}
	*Flash_CR &=~ 0xf<<3;
	*Flash_CR |= (secter<<3);
	*Flash_CR |= 1<<1;
	*Flash_CR |= 1<<16;
	while (((*Flash_SR>>16)&1)!=0);
	*Flash_CR &=~ 1<<1;
}
void Programing (void* dia_chi,char* data, int size){
	uint32_t*Flash_SR = (uint32_t*)0x40023c0c;
	uint32_t*Flash_CR = (uint32_t*)0x40023c10;
	while (((*Flash_SR>>16)&1)!=0);
	if ((*Flash_CR>>31)==1){
			uint32_t*Flash_KEYR = (uint32_t*)0x40023c04;
			*Flash_KEYR = 0x45670123;
			*Flash_KEYR = 0xCDEF89AB;
		}
	*Flash_CR |= 1<<0;
	char* write_point = dia_chi;
	for (int i=0; i <size;i++){
		*write_point = data[i];
		while (((*Flash_SR>>16)&1)!=0);
		write_point++;
	}
	*Flash_CR &=~ 1<<0;
}
void Read_Flash(void* dia_chi, char* doc, int size) {
    for (int i = 0; i < size; i++) {
        doc[i] = *(char*)dia_chi;         // Bỏ * trước doc[i] ✅
        dia_chi = (char*)dia_chi + 1;
    }
}

