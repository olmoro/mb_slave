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
#define APP_VERSION "MB_SLAVE 20250310.05"
// 20250314.05:  Test stuffing                RAM: 6.4% Flash: 14.7%
// 20250310.04:  Test spUART TX-OK                 RAM: 6.4% Flash: 14.7%
// 20250310.03:  Test spUART RX-OK    TX-NO        RAM: 6.4% Flash: 14.7%
// 20250310.02:  Драйвер (объявления)              RAM: 5.3% Flash: 14.7%
// 20250310.01:  Создание статической задачи       RAM: 5.3% Flash: 14.7%
// 20250310.00:            OK                      RAM: 5.3% Flash: 


// ------------------------------------------------------------------------
//                               GPIO
// ------------------------------------------------------------------------
  // Плата SPN.55
  // Светодиоды
  #define CONFIG_GPIO_LED_RED        4    // Красный, катод на GND (7mA)
  #define CONFIG_GPIO_LED_GREEN      2    // Зелёный, катод на GND (5mA)
  #define CONFIG_GPIO_LED_BLUE      27    // Синий,   катод на GND (4mA)

  // Входы
  #define CONFIG_GPIO_IR            19    // Вход ИК датчика


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


#define CONFIG_SLAVE_TASK_STACK_SIZE  1024 * 4
#define CONFIG_SLAVE_TASK_PRIORITY    10
