/*=====================================================================================
 * Description:
 *    ESP32
 *====================================================================================*/


#include "staff.h"
#include <stdio.h>
#include <stdint.h>
#include "esp_err.h"
#include "uart.h"
//#include "mbcontroller.h"       // for mbcontroller defines and api
//#include "modbus_params.h"      // for modbus parameters structures
#include "esp_log.h"            // for log_write
#include "sdkconfig.h"




static void staff_task(void *arg)
{
  // Set Communication Parameters
  const uart_port_t uart_num = UART_NUM_2;
  uart_config_t uart_config = 
  {
      .baud_rate = 9600;    //115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
      .rx_flow_ctrl_thresh = 122,
  };

  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
  // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
  // Set UART pins(TX: IO16, RX: IO17, RTS: IO19, CTS: no)
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 16, 17, 19, -1));

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
