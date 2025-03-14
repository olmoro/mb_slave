/*
   https://esp32tutorials.com/esp32-uart-tutorial-esp-idf/
   ESP32 UART Communication using ESP-IDF

   UART Echo Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   The RS485 half duplex does not work in your example because you configured it incorrectly.
   Please refer to Overview of RS485 specific communication options:
   https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/peripherals/uart.html#overview-of-rs485-specific-communication-options
*/

#include <stdio.h>
#include "project_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"

/**
 * This is an example which echos any data it receives on configured UART back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 * 
 * DeepSeek: "ESP-IDF ESP32 настроить UART для RS485 полудуплекс"
 * С заданием не справился. В "примерах" код для полудуплекса на RS485 нерабочий
 * при том, что для modbus на RS485 выполняется.
 * 
 * Здесь только тест
 */


 #define SP_PORT_NUM (CONFIG_SP_UART_PORT_NUM)          // =2 Number of UART port used for connection
 #define SP_UART_BAUD_RATE (CONFIG_SP_UART_BAUD_RATE)   // =9600 The communication speed of the UART
 #define SP_TASK_STACK_SIZE (CONFIG_SP_TASK_STACK_SIZE) // =3072
 #define SP_UART_RXD (CONFIG_SP_UART_RXD)               // =21
 #define SP_UART_TXD (CONFIG_SP_UART_TXD)               // =23
 #define SP_UART_RTS (CONFIG_SP_UART_RTS)               // =22

#define BUF_SIZE (1024) // размер буфера

static const char *TAG = "sp_UART_TEST";


// static send_rs485_data(const char* data) {
//     uart_write_bytes(SP_PORT_NUM, data, strlen(data));
// }

// static receive_rs485_data() {
//     uint8_t data[BUF_SIZE];
//     int len = uart_read_bytes(SP_PORT_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
//     if (len > 0) {
//         // Обработка полученных данных
//     }
// }

static void echo_task(void *arg)
{
    /* Configure parameters of an UART driver, communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = SP_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  // RTS для управления направлением DE/RE !!
        .rx_flow_ctrl_thresh = 122,
    };

    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(SP_PORT_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0));
    
    // // Выделить буферы для данных UART
    // uint8_t[BUF_SIZE] = {0};

    ESP_ERROR_CHECK(uart_set_pin(SP_PORT_NUM, SP_UART_TXD, SP_UART_RXD, SP_UART_RTS, 32));   // IO32 свободен (трюк)

    ESP_ERROR_CHECK(uart_set_mode(SP_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX)); // activate RS485 half duplex in the driver


    // your other code continue here ....

    // Set UART pin numbers
    // ESP_ERROR_CHECK(uart_set_pin(SP_PORT_NUM, CONFIG_SP_UART_TXD,
    //                             CONFIG_SP_UART_RXD, CONFIG_SP_UART_RTS,
    //                             UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_param_config(SP_PORT_NUM, &uart_config));  

 
    // Опционально: установка задержек для RTS (в тактах)
    //ESP_ERROR_CHECK(uart_set_rs485_rts_delay(SP_PORT_NUM, 0, 0));


    ESP_LOGI(TAG, "SP_uart initialized.");


// Configure a temporary buffer for the incoming data
uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    /* Внутри цикла while мы будем считывать данные с UART2, вызывая функцию uart_read_bytes(),
    * и сохранять их в переменной int «len». Затем мы будем записывать эти данные (len) обратно
    * в UART с помощью функции uart_write_bytes().
    */
    while (1)
    {
    // Read data from the UART

    int len = uart_read_bytes(SP_PORT_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    // Write data back to the UART
    
    uart_write_bytes(SP_PORT_NUM, (const char *)data, len);

    }
      // Destroy of Modbus controller on alarm
    ESP_LOGI(TAG,"SP_uart destroyed.");
    vTaskDelay(100);
    ESP_ERROR_CHECK(uart_driver_delete(SP_PORT_NUM));
    vTaskDelete(NULL);
}

/* 
 * void app_main(void)
 * {
 *     xTaskCreate(echo_task, "uart_echo_task", SP_TASK_STACK_SIZE, NULL, 10, NULL);
 * }
 */

// то же статически
void spTaskStart()
{
    static StackType_t spTaskStack[SP_TASK_STACK_SIZE];
    static StaticTask_t spTaskBuffer;

    TaskHandle_t task_handle = xTaskCreateStaticPinnedToCore(
        echo_task,                      // Функция задачи
        "echo_task",                    // Имя задачи
        SP_TASK_STACK_SIZE,             // Размер стека
        NULL,                           // Параметры
        CONFIG_SLAVE_TASK_PRIORITY - 1, // Приоритет
        spTaskStack,                    // Буфер стека
        &spTaskBuffer,                  // Буфер задачи
        tskNO_AFFINITY);                // Без привязки к ядру или CONFIG_TASK_CORE_SLAVE

    if (!task_handle)
    {
        ESP_LOGE(TAG, "Failed to create sp_UART task");
        return;
    }

    ESP_LOGI(TAG, "sp_UART task created successfully");
}
