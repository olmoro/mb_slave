/*
   ------------------------------------------------------------------------
   Файл конфигурации проекта
   ------------------------------------------------------------------------
*/

#pragma once

#include <stdint.h>
#include "esp_task.h"

// ------------------------------------------------------------------------
//                              Версии 
// ------------------------------------------------------------------------
#define APP_VERSION "MB_SLAVE 20250327.11"
// 20250327.11:   Проверка                          RAM: 6.4% Flash: 14.9%
// 20250322.10:  Возврат к версии 07                RAM: 6.4% Flash: 14.9%
// 20250320.09:                                     RAM: 6.5% Flash: 14.9%
// 20250320.08:  +mboard                            RAM: 6.4% Flash: 14.9%
// 20250319.07:  main.cpp +extern "C"               RAM: 6.4% Flash: 14.9%
// 20250319.06:  Test RGB                           RAM: 6.4% Flash: 14.9%
// 20250314.05:  Test stuffing                      RAM: 6.4% Flash: 14.7%


// ------------------------------------------------------------------------
//                               GPIO
// ------------------------------------------------------------------------
  // Плата SPN.55
  // Светодиоды
#define RGB_RED_GPIO 4   // Красный, катод на GND (7mA)
#define RGB_GREEN_GPIO 2 // Зелёный, катод на GND (5mA)
#define RGB_BLUE_GPIO 27 // Синий,   катод на GND (4mA)

// Входы
#define CONFIG_GPIO_IR 19 // Вход ИК датчика

 
    // // UART1
    // #define CONFIG_UART1_RXD          25
    // #define CONFIG_UART1_TXD          26
    // #define CONFIG_UART1_RTS          33
  
    // // UART2
    // #define CONFIG_UART2_RXD          21
    // #define CONFIG_UART2_TXD          23
    // #define CONFIG_UART2_RTS          22

// ------------------------------------------------------------------------
//                                MODBUS 
// ------------------------------------------------------------------------

// Test
#define CONFIG_SLAVE_TASK_STACK_SIZE  1024 * 4
#define CONFIG_SLAVE_TASK_PRIORITY    10


