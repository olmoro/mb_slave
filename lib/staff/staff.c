/*=====================================================================================
 * Description:
 *    ESP32
 *====================================================================================*/

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
#include "esp_err.h"




static void staff_task(void *arg)
{
  // Set Communication Parameters
  const uart_port_t uart_num = UART_NUM_2;
  uart_config_t uart_config = 
  {
      .baud_rate = 9600,    //115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
      .rx_flow_ctrl_thresh = 122,
  };

  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
  // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
  // Set UART pins(TX: IO27, RX: IO26, RTS: IO25, CTS: no)
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 27, 26, 25, -1));

  // Setup UART buffered IO with event queue
  const int uart_buffer_size = (1024 * 2);
  QueueHandle_t uart2_queue;
  // Install UART driver using an event queue here
  ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, \
                                          uart_buffer_size, 10, &uart2_queue, 0));







  while (1)
  {
    /* code */
  }
  vTaskDelete(NULL);
}  


void staffTaskStart()
{
  //xTaskCreate(slave_task, "mb_slave", 4096, NULL, 10, NULL);

  static StackType_t stTaskStack[4096];
  static StaticTask_t stTaskBuffer;

  xTaskCreateStatic(staff_task,
                       "staff",
                          4096,
                          NULL,
                            10,
                   stTaskStack,
                 &stTaskBuffer);
}
