/*
 * SPDX-FileCopyrightText: 2016-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "slave.h"
#include "staff.h"


void app_main(void)
{
  // Запуск службы modbus_slave
  slaveTaskStart();
  vTaskDelay(1);

  // Запуск службы staff
//  staffTaskStart();
//  vTaskDelay(1);
}
