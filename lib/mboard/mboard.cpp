/*
  Методы работы с аппаратными ресурсами платы
  pcb: spn.55
  17.03.2025
*/

#include "mboard.h"
#include "project_config.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"

static const char *logTAG = "MBOARD";

gpio_num_t _rgb_red_gpio = static_cast<gpio_num_t>(RGB_RED_GPIO);
gpio_num_t _rgb_green_gpio = static_cast<gpio_num_t>(RGB_GREEN_GPIO);
gpio_num_t _rgb_blue_gpio = static_cast<gpio_num_t>(RGB_BLUE_GPIO);

void boardInit()
{
    /* Configure IO Pad as General Purpose IO,
     * so that it can be connected to internal Matrix,
     * then combined with one or more peripheral signals. */
    // gpio_pad_select_gpio(_rgb_red_gpio);
    // gpio_pad_select_gpio(_rgb_green_gpio);
    // gpio_pad_select_gpio(_rgb_blue_gpio);

    // esp_rom_gpio_pad_select_gpio(_rgb_red_gpio);
    // esp_rom_gpio_pad_select_gpio(_rgb_green_gpio);
    // esp_rom_gpio_pad_select_gpio(_rgb_blue_gpio);

    /* Инициализация GPIO (push/pull output) */
    gpio_reset_pin(_rgb_red_gpio);
    gpio_reset_pin(_rgb_green_gpio);
    gpio_reset_pin(_rgb_blue_gpio);
    gpio_set_direction(_rgb_red_gpio, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(_rgb_green_gpio, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(_rgb_blue_gpio, GPIO_MODE_INPUT_OUTPUT);

    /* Установка начального состояния (выключено) */
    gpio_set_level(_rgb_red_gpio, 0);
    gpio_set_level(_rgb_green_gpio, 0);
    gpio_set_level(_rgb_blue_gpio, 0);
}

void ledsOn()
{
    gpio_set_level(_rgb_red_gpio, 1);
    gpio_set_level(_rgb_green_gpio, 1);
    gpio_set_level(_rgb_blue_gpio, 1);
}
void ledsRed()
{
    gpio_set_level(_rgb_red_gpio, 1);
    gpio_set_level(_rgb_green_gpio, 0);
    gpio_set_level(_rgb_blue_gpio, 0);
}
void ledsGreen()
{
    gpio_set_level(_rgb_red_gpio, 0);
    gpio_set_level(_rgb_green_gpio, 1);
    gpio_set_level(_rgb_blue_gpio, 0);
}
void ledsBlue()
{
    gpio_set_level(_rgb_red_gpio, 0);
    gpio_set_level(_rgb_green_gpio, 0);
    gpio_set_level(_rgb_blue_gpio, 1);
}

void ledsOff()
{
    gpio_set_level(_rgb_red_gpio, 0);
    gpio_set_level(_rgb_green_gpio, 0);
    gpio_set_level(_rgb_blue_gpio, 0);
}

void ledRedToggle()
{
    if (gpio_get_level(_rgb_red_gpio))
        gpio_set_level(_rgb_red_gpio, 0);
    else
        gpio_set_level(_rgb_red_gpio, 1);
}

void ledGreenToggle()
{
    if (gpio_get_level(_rgb_green_gpio))
        gpio_set_level(_rgb_green_gpio, 0);
    else
        gpio_set_level(_rgb_green_gpio, 1);
}

void ledBlueToggle()
{
    if (gpio_get_level(_rgb_blue_gpio))
        gpio_set_level(_rgb_blue_gpio, 0);
    else
        gpio_set_level(_rgb_blue_gpio, 1);
}


// Управление цветом светодиода

// void mboard::ledsOn()
// {
// 	ledROn();
// 	ledGOn();
// 	ledBOn();
// } // Белый
//   void mboard::ledsOff()
//   {
// 	  ledROff();
// 	  ledGOff();
// 	  ledBOff();
//   } // Погасить все
//   void mboard::ledsRed()
//   {
// 	  ledROn();
// 	  ledGOff();
// 	  ledBOff();
//   } // Красный
//   void mboard::ledsGreen()
//   {
// 	  ledROff();
// 	  ledGOn();
// 	  ledBOff();
//   } // Зеленый
//   void mboard::ledsBlue()
//   {
// 	  ledROff();
// 	  ledGOff();
// 	  ledBOn();
//   } // Синий
//   void MBomboardard::ledsYellow()
//   {
// 	  ledROn();
// 	  ledGOn();
// 	  ledBOff();
//   } // Желтый
//   void mboard::ledsCyan()
//   {
// 	  ledROff();
// 	  ledGOn();
// 	  ledBOn();
//   } // Голубой

//   // Инициализация аппаратных ресурсов ESP32
//   void mboard::initEsp32() 
//   {
//   // Светодиоды
//   pinMode( MPins::led_r_pin, OUTPUT_OPEN_DRAIN );
//   pinMode( MPins::led_g_pin, OUTPUT_OPEN_DRAIN );
//   pinMode( MPins::led_b_pin, OUTPUT_OPEN_DRAIN );

//   ledROff();
//   ledGOff();
//   ledBOff();
//   }

// class MBoard
// {
// private:
// 	gpio_num_t _rgb_red_gpio = static_cast<gpio_num_t>(RGB_RED_GPIO);
// 	gpio_num_t _rgb_green_gpio = static_cast<gpio_num_t>(RGB_GREEN_GPIO);
// 	gpio_num_t _rgb_blue_gpio = static_cast<gpio_num_t>(RGB_BLUE_GPIO);

// public:
// 	void initRGB();
// 	void setColor(uint8_t red, uint8_t green, uint8_t blue);
// 	void turnOffRGB();

// 	void ledsOn();
// 	void ledsCyan();
// 	void ledsGreen();
// 	void ledsRed();
// 	void ledsBlue();
// 	void ledsOff();
// };





// // Конфигурация LEDC (ШИМ)
// #define LEDC_TIMER      LEDC_TIMER_0
// #define LEDC_MODE       LEDC_HIGH_SPEED_MODE
// #define LEDC_CH_RED     LEDC_CHANNEL_0
// #define LEDC_CH_GREEN   LEDC_CHANNEL_1
// #define LEDC_CH_BLUE    LEDC_CHANNEL_2
// #define LEDC_RESOLUTION LEDC_TIMER_8_BIT
// #define LEDC_FREQUENCY  5000






// MBoard::MBoard()
// {
// 	initRGB();
// }

// // Инициализация GPIO и LEDC
// extern "C" void MBoard::initRGB()
// {
//     // // Настройка таймера LEDC
//     // ledc_timer_config_t ledc_timer = {
//     //     .speed_mode = LEDC_MODE,
//     //     .duty_resolution = LEDC_RESOLUTION,
//     //     .timer_num = LEDC_TIMER,
//     //     .freq_hz = LEDC_FREQUENCY,
//     //     .clk_cfg = LEDC_AUTO_CLK};
//     // ledc_timer_config(&ledc_timer);

//     // // Настройка каналов LEDC
//     // ledc_channel_config_t ledc_channel[3] = {
//     //     {.gpio_num = RGB_RED_GPIO,
//     //      .speed_mode = LEDC_MODE,
//     //      .channel = LEDC_CH_RED,
//     //      .timer_sel = LEDC_TIMER,
//     //      .duty = 0,
//     //      .hpoint = 0},
//     //     {.gpio_num = RGB_GREEN_GPIO,
//     //      .speed_mode = LEDC_MODE,
//     //      .channel = LEDC_CH_GREEN,
//     //      .timer_sel = LEDC_TIMER,
//     //      .duty = 0,
//     //      .hpoint = 0},
//     //     {.gpio_num = RGB_BLUE_GPIO,
//     //      .speed_mode = LEDC_MODE,
//     //      .channel = LEDC_CH_BLUE,
//     //      .timer_sel = LEDC_TIMER,
//     //      .duty = 0,
//     //      .hpoint = 0}};

//     // // Настройка каналов RGB
//     // for (int i = 0; i < 3; i++)
//     // {
//     //     ledc_channel_config(&ledc_channel[i]);
//     // }
// }

// // Установка цвета (0-255)
// extern "C" void MBoard::setColor(uint8_t red, uint8_t green, uint8_t blue)
// {
//     // ledc_set_duty(LEDC_MODE, LEDC_CH_RED, red);
//     // ledc_update_duty(LEDC_MODE, LEDC_CH_RED);

//     // ledc_set_duty(LEDC_MODE, LEDC_CH_GREEN, green);
//     // ledc_update_duty(LEDC_MODE, LEDC_CH_GREEN);

//     // ledc_set_duty(LEDC_MODE, LEDC_CH_BLUE, blue);
//     // ledc_update_duty(LEDC_MODE, LEDC_CH_BLUE);
// }

// // Выключение светодиода
// void mboard::turnOffRGB()
// {
//     set_color(0, 0, 0);
// }


// void mboard::ledsOn()
// {
//     gpio_set_level(_rgb_red_gpio, 1);
//     gpio_set_level(_rgb_green_gpio, 1);
//     gpio_set_level(_rgb_blue_gpio, 1);
// }

// void mboard::ledsCyan()
// {

// }

