/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdarg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "I2c.h"
#include "Flash.h"
#include "Oled.h"
#include "RTC.h"
#include "Led.h"
#include "DFPLAYER.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_spi2_rx;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* Definitions for I2S_led */
osThreadId_t I2S_ledHandle;
const osThreadAttr_t I2S_led_attributes = {
  .name = "I2S_led",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* USER CODE BEGIN PV */
osThreadId_t I2C_TimeHandle;
const osThreadAttr_t I2C_Time_atributes = {
    .name = "I2C_Time",
    .stack_size = 256*4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
osThreadId_t I2C_Time_ClockHandle;
const osThreadAttr_t I2C_Time_Clock_atributes = {
    .name = "I2C_Time_Clock",
    .stack_size = 128*4,
    .priority = (osPriority_t)osPriorityNormal,
};

#define AUDIO_BUFFER_SIZE 1024
uint16_t audio_buffer[AUDIO_BUFFER_SIZE];
volatile uint8_t data_ready_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2S2_Init(void);
static void MX_USART2_UART_Init(void);
void I2S_ledTask(void *argument);
void I2C_TimeTask(void *argument);
void I2C_Time_ClockTask(void *argument);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char chuyendoi[20];
char chuyen[25];

void chuyen_doi1(RTC_1307_time gia_tri){
    sprintf(chuyendoi, "%02d:%02d:%02d",
            gia_tri.register_hours,
            gia_tri.register_Minutes,
            gia_tri.register_seconds);
}

typedef enum {
    STATE_DONG_HO = 0,
    STATE_MAIN_MENU,
    STATE_SET_RTC_THU,
    STATE_SET_RTC_GIO,
    STATE_SET_RTC_PHUT,
    STATE_SET_RTC_NGAY,
    STATE_SET_RTC_THANG,
    STATE_SET_RTC_NAM,
    STATE_MENU_CHON_ALM,
    STATE_EDIT_ALM_GIO,
    STATE_EDIT_ALM_PHUT,
    STATE_CONFIRM_SAVE,
	STATE_MUSIC,
	STATE_MUSIC_ALM,
	STATE_MUSIC_SONG,
	STATE_MUSIC_SAVE,
	STATE_MUSIC_OFF
} UI_State_t;

UI_State_t trang_thai_ui = STATE_DONG_HO;
DFPLAYER_Name MP3;

HenGio_t danh_sach_bao_thuc[2];

uint8_t vi_tri_menu_chinh = 0;
uint8_t alm_dang_chon = 0;
uint8_t lua_chon_save = 1;
uint8_t lua_chon_save_bh =1;
uint8_t ghi_0_Flash = 0xFF;

volatile uint8_t flag_MENU = 0;
volatile uint8_t flag_ENTER = 0;
volatile uint8_t co_luu_rtc = 0;
volatile uint8_t co_luu_flash = 0;

volatile uint32_t thoi_gian_chong_nhieu_pin13 = 0;
volatile uint32_t thoi_gian_chong_nhieu_pin8 = 0;
// cho hàm loa
uint8_t Rx_Buffer [10];

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if (GPIO_Pin == GPIO_PIN_13){ // Nút PB13 làm Nút Menu/Thoát
        if ((HAL_GetTick() - thoi_gian_chong_nhieu_pin13) > 300){
            flag_MENU = 1;
            thoi_gian_chong_nhieu_pin13 = HAL_GetTick();
        }
    }
    if (GPIO_Pin == GPIO_PIN_8){  // Nút PA8 làm Nút Enter
        if ((HAL_GetTick() - thoi_gian_chong_nhieu_pin8) > 300){
            flag_ENTER = 1;
            thoi_gian_chong_nhieu_pin8 = HAL_GetTick();
        }
    }
}

//uint8_t Check_Button_Next(void) {
//    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) {
//        osDelay(50);
//        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) {
//            while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) { osDelay(10); }
//            return 1;
//        }
//    }
//    return 0;
//}
uint8_t Check_Button_Next(void) {
    // Biến static để nhớ thời gian giữ nút qua mỗi vòng lặp
    static uint16_t time_hold = 0;

    // Nút đang bị ấn xuống (Mức 0)
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) {
        time_hold++; // Tăng bộ đếm thời gian giữ

        // 1. BẤM NHẢ BÌNH THƯỜNG (Xử lý ngay ở lần quét đầu tiên)
        if (time_hold == 1) {
            osDelay(20); // Chống nhiễu nhẹ
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) {
                return 1; // Bấm phát ăn ngay 1 đơn vị
            } else {
                time_hold = 0; // Nhiễu thì reset
            }
        }
        // 2. NHẤN GIỮ TĂNG TỐC
        // Vòng lặp RTOS của bạn đang trễ 50ms.
        // Đợi time_hold > 10 (tức là 10 * 50ms = 500ms) thì bắt đầu tăng liên tục
        else if (time_hold > 6) {
            time_hold = 3; // Trả bộ đếm về 7 để 3 vòng lặp sau (150ms) nó lại nhảy vào đây tiếp -> Tốc độ tăng vừa phải không bị vụt quá nhanh
            return 1;
        }
    }
    // Người dùng đã nhả nút (Mức 1)
    else {
        time_hold = 0; // Reset bộ đếm về 0 để sẵn sàng cho lần bấm tiếp theo
    }

    return 0; // Không có sự kiện bấm
}
void turn_on_off (int on_off,DFPLAYER_Name* MP3,uint8_t folder, uint32_t num ){

	if (on_off==1){
		DFPLAYER_PlayFileInFolder(MP3, folder, num);
	}
	if (on_off==0){
		DFPLAYER_Stop(MP3);
	}
	vTaskResume(I2S_ledHandle);
	vTaskResume(I2C_Time_ClockHandle);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_I2S2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of I2S_led */
  I2S_ledHandle = osThreadNew(I2S_ledTask, NULL, &I2S_led_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  I2C_TimeHandle      = osThreadNew(I2C_TimeTask,       NULL, &I2C_Time_atributes);
  I2C_Time_ClockHandle= osThreadNew(I2C_Time_ClockTask, NULL, &I2C_Time_Clock_atributes);
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2S2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S2_Init(void)
{

  /* USER CODE BEGIN I2S2_Init 0 */

  /* USER CODE END I2S2_Init 0 */

  /* USER CODE BEGIN I2S2_Init 1 */

  /* USER CODE END I2S2_Init 1 */
  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_MASTER_RX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_16K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S2_Init 2 */

  /* USER CODE END I2S2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 104;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) { if (hi2s->Instance == SPI2) data_ready_flag = 1; }
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)     { if (hi2s->Instance == SPI2) data_ready_flag = 2; }
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)       { if (hi2s->Instance == SPI2) HAL_I2S_Receive_DMA(&hi2s2, audio_buffer, AUDIO_BUFFER_SIZE); }

/* ======================================================================
 * TASK QUẢN LÝ GIAO DIỆN OLED (BẢO VỆ I2C BẰNG vTaskSuspendAll)
 * ====================================================================== */
void I2C_TimeTask(void *argument){
    I2C_Init();
    MX_USART2_UART_Init();
  // cau_hinh(&timer);
    OLED_init();
    Clear_all();
	DFPLAYER_Init(&MP3, &huart2);

    static uint8_t giay_cu = 255;
    static uint8_t vi_tri_menu_Music =0;
    static uint8_t chon_nhac =0;
    static uint32_t tong_bai_hat =5;
    static uint8_t am_thanh=5;
    static uint8_t am_thanh_co =0;



    Read_Flash((void*)FLASH_HEN_GIO_ADDR, (char*)danh_sach_bao_thuc, sizeof(danh_sach_bao_thuc));
    for (int i = 0; i < 2; i++) {
        if (danh_sach_bao_thuc[i].da_dat != 0xFF) {
            if (danh_sach_bao_thuc[i].gio > 23 || danh_sach_bao_thuc[i].phut > 59) {
                danh_sach_bao_thuc[i].da_dat = 0xFF;
            }
        }
    }

    uint8_t old_ui_state = 255;

    while(1){
        // 1. CẬP NHẬT DỮ LIỆU I2C AN TOÀN
    	uint8_t btn_next = Check_Button_Next();
        if (co_luu_rtc == 1) {
            vTaskSuspendAll(); cau_hinh(&timer); xTaskResumeAll();
            co_luu_rtc = 0;
        }

        if (trang_thai_ui == STATE_DONG_HO || trang_thai_ui == STATE_MAIN_MENU || trang_thai_ui >= STATE_MENU_CHON_ALM ) {
            vTaskSuspendAll(); doc(&timer); xTaskResumeAll();

        }
        if (am_thanh_co==1){
        	osDelay(10);
        	vTaskSuspendAll();
        	Clear_page();
        	xTaskResumeAll();
        }

        //chuyen_doi1(timer);

        // 2. VẼ MÀN HÌNH CHUNG LÊN OLED AN TOÀN
//        vTaskSuspendAll();
//        point_many(0, 15, chuyendoi);
//        sprintf(chuyen, "%s %02d.%02d.%02d", mang_thu[timer.register_Day], timer.register_Date, timer.register_Month, timer.register_Year);
//
//        oled_string_5x7(3, 20, chuyen);
//        xTaskResumeAll();
       // if (timer.register_seconds != giay_cu) {

                    // 1. Chuyển đổi dữ liệu (Chạy cực nhanh trên RAM, không cần khóa)
                    chuyen_doi1(timer);
                    sprintf(chuyen, "%s %02d.%02d.%02d", mang_thu[timer.register_Day], timer.register_Date, timer.register_Month, timer.register_Year);

                    vTaskSuspendAll();
                    point_many(0, 15, chuyendoi);
                    xTaskResumeAll();  // Mở khóa ra ngay lập tức

                    osDelay(2);

                    // 3. Khóa RTOS lại để truyền dòng 2 (Ngày tháng năm)
                    vTaskSuspendAll();
                    oled_string_5x7(3, 20, chuyen);
                    xTaskResumeAll(); // Lại mở khóa ra

                    // 4. Cập nhật lại mốc thời gian
                    giay_cu = timer.register_seconds;
         //       }

        if (trang_thai_ui != old_ui_state) {
            vTaskSuspendAll(); Clear_page(); xTaskResumeAll();
            old_ui_state = trang_thai_ui;
        }

        // 3. ĐỌC NÚT BẤM (KHÔNG ĐƯỢC CHỨA TRONG vTaskSuspendAll)
        //uint8_t btn_next = Check_Button_Next();

        // 4. MÁY TRẠNG THÁI (VẼ UI VÀ XỬ LÝ LOGIC)
        switch(trang_thai_ui) {
            case STATE_DONG_HO:
                // --- PHẦN VẼ UI ---
                if (btn_next) {
                	am_thanh++;
                	if(am_thanh >29 ) am_thanh=0;
                	DFPLAYER_SetVolume(&MP3,am_thanh);
                	vTaskSuspendAll();
                	sprintf(chuyen, "LOA  %02d", am_thanh);
                	point_many(4, 12, chuyen);
                	//am_thanh_co=1;
                	xTaskResumeAll();
                }
                osDelay(10);
                vTaskSuspendAll();
                if (ghi_0_Flash != 0xFF) { // khi cờ báo thức được set
                    if ((HAL_GetTick() / 300) % 2 == 0){
                    	Clear_page();
                    	oled_string_5x7(5, 25, "DEN GIO !!!");
                    	//oled_string_5x7(7, 25, "BAM NUT TAT");
                    } else {
                    	Clear_page();
                    	oled_string_5x7(5, 25, "           ");
                    	//oled_string_5x7(7, 25, "           ");
                    }
                } else {
                    if (danh_sach_bao_thuc[0].da_dat == 1 || danh_sach_bao_thuc[1].da_dat == 1) {
                    	Clear_page();
                        oled_string_5x7(7, 30, "HEN GIO BAT");
                    } else {
                    	Clear_page();
                        oled_string_5x7(7, 30, "HEN GIO TAT");
                    }
                }
                xTaskResumeAll();

                // --- PHẦN LOGIC --- khi cờ ghi_0_Flash dược set ( có báo thức) Khi nhấn nút thì tắt còi
                // rồi ghi vào Flash.da_dat =0
                if (ghi_0_Flash != 0xFF) {
                    if (flag_MENU || flag_ENTER) {
                        //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0); // Tắt còi
                    	DFPLAYER_Stop(&MP3);
                        if (ghi_0_Flash < 2) {
                            danh_sach_bao_thuc[ghi_0_Flash].da_dat = 0;
                            co_luu_flash = 1;
                        }
                        ghi_0_Flash = 0xFF;
                        flag_MENU = 0; flag_ENTER = 0;
                        vTaskSuspendAll(); Clear_page(); xTaskResumeAll();
                    }
                } else {
                    if (flag_MENU || flag_ENTER) {
                        flag_MENU = 0; flag_ENTER = 0;
                        trang_thai_ui = STATE_MAIN_MENU;
                        vi_tri_menu_chinh = 0;
                    }
                }
                break;

            case STATE_MAIN_MENU:
                vTaskSuspendAll();
                oled_string_5x7(4, 0, "--- MAIN MENU ---");
                if (vi_tri_menu_chinh == 0) {
                    oled_string_5x7(5, 5, "> 1. SET CLOCK");
                    oled_string_5x7(6, 5, "  2. SET HEN GIO");
                    oled_string_5x7(7, 5, "  3. SET MUSIC");
                } else if  (vi_tri_menu_chinh ==1) {
                    oled_string_5x7(5, 5, "  1. SET CLOCK");
                    oled_string_5x7(6, 5, "> 2. SET HEN GIO");
                    oled_string_5x7(7, 5, "  3. SET MUSIC");
                } else {
                    oled_string_5x7(5, 5, "  1. SET CLOCK");
                    oled_string_5x7(6, 5, "  2. SET HEN GIO");
                    oled_string_5x7(7, 5, "> 3. SET MUSIC");
                }
                xTaskResumeAll();

                if (btn_next) {
                	vi_tri_menu_chinh ++;
                	if (vi_tri_menu_chinh>2)vi_tri_menu_chinh=0;
                 }
                if (flag_ENTER) {
                    flag_ENTER = 0;
                    if (vi_tri_menu_chinh == 0) trang_thai_ui = STATE_SET_RTC_THU;
                    else if (vi_tri_menu_chinh==1) trang_thai_ui = STATE_MENU_CHON_ALM;
                    else {
                    	vi_tri_menu_Music=0;
                    	trang_thai_ui = STATE_MUSIC;

                    }
                }
                if (flag_MENU) {
                    flag_MENU = 0;
                    trang_thai_ui = STATE_DONG_HO;
                }
                break;

            case STATE_SET_RTC_THU:
                vTaskSuspendAll();
                oled_string_5x7(4, 0, "-- SET CLOCK --");
                sprintf(chuyen, "> DAY : %s", mang_thu[timer.register_Day]);
                oled_string_5x7(6, 5, chuyen);
                xTaskResumeAll();

                if (btn_next) { timer.register_Day++; if(timer.register_Day > 7) timer.register_Day = 1; }
                if (flag_ENTER) { flag_ENTER = 0; trang_thai_ui = STATE_SET_RTC_GIO; }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_MAIN_MENU; }
                break;

            case STATE_SET_RTC_GIO:
                vTaskSuspendAll();
                oled_string_5x7(4, 0, "-- SET CLOCK --");
                sprintf(chuyen, "> HOUR: %02d", timer.register_hours);
                oled_string_5x7(6, 5, chuyen);
                xTaskResumeAll();

                if (btn_next) { timer.register_hours++; if(timer.register_hours > 23) timer.register_hours = 0; }
                if (flag_ENTER) { flag_ENTER = 0; trang_thai_ui = STATE_SET_RTC_PHUT; }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_SET_RTC_THU; }
                break;

            case STATE_SET_RTC_PHUT:
                vTaskSuspendAll();
                oled_string_5x7(4, 0, "-- SET CLOCK --");
                sprintf(chuyen, "> MIN : %02d", timer.register_Minutes);
                oled_string_5x7(6, 5, chuyen);
                xTaskResumeAll();

                if (btn_next) { timer.register_Minutes++; if(timer.register_Minutes > 59) timer.register_Minutes = 0; }
                if (flag_ENTER) { flag_ENTER = 0; trang_thai_ui = STATE_SET_RTC_NGAY; }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_SET_RTC_GIO; }
                break;

            case STATE_SET_RTC_NGAY:
                vTaskSuspendAll();
                oled_string_5x7(4, 0, "-- SET CLOCK --");
                sprintf(chuyen, "> DATE: %02d", timer.register_Date);
                oled_string_5x7(6, 5, chuyen);
                xTaskResumeAll();

                if (btn_next) { timer.register_Date++; if(timer.register_Date > 31) timer.register_Date = 1; }
                if (flag_ENTER) { flag_ENTER = 0; trang_thai_ui = STATE_SET_RTC_THANG; }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_SET_RTC_PHUT; }
                break;

            case STATE_SET_RTC_THANG:
                vTaskSuspendAll();
                oled_string_5x7(4, 0, "-- SET CLOCK --");
                sprintf(chuyen, "> MNTH: %02d", timer.register_Month);
                oled_string_5x7(6, 5, chuyen);
                xTaskResumeAll();

                if (btn_next) { timer.register_Month++; if(timer.register_Month > 12) timer.register_Month = 1; }
                if (flag_ENTER) { flag_ENTER = 0; trang_thai_ui = STATE_SET_RTC_NAM; }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_SET_RTC_NGAY; }
                break;

            case STATE_SET_RTC_NAM:
                vTaskSuspendAll();
                oled_string_5x7(4, 0, "-- SET CLOCK --");
                sprintf(chuyen, "> YEAR: 20%02d", timer.register_Year);
                oled_string_5x7(6, 5, chuyen);
                xTaskResumeAll();

                if (btn_next) { timer.register_Year++; if(timer.register_Year > 99) timer.register_Year = 0; }
                if (flag_ENTER) {
                    flag_ENTER = 0; co_luu_rtc = 1; trang_thai_ui = STATE_DONG_HO;
                }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_SET_RTC_THANG; }
                break;

            case STATE_MENU_CHON_ALM:
                vTaskSuspendAll();
                oled_string_5x7(4, 0, "--- HEN GIO ---");
                for (int i = 0; i < 2; i++) {
                    if (danh_sach_bao_thuc[i].da_dat == 0xFF) {
                        sprintf(chuyen, "%c HG %d: --:--", (alm_dang_chon == i) ? '>' : ' ', i + 1);
                    } else {
                        sprintf(chuyen, "%c HG% d: %02d:%02d [%s]",
                                (alm_dang_chon == i) ? '>' : ' ', i + 1,
                                danh_sach_bao_thuc[i].gio, danh_sach_bao_thuc[i].phut,
                                (danh_sach_bao_thuc[i].da_dat == 1) ? "ON " : "OFF");
                    }
                    oled_string_5x7(5 + i, 0, chuyen);
                }
                xTaskResumeAll();

                if (btn_next) { alm_dang_chon = 1 - alm_dang_chon; }
                if (flag_ENTER) {
                    flag_ENTER = 0; trang_thai_ui = STATE_EDIT_ALM_GIO;
                    if (danh_sach_bao_thuc[alm_dang_chon].da_dat == 0xFF) {
                        danh_sach_bao_thuc[alm_dang_chon].gio = 0;
                        danh_sach_bao_thuc[alm_dang_chon].phut = 0;
                    }
                }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_MAIN_MENU; }
                break;

            case STATE_EDIT_ALM_GIO:
                vTaskSuspendAll();
                sprintf(chuyen, "-- EDIT HG %d --", alm_dang_chon + 1);
                oled_string_5x7(4, 0, chuyen);
                sprintf(chuyen, "> HOUR: %02d", danh_sach_bao_thuc[alm_dang_chon].gio);
                oled_string_5x7(6, 5, chuyen);
                xTaskResumeAll();

                if (btn_next) {
                    danh_sach_bao_thuc[alm_dang_chon].gio++;
                    if (danh_sach_bao_thuc[alm_dang_chon].gio > 23) danh_sach_bao_thuc[alm_dang_chon].gio = 0;
                }
                if (flag_ENTER) { flag_ENTER = 0; trang_thai_ui = STATE_EDIT_ALM_PHUT; }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_MENU_CHON_ALM; }
                break;

            case STATE_EDIT_ALM_PHUT:
                vTaskSuspendAll();
                sprintf(chuyen, "-- EDIT HG %d --", alm_dang_chon + 1);
                oled_string_5x7(4, 0, chuyen);
                sprintf(chuyen, "> MIN : %02d", danh_sach_bao_thuc[alm_dang_chon].phut);
                oled_string_5x7(6, 5, chuyen);
                xTaskResumeAll();

                if (btn_next) {
                    danh_sach_bao_thuc[alm_dang_chon].phut++;
                    if (danh_sach_bao_thuc[alm_dang_chon].phut > 59) danh_sach_bao_thuc[alm_dang_chon].phut = 0;
                }
                if (flag_ENTER) { flag_ENTER = 0; trang_thai_ui = STATE_CONFIRM_SAVE; lua_chon_save = 1; }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_EDIT_ALM_GIO; }
                break;

            case STATE_CONFIRM_SAVE:
                vTaskSuspendAll();
            	sprintf(chuyen, "-- SAVE HG %d ? --", alm_dang_chon + 1);
                oled_string_5x7(4, 0, chuyen);
                if (lua_chon_save == 1) oled_string_5x7(6, 10, "> SAVE      DELETE ");
                else                    oled_string_5x7(6, 10, "  SAVE    > DELETE ");
                xTaskResumeAll();

                if (btn_next) { lua_chon_save = 1 - lua_chon_save; }
                if (flag_ENTER) {
                    flag_ENTER = 0;
                    if (lua_chon_save == 1) danh_sach_bao_thuc[alm_dang_chon].da_dat = 1;
                    else danh_sach_bao_thuc[alm_dang_chon].da_dat = 0xFF;
                    co_luu_flash = 1;
                    trang_thai_ui = STATE_MENU_CHON_ALM;
                }
                if (flag_MENU) { flag_MENU = 0; trang_thai_ui = STATE_EDIT_ALM_PHUT; }
                break;
            case STATE_MUSIC:
            	vTaskSuspendAll();

            	oled_string_5x7(4, 0, "--- MUSIC ---");
            	if (vi_tri_menu_Music==0){
            		oled_string_5x7(5, 0, "> 1. NHAC THUONG");
            		oled_string_5x7(6, 0, "  2. NHAC BAO THUC");
            		oled_string_5x7(7, 0, "  3. TAT NHAC");
            	}
            	else if (vi_tri_menu_Music==1){
            		oled_string_5x7(5, 0, "  1. NHAC THUONG");
            		oled_string_5x7(6, 0, "> 2. NHAC BAO THUC");
            		oled_string_5x7(7, 0, "  3. TAT NHAC");
            	}
            	else {
            		oled_string_5x7(5, 0, "  1. NHAC THUONG");
            		oled_string_5x7(6, 0, "  2. NHAC BAO THUC");
            		oled_string_5x7(7, 0, "> 3. TAT NHAC");
            	}
            	xTaskResumeAll();
			if (btn_next) {
				vi_tri_menu_Music++;
				if (vi_tri_menu_Music > 2)
					vi_tri_menu_Music = 0;
			}
            	if (flag_ENTER){
            		flag_ENTER=0;
            		Clear_page();
            		if (vi_tri_menu_Music==0) trang_thai_ui = STATE_MUSIC_SONG;
            		else if(vi_tri_menu_Music==1) trang_thai_ui = STATE_MUSIC_ALM;
            		else trang_thai_ui= STATE_MUSIC_OFF;
            	}
            	if (flag_MENU){
            		flag_MENU =0;
            		Clear_page();
            		trang_thai_ui = STATE_MAIN_MENU;
            	}
            	break;
            case STATE_MUSIC_SONG:
            	vTaskSuspendAll();
            	oled_string_5x7(4, 0, "--- CHON NHAC ---");
            	sprintf(chuyen, " BAI SO : %02d",chon_nhac );
            	oled_string_5x7(5,0,chuyen);
            	xTaskResumeAll();
            	osDelay(10);


			if (btn_next) {
				chon_nhac++;

				if (chon_nhac> tong_bai_hat)
					chon_nhac = 0;
				turn_on_off(1, &MP3, 02, chon_nhac);
			}
            	if (flag_ENTER){
            		flag_ENTER=0;
            		Clear_page();
            		vTaskSuspendAll();
            		oled_string_5x7(4, 0, "---DANG PHAT---");
            		xTaskResumeAll();
            		trang_thai_ui = STATE_MUSIC;
            	}
            	if (flag_MENU){
            		flag_MENU =0;
            		Clear_page();
            		trang_thai_ui = STATE_MUSIC;
            	}
            	break;
            case STATE_MUSIC_ALM:
            	vTaskSuspendAll();
            	oled_string_5x7(4, 0, "--- CHON NHAC ---");
            	sprintf(chuyen, " BAI SO : %02d",chon_nhac );
            	oled_string_5x7(5,0,chuyen);
            	xTaskResumeAll();
            	osDelay(10);
    			if (btn_next) {
    				chon_nhac++;
    				if (chon_nhac> tong_bai_hat)
    					chon_nhac = 0;
    				turn_on_off(1, &MP3, 02, chon_nhac);
    			}
            	if (flag_ENTER){
				flag_ENTER = 0;
				Clear_page();
				lua_chon_save_bh = 1;
				trang_thai_ui=STATE_MUSIC_SAVE;
			}
			if (flag_MENU) {
				flag_MENU = 0;
				Clear_page();
				trang_thai_ui = STATE_MUSIC;
			}
			break;
            case STATE_MUSIC_SAVE:
			vTaskSuspendAll();
			sprintf(chuyen, "-- SAVE BH %d ? --", alm_dang_chon + 1);
			oled_string_5x7(4, 0, chuyen);
			if (lua_chon_save == 1)
				oled_string_5x7(6, 10, "> SAVE      DELETE ");
			else
				oled_string_5x7(6, 10, "  SAVE    > DELETE ");
			xTaskResumeAll();

			if (btn_next) {
				lua_chon_save = 1 - lua_chon_save;
			}
			if (flag_ENTER) {
				flag_ENTER = 0;
				if (lua_chon_save == 1){
					danh_sach_bao_thuc[0].bai_hat = chon_nhac;
				    danh_sach_bao_thuc[1].bai_hat = chon_nhac;
				}
				co_luu_flash = 1;
				DFPLAYER_Stop(&MP3);
				trang_thai_ui = STATE_MUSIC;
			}
			if (flag_MENU) {
				flag_MENU = 0;
				DFPLAYER_Stop(&MP3);
				trang_thai_ui = STATE_MUSIC_ALM;
			}
			break;
            case STATE_MUSIC_OFF:
            	Clear_page();
            	DFPLAYER_Stop(&MP3);
            	trang_thai_ui = STATE_MUSIC;
            	break;
        }
        osDelay(50);
    }
}

/* ======================================================================
 * TASK KIỂM TRA & LƯU FLASH (BACKGROUND TASK)
 * ====================================================================== */
/*void I2C_Time_ClockTask(void *argument)
{
    while(1) {
        if (co_luu_flash == 1) {
            Erase_Secter(secter_3);
            osDelay(20);
            Programing((void*)FLASH_HEN_GIO_ADDR, (char*)danh_sach_bao_thuc, sizeof(danh_sach_bao_thuc));
            co_luu_flash = 0;
        }

        for (int i = 0; i < 2; i++) {
            if (danh_sach_bao_thuc[i].da_dat == 1 && trang_thai_ui == STATE_DONG_HO) {
                if (timer.register_hours   == danh_sach_bao_thuc[i].gio  &&
                    timer.register_Minutes == danh_sach_bao_thuc[i].phut &&
                    timer.register_seconds == 0)
                {
                   // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1);
                	turn_on_off(1, &MP3,02,danh_sach_bao_thuc[0].bai_hat);

                    ghi_0_Flash = i;
                }
            }
        }
        osDelay(100);
    }
}*/
void I2C_Time_ClockTask(void *argument)
{
    while(1) {
        if (co_luu_flash == 1) {
            Erase_Secter(secter_3);
            osDelay(20);
            Programing((void*)FLASH_HEN_GIO_ADDR, (char*)danh_sach_bao_thuc, sizeof(danh_sach_bao_thuc));
            co_luu_flash = 0;
        }

        for (int i = 0; i < 2; i++) {
            if (danh_sach_bao_thuc[i].da_dat == 1 && trang_thai_ui == STATE_DONG_HO) {

                // THÊM ĐIỀU KIỆN: ghi_0_Flash == 0xFF
                if (timer.register_hours   == danh_sach_bao_thuc[i].gio  &&
                    timer.register_Minutes == danh_sach_bao_thuc[i].phut &&
                    timer.register_seconds == 0  &&
                    ghi_0_Flash == 0xFF)
                {
                   // SỬA [0] THÀNH [i] CHO ĐÚNG NHẠC CỦA BÁO THỨC ĐÓ
                   turn_on_off(1, &MP3, 02, danh_sach_bao_thuc[i].bai_hat);
                   ghi_0_Flash = i; // Đánh dấu đã kêu, vòng lặp osDelay tiếp theo sẽ không vào đây nữa
                }
            }
        }
        osDelay(100);
    }
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_I2S_ledTask */
/**
  * @brief  Function implementing the I2S_led thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_I2S_ledTask */
void I2S_ledTask(void *argument)
{
  /* USER CODE BEGIN 5 */
    HAL_I2S_Receive_DMA(&hi2s2, (uint16_t*)audio_buffer, AUDIO_BUFFER_SIZE);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    for(;;)
    {
        if (data_ready_flag == 1 || data_ready_flag == 2)
        {
            int start_idx = (data_ready_flag == 1) ? 0 : (AUDIO_BUFFER_SIZE / 2);
            int end_idx = start_idx + (AUDIO_BUFFER_SIZE / 2);
            data_ready_flag = 0;

            int16_t max_val = -32768;
            int16_t min_val = 32767;

            for (int i = start_idx; i < end_idx; i += 2)
            {
                int16_t sample = (int16_t)audio_buffer[i];
                if (sample > max_val) max_val = sample;
                if (sample < min_val) min_val = sample;
            }

            uint32_t amplitude = max_val - min_val;
            int so_led_muc_tieu = 0;

            if (amplitude > 7000)       so_led_muc_tieu = 8;
            else if (amplitude > 5000)   so_led_muc_tieu = 7;
            else if (amplitude > 2000)   so_led_muc_tieu = 6;
            else if (amplitude > 1500)   so_led_muc_tieu = 5;
            else if (amplitude > 900)   so_led_muc_tieu = 4;
            else if (amplitude > 500)    so_led_muc_tieu = 3;
            else if (amplitude > 400)    so_led_muc_tieu = 2;
            else if (amplitude > 300)    so_led_muc_tieu = 1;
            else                         so_led_muc_tieu = 0;

            gui(so_led_muc_tieu);
        }
        osDelay(5); // ĐÃ THAY ĐỔI TỪ 1 THÀNH 20 ĐỂ CỨU I2C
    }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
