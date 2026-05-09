/*
 * Led.c
 *
 * Created on: Apr 2, 2026
 * Author: havandat
 */

#include "Led.h"

// THÊM DÒNG NÀY ĐỂ BÁO CHO TRÌNH BIÊN DỊCH BIẾT HTIM2 ĐÃ ĐƯỢC KHAI BÁO Ở NƠI KHÁC
extern TIM_HandleTypeDef htim2;

void set1_0(int hig_low){
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

    if (hig_low==1){
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 68); // ~66% chu kỳ cho mức 1
    }
    else if (hig_low==0){
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 34); // ~33% chu kỳ cho mức 0
    }

    // Với 84MHz, CPU chạy cực nhanh nên sẽ đợi ở đây chuẩn xác từng micro-giây
    while (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE) == RESET) {}
}

void gui_1_led(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
    for (int i = 23; i >= 0; i--) {
        if (((color >> i) & 0x01) == 0) {
            set1_0(0);
        } else {
            set1_0(1);
        }
    }
}

void chot_du_lieu_led() {
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    // Vòng lặp xả chốt, chạy cực nhanh ở 84MHz
    for (volatile int j = 0; j < 5000; j++) {}
}

void tat_den(){
    __disable_irq(); // Khóa ngắt
    for (int i=0; i<8; i++){
        gui_1_led(0, 0, 0);
    }
    chot_du_lieu_led();
    __enable_irq();
}

void gui(int a){
    __disable_irq(); // Khóa ngắt
    for (int i = 0; i < a; i++){
        gui_1_led(0, 50, 0); // Sáng màu Xanh Lá
    }
    for (int j = a; j < 8; j++){
        gui_1_led(0, 0, 0);  // Tắt
    }
    chot_du_lieu_led();
    __enable_irq();
}
