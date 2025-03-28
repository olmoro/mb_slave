

/* Uart Events Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "staff.h"
//#include "wake.h"


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


/* Для структурирования сообщений используются управляющие символы */
#define SOH   0x01 // начало заголовка
#define ISI   0x1F // указатель кода функции FNC
#define STX   0x02 // начало тела сообщения
#define ETX   0x03 // конец тела сообщения

/* Использованы следующие обозначения:
DAD  байт адреса приёмника,
SAD  байт адреса источника,
FNC  байт кода функции,
CRC1, CRC2  циклические контрольные коды
*/

/*
 - заголовок    | SOH | DAD | SAD | ISI | FNC | DataHead |
 или безадресный заголовок  | SOH | ISI | FNC | DataHead |
 - данные                                                | STX | DataSet | ETX | 
 - контрольная информация                                                      | CRC1 | CRC2 | 
 */


#define DLE 0x10        // символ-префикс


static const char *TAG = "STAFF";








bool staffProcess(const uint8_t *src, uint8_t *dest, size_t src_len, size_t dest_max_len, size_t *dest_actual_len)
{
    size_t j = 0;
    for (size_t i = 0; i < src_len; i++) {
        // Определяем, сколько байт нужно записать
        size_t required = (src[i] == 0x01) ? 2 : 1;
        
        // Проверяем, достаточно ли места в целевом буфере
        if (j + required > dest_max_len) {
            *dest_actual_len = j; // Текущая длина на момент ошибки
            return false;
        }
         // Вставляем байты согласно условию
         if (src[i] == 0x01) {
            dest[j++] = 0x10;
            dest[j++] = 0x01;
        } else {
            dest[j++] = src[i];
        }
    }
    *dest_actual_len = j;
    return true;
}



/* Пример использования
void app_main() {
    uint8_t src[] = {0x00, 0x01, 0x02, 0x01};
    size_t src_len = sizeof(src);
    uint8_t dest[10]; // Буфер с запасом
    size_t dest_len;

    if (process_buffers(src, dest, src_len, sizeof(dest), &dest_len)) {
        // Успешная обработка
        ESP_LOGI("TAG", "Processed data length: %d", dest_len);
        // Далее можно использовать данные в dest
    } else {
        ESP_LOGE("TAG", "Destination buffer overflow!");
    }
}
*/







// // Note: Some pins on target chip cannot be assigned for UART communication.
// // Please refer to documentation for selected board and target to configure pins using Kconfig.
// #define ECHO_TEST_TXD           (CONFIG_SP_UART_TXD)
// #define ECHO_TEST_RXD           (CONFIG_SP_UART_RXD)

// // RTS for RS485 Half-Duplex Mode manages DE/~RE
// #define ECHO_TEST_RTS           (CONFIG_SP_UART_RTS)

// // CTS is not used in RS485 Half-Duplex Mode
// #define ECHO_TEST_CTS           (UART_PIN_NO_CHANGE)

// #define BUF_SIZE                (127)
// #define BAUD_RATE               (CONFIG_SP_UART_BAUD_RATE)

// // Read packet timeout
// #define PACKET_READ_TICS        (100 / portTICK_PERIOD_MS)
// #define ECHO_TASK_STACK_SIZE    (CONFIG_SP_TASK_STACK_SIZE)
// #define ECHO_TASK_PRIO          (10)
// #define ECHO_UART_PORT          (CONFIG_SP_UART_PORT_NUM)

// // Timeout threshold for UART = number of symbols (~10 tics) with unchanged state on receive pin
// #define ECHO_READ_TOUT          (3) // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks

// static void echo_send(const int port, const char* str, uint8_t length)
// {
//     if (uart_write_bytes(port, str, length) != length) {
//         ESP_LOGE(TAG, "Send data critical failure.");
//         // add your code to handle sending failure here
//         abort();
//     }
// }

// // An example of echo test with hardware flow control on UART
// static void echo_task(void *arg)
// {
//     const int uart_num = ECHO_UART_PORT;
//     uart_config_t uart_config = {
//         .baud_rate = BAUD_RATE,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//         .rx_flow_ctrl_thresh = 122,
//         .source_clk = UART_SCLK_DEFAULT,
//     };

//     // Set UART log level
//     esp_log_level_set(TAG, ESP_LOG_INFO);

//     ESP_LOGI(TAG, "Start RS485 application test and configure UART.");

//     // Install UART driver (we don't need an event queue here)
//     // In this example we don't even use a buffer for sending data.
//     ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0));

//     // Configure UART parameters
//     ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

//     ESP_LOGI(TAG, "UART set pins, mode and install driver.");

//     // Set UART pins as per KConfig settings
//     ESP_ERROR_CHECK(uart_set_pin(uart_num, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

//     // Set RS485 half duplex mode
//     ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));

//     // Set read timeout of UART TOUT feature
//     ESP_ERROR_CHECK(uart_set_rx_timeout(uart_num, ECHO_READ_TOUT));

//     // Allocate buffers for UART
//     uint8_t* data = (uint8_t*) malloc(BUF_SIZE);

//     ESP_LOGI(TAG, "UART start receive loop.\r");
//     echo_send(uart_num, "Start RS485 UART test.\r\n", 24);

//     while (1) {
//         //Read data from UART
//         int len = uart_read_bytes(uart_num, data, BUF_SIZE, PACKET_READ_TICS);

//         //Write data back to UART
//         if (len > 0) {
//             echo_send(uart_num, "\r\n", 2);
//             char prefix[] = "RS485 Received: [";
//             echo_send(uart_num, prefix, (sizeof(prefix) - 1));
//             ESP_LOGI(TAG, "Received %u bytes:", len);
//             printf("[ ");
//             for (int i = 0; i < len; i++) {
//                 printf("0x%.2X ", (uint8_t)data[i]);
//                 echo_send(uart_num, (const char*)&data[i], 1);
//                 // Add a Newline character if you get a return character from paste (Paste tests multibyte receipt/buffer)
//                 if (data[i] == '\r') {
//                     echo_send(uart_num, "\n", 1);
//                 }
//             }
//             printf("] \n");
//             echo_send(uart_num, "]\r\n", 3);
//         } else {
//             // Echo a "." to show we are alive while we wait for input
//             echo_send(uart_num, ".", 1);
//             ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 10));
//         }
//     }
//     vTaskDelete(NULL);
// }

// //void app_main(void)
// void staffTaskStart()
// {
//     //A uart read/write example without event queue;
//     xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, ECHO_TASK_PRIO, NULL);
// }




// // void staffTaskStart()
// // {
// //   //xTaskCreate(slave_task, "mb_slave", 4096, NULL, 10, NULL);

// //   static StackType_t stTaskStack[4096];
// //   static StaticTask_t stTaskBuffer;

// //   xTaskCreateStatic(staff_task,
// //                        "staff",
// //                           4096,
// //                           NULL,
// //                             10,
// //                    stTaskStack,
// //                  &stTaskBuffer);
// // }
