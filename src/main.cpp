/*
 *
 *
 *
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "project_config.h"
#include "mboard.h"
#include "slave.h"
#include "sp_uart.h"
#include "staff.h"

static MBoard Board;

extern "C" void app_main(void)
{
    /* Инициализируем логи и выводим версию прошивки */
    // rlog_empty();
    // disbleEspIdfLogs();
    // rloga_i("Firmware initialization, version %s", APP_VERSION);
    vTaskDelay(1);

    /* Регистрируем обработчики перезагрузки (всего можно добавить до 5 обработчиков,
       1 - системный (отладка), остается 4 для приложений) */
    // espRegisterSystemShutdownHandler(); // #1
    vTaskDelay(1);

    /* Инициализация периферии esp32 (светодиодов и др.) */
    Board.boardInit(); // ledSysOn(false);
    vTaskDelay(1);

    /* Запуск службы modbus_slave */
    slaveTaskStart();
    vTaskDelay(1);

    /* Запуск службы проверки sp */
    spTaskStart();
    vTaskDelay(1);

    /* Проверка RGB светодиода */
    while (1)
    {
        Board.ledRedToggle();
        Board.ledGreenToggle();
        Board.ledBlueToggle();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
