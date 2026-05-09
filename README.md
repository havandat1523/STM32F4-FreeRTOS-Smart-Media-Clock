# STM32F4-FreeRTOS-Smart-Media-Clock
A RTC alarm clock built on STM32F411 using FreeRTOS. Features include an audio-reactive LED display, MP3 music playback, and fully customizable alarm profiles (tones & volume).
# STM32F4 FreeRTOS Smart Media Clock

![Project Status](https://img.shields.io/badge/Status-Completed-success)
![Platform](https://img.shields.io/badge/Platform-STM32-blue)
![OS](https://img.shields.io/badge/OS-FreeRTOS-orange)

[cite_start]An advanced, feature-rich embedded clock system powered by the STM32F411CEU6 microcontroller and FreeRTOS[cite: 201, 488]. This project goes beyond a standard digital clock by integrating real-time audio processing for LED effects, media playback, and smart alarm management.

---

## 🌟 Key Features

* [cite_start]**Real-Time Accuracy:** Displays highly accurate time and date using the DS1307 RTC module[cite: 221, 222].
* [cite_start]**Audio-Reactive LED Effects:** Uses an INMP441 I2S MEMS microphone to capture real-time audio and synchronizes it with WS2812 RGB LEDs for dynamic visual effects[cite: 232, 261].
* [cite_start]**Smart Media Alarm:** Features customizable alarm settings with high-quality MP3 playback via the DFPlayer Mini module[cite: 243, 607].
* [cite_start]**Flash Memory Storage:** Alarm configurations are safely stored in the MCU's internal Flash memory, ensuring data is retained even after power loss[cite: 607].
* [cite_start]**Intuitive UI:** Clean and responsive user interface displayed on a 0.96-inch SSD1306 OLED screen[cite: 249, 252].
* [cite_start]**Multitasking:** Efficient task management and low-latency audio processing handled by FreeRTOS[cite: 543].

---

## 🛠️ Hardware Components

| Component | Description | Protocol/Interface |
| :--- | :--- | :--- |
| **STM32F411CEU6** | [cite_start]Main Microcontroller (ARM Cortex-M4, 100MHz) [cite: 202, 210] | N/A |
| **DS1307** | [cite_start]Real-Time Clock (RTC) with backup battery [cite: 221, 227] | [cite_start]I2C [cite: 226] |
| **INMP441** | [cite_start]Digital MEMS Microphone with built-in ADC [cite: 232, 233] | [cite_start]I2S + DMA [cite: 236, 544] |
| **DFPlayer Mini** | [cite_start]MP3 Audio Decoder & Amplifier [cite: 243] | [cite_start]UART [cite: 244] |
| **SSD1306** | [cite_start]0.96" OLED Display (128x64) [cite: 249] | [cite_start]I2C [cite: 250] |
| **WS2812** | [cite_start]Smart RGB LED Strip [cite: 259] | [cite_start]PWM (Timer) [cite: 260, 434] |
| **Mechanical Switches**| [cite_start]User input for menu navigation and settings [cite: 267] | [cite_start]EXTI [cite: 548] |

---

## 🧠 System Architecture (FreeRTOS)

[cite_start]The system relies on FreeRTOS (CMSIS_V2) to manage multiple tasks concurrently without blocking the audio processing pipeline[cite: 488, 649]. 

1. **`I2S_ledTask` (High Priority):** Triggered by an I2S DMA interrupt. [cite_start]It reads digital audio samples from the INMP441, processes the amplitude, and updates the WS2812 LEDs to create synchronized lighting effects[cite: 544, 545].
2. **`I2C_TimeTask` (Normal Priority):** Manages the user interface. [cite_start]It reads data from the RTC, updates the OLED display, and handles the state machine for menu navigation via EXTI button inputs[cite: 548, 549, 588].
3. **`I2C_Time_ClockTask` (Low Priority):** Continuously checks if the current RTC time matches the user-defined alarm. [cite_start]It triggers the DFPlayer and saves any user configuration changes to the Flash memory[cite: 605, 606, 607].

---

## 🚀 Getting Started

### Prerequisites
* [cite_start][STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (Used for development and compilation) [cite: 540]
* ST-Link V2 (For programming and debugging)

### Installation & Flashing
1. Clone this repository to your local machine.
2. Open the project folder in **STM32CubeIDE**.
3. Re-generate the initialization code via the `.ioc` file if necessary.
4. Build the project and flash it to the STM32F411CEU6 using your ST-Link.

---

## 📸 Demo

*(Lưu ý: Bạn hãy quay một đoạn GIF hoặc video ngắn cảnh LED nháy theo nhạc và up file đó vào thư mục dự án, sau đó thay link ảnh vào đây nhé)*

![System Demo](link_den_anh_hoac_gif_cua_ban.gif)

---

## 👥 Authors

[cite_start]This project was developed as a coursework assignment for Embedded Systems at the **Academy of Cryptography Techniques (KMA)**[cite: 1, 8].

* [cite_start]**Hà Văn Đạt** (DT060209) [cite: 12]
* **Dương Hải Đăng** (DT060206) [cite: 13]
* [cite_start]**Nguyễn Mạnh Lân** (DT060231) [cite: 14]

Instructor: **ThS. [cite_start]Lê Thị Hồng Vân** [cite: 11]

---
*If you find this project interesting or helpful, feel free to give it a ⭐!*
