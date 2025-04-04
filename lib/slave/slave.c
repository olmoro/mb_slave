/*=====================================================================================
 * Description:
 *   FreeModbus Slave ESP32
 * https://github.com/espressif/esp-idf/blob/master/examples/protocols/modbus/serial/mb_slave/sdkconfig.defaults
 *====================================================================================*/

#include "slave.h"
#include "board.h"

#include "project_config.h"
#include <stdio.h>
#include <stdint.h>
#include "esp_err.h"
#include "mbcontroller.h"               // for mbcontroller defines and api
#include "modbus_params.h"              // for modbus parameters structures
#include "esp_modbus_slave.h"
#include "esp_log.h"                    // for log_write
#include "sdkconfig.h"
// #include "soc/soc.h"                // for BITN definitions
// #include "freertos/FreeRTOS.h"      // for task creation and queues access
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "freertos/event_groups.h"  // for event groups
// #include "esp_modbus_common.h"      // for common types


static const char *TAG = "MODBUS_SLAVE";

static portMUX_TYPE param_lock = portMUX_INITIALIZER_UNLOCKED;

// // Set register values into known state
// static void setup_reg_data(void)
// {
//     // Define initial state of parameters
//     discrete_reg_params.discrete_input0 = 1;
//     discrete_reg_params.discrete_input1 = 0;
//     discrete_reg_params.discrete_input2 = 1;
//     discrete_reg_params.discrete_input3 = 0;
//     discrete_reg_params.discrete_input4 = 1;
//     discrete_reg_params.discrete_input5 = 0;
//     discrete_reg_params.discrete_input6 = 1;
//     discrete_reg_params.discrete_input7 = 0;

//     holding_reg_params.holding_data0 = 1.34;
//     holding_reg_params.holding_data1 = 2.56;
//     holding_reg_params.holding_data2 = 3.78;
//     holding_reg_params.holding_data3 = 4.90;

//     holding_reg_params.holding_data4 = 5.67;
//     holding_reg_params.holding_data5 = 6.78;
//     holding_reg_params.holding_data6 = 7.79;
//     holding_reg_params.holding_data7 = 8.80;

//     coil_reg_params.coils_port0 = 0x55;
//     coil_reg_params.coils_port1 = 0xAA;

//     input_reg_params.input_data0 = 1.12;
//     input_reg_params.input_data1 = 2.34;
//     input_reg_params.input_data2 = 3.56;
//     input_reg_params.input_data3 = 4.78;

//     input_reg_params.input_data4 = 1.12;
//     input_reg_params.input_data5 = 2.34;
//     input_reg_params.input_data6 = 3.56;
//     input_reg_params.input_data7 = 4.78;
// }


/* An example application of Modbus slave. It is based on freemodbus stack.
   See deviceparams.h file for more information about assigned Modbus parameters.
   These parameters can be accessed from main application and also can be changed
   by external Modbus master host. */
   static void slave_task(void *arg)
   {
       mb_param_info_t reg_info;               // keeps the Modbus registers access information
       mb_communication_info_t comm_info;      // Modbus communication parameters
       mb_register_area_descriptor_t reg_area; // Modbus register area descriptor structure
   
       // Set UART log level
       esp_log_level_set(TAG, ESP_LOG_INFO);
       void *mbc_slave_handler = NULL;
   
       /* Инициализация контроллера Modbus */
       ESP_ERROR_CHECK(mbc_slave_init(MB_PORT_SERIAL_SLAVE, &mbc_slave_handler));
   
       /* Настройка режима связи и запуска стека */
       #if CONFIG_MB_COMM_MODE_ASCII
           comm_info.mode = MB_MODE_ASCII;
       #elif CONFIG_MB_COMM_MODE_RTU
           comm_info.mode = MB_MODE_RTU;
       #endif
       comm_info.slave_addr = MB_SLAVE_ADDR;
       comm_info.port = MB_PORT_NUM;
       comm_info.baudrate = MB_DEV_BAUD_RATE;
       comm_info.parity = MB_PARITY_NONE;
       ESP_ERROR_CHECK(mbc_slave_setup((void *)&comm_info));
   
       /* Приведенный ниже код инициализирует дескрипторы областей регистров Modbus
       для регистров Modbus, входных регистров, катушек и дискретных входов
       Инициализация должна выполняться для каждой поддерживаемой области
       регистров Modbus в соответствии с картой регистров.
       Если внешний мастер пытается получить доступ к регистру в области, которая
       не была инициализирована вызовом API mbc_slave_set_descriptor(), то
       стек Modbus отправит ответ об ошибке для этой области регистров. */
       
       reg_area.type = MB_PARAM_HOLDING;                             // Set type of register area
       reg_area.start_offset = MB_REG_HOLDING_START_AREA0;           // Offset of register area in Modbus protocol
       reg_area.address = (void *)&holding_reg_params.holding_data0; // Set pointer to storage instance
       
       /* Установите размер экземпляра хранилища регистров = 150 хранилищ регистров */
       reg_area.size = (size_t)(HOLD_OFFSET(holding_data4) - HOLD_OFFSET(test_regs));
       ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
   
       reg_area.type = MB_PARAM_HOLDING;                             // Set type of register area
       reg_area.start_offset = MB_REG_HOLDING_START_AREA1;           // Offset of register area in Modbus protocol
       reg_area.address = (void *)&holding_reg_params.holding_data4; // Set pointer to storage instance
       reg_area.size = sizeof(float) << 2;                           // Set the size of register storage instance
       ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
   
       /* Инициализация области входных регистров */
       reg_area.type = MB_PARAM_INPUT;
       reg_area.start_offset = MB_REG_INPUT_START_AREA0;
       reg_area.address = (void *)&input_reg_params.input_data0;
       reg_area.size = sizeof(float) << 2;
       ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
   
       reg_area.type = MB_PARAM_INPUT;
       reg_area.start_offset = MB_REG_INPUT_START_AREA1;
       reg_area.address = (void *)&input_reg_params.input_data4;
       reg_area.size = sizeof(float) << 2;
       ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
   
       /* Инициализация области регистрации катушек */
       reg_area.type = MB_PARAM_COIL;
       reg_area.start_offset = MB_REG_COILS_START;
       reg_area.address = (void *)&coil_reg_params;
       reg_area.size = sizeof(coil_reg_params);
       ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
   
       /* Инициализация области регистра дискретных входов */
       reg_area.type = MB_PARAM_DISCRETE;
       reg_area.start_offset = MB_REG_DISCRETE_INPUT_START;
       reg_area.address = (void *)&discrete_reg_params;
       reg_area.size = sizeof(discrete_reg_params);
       ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
   
       // /* Установите значения в известное состояние */
       // setup_reg_data(); 
   
       /* Запуск контроллера modbus и стека */
       ESP_ERROR_CHECK(mbc_slave_start());
      
       /* Установите GPIO UART */
       ESP_ERROR_CHECK(uart_set_pin(
           MB_PORT_NUM,
           CONFIG_MB_UART_TXD,
           CONFIG_MB_UART_RXD,
           CONFIG_MB_UART_RTS,
           UART_PIN_NO_CHANGE));
   
       /* Установите режим драйвера UART на полудуплексный */
       ESP_ERROR_CHECK(uart_set_mode(
           MB_PORT_NUM, 
           UART_MODE_RS485_HALF_DUPLEX));
   
       ESP_LOGI(TAG, "Modbus slave stack initialized.");
       ESP_LOGI(TAG, "Start modbus test...");
   
       
   
   
       /* Цикл, описанный ниже, завершится, когда параметр holdingRegParams.dataChan0
       увеличится на единицу в каждом цикле доступа и достигнет значения CHAN_DATA_MAX_VAL. */
       for (; holding_reg_params.holding_data0 < MB_CHAN_DATA_MAX_VAL;)
       {
           /* Проверьте наличие событий чтения/записи Modbus master для определенных событий */
           (void)mbc_slave_check_event(MB_READ_WRITE_MASK);
           ESP_ERROR_CHECK_WITHOUT_ABORT(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
           const char *rw_str = (reg_info.type & MB_READ_MASK) ? "READ" : "WRITE";
   
           /* Фильтруйте события и обрабатывайте их соответствующим образом */
           if (reg_info.type & (MB_EVENT_HOLDING_REG_WR | MB_EVENT_HOLDING_REG_RD))
           {
               ledRedToggle(); // Чтение или запись регистров хранения только
   
               /* Получение информации о параметрах из очереди параметров */
               ESP_LOGI(TAG, "HOLDING %s (%" PRIu32 " us), ADDR:%u, TYPE:%u, INST_ADDR:0x%" PRIx32 ", SIZE:%u",
                        rw_str,
                        reg_info.time_stamp,
                        (unsigned)reg_info.mb_offset,
                        (unsigned)reg_info.type,
                        (uint32_t)reg_info.address,
                        (unsigned)reg_info.size);
               if (reg_info.address == (uint8_t *)&holding_reg_params.holding_data0)
               {
                   portENTER_CRITICAL(&param_lock);
                   holding_reg_params.holding_data0 += MB_CHAN_DATA_OFFSET;
   
   
   
                   if (holding_reg_params.holding_data0 >= (MB_CHAN_DATA_MAX_VAL - MB_CHAN_DATA_OFFSET))
                   {
                       coil_reg_params.coils_port1 = 0xFF;
                   }
                   portEXIT_CRITICAL(&param_lock);
               }
           }
           else if (reg_info.type & MB_EVENT_INPUT_REG_RD)
           {
               ESP_LOGI(TAG, "INPUT READ (%" PRIu32 " us), ADDR:%u, TYPE:%u, INST_ADDR:0x%" PRIx32 ", SIZE:%u",
                        reg_info.time_stamp,
                        (unsigned)reg_info.mb_offset,
                        (unsigned)reg_info.type,
                        (uint32_t)reg_info.address,
                        (unsigned)reg_info.size);
           }
           else if (reg_info.type & MB_EVENT_DISCRETE_RD)
           {
               ESP_LOGI(TAG, "DISCRETE READ (%" PRIu32 " us): ADDR:%u, TYPE:%u, INST_ADDR:0x%" PRIx32 ", SIZE:%u",
                        reg_info.time_stamp,
                        (unsigned)reg_info.mb_offset,
                        (unsigned)reg_info.type,
                        (uint32_t)reg_info.address,
                        (unsigned)reg_info.size);
           }
           else if (reg_info.type & (MB_EVENT_COILS_RD | MB_EVENT_COILS_WR))
           {
               ESP_LOGI(TAG, "COILS %s (%" PRIu32 " us), ADDR:%u, TYPE:%u, INST_ADDR:0x%" PRIx32 ", SIZE:%u",
                        rw_str,
                        reg_info.time_stamp,
                        (unsigned)reg_info.mb_offset,
                        (unsigned)reg_info.type,
                        (uint32_t)reg_info.address,
                        (unsigned)reg_info.size);
               if (coil_reg_params.coils_port1 == 0xFF)
                   break;
           }
          // ledsOff();
       }
   
       /* Аварийное отключение контроллера Modbus */
       ESP_LOGI(TAG, "Modbus controller destroyed.");
       vTaskDelay(100);
       ESP_ERROR_CHECK(mbc_slave_destroy());
       vTaskDelete(NULL);
   }
   
   void slaveTaskStart()
   {
       static StackType_t slTaskStack[CONFIG_SLAVE_TASK_STACK_SIZE];
       static StaticTask_t slTaskBuffer;
   
       // Создание статической задачи
       TaskHandle_t task_handle = xTaskCreateStaticPinnedToCore(
           slave_task,                   // Функция задачи
           "mb_slave",                   // Имя задачи
           CONFIG_SLAVE_TASK_STACK_SIZE, // Размер стека
           NULL,                         // Параметры
           CONFIG_SLAVE_TASK_PRIORITY,   // Приоритет
           slTaskStack,                  // Буфер стека
           &slTaskBuffer,                // Буфер задачи
           tskNO_AFFINITY);              // Без привязки к ядру или CONFIG_TASK_CORE_SLAVE
   
       if (!task_handle)
       {
           ESP_LOGE(TAG, "Failed to create Slave Modbus task");
           return;
       }
   
       ESP_LOGI(TAG, "Slave Modbus task created successfully");
   }
   



// =============================================================


// /* An example application of Modbus slave. It is based on freemodbus stack.
//    See deviceparams.h file for more information about assigned Modbus parameters.
//    These parameters can be accessed from main application and also can be changed
//    by external Modbus master host. */
// static void slave_task(void *arg)
// {
//     mb_param_info_t reg_info;               // keeps the Modbus registers access information
//     mb_communication_info_t comm_info;      // Modbus communication parameters
//     mb_register_area_descriptor_t reg_area; // Modbus register area descriptor structure

//     // Set UART log level
//     esp_log_level_set(TAG, ESP_LOG_INFO);
//     void *mbc_slave_handler = NULL;

//     ESP_ERROR_CHECK(mbc_slave_init(MB_PORT_SERIAL_SLAVE, &mbc_slave_handler)); // Initialization of Modbus controller

// // Setup communication mode and start stack
// #if CONFIG_MB_COMM_MODE_ASCII
//     comm_info.mode = MB_MODE_ASCII;
// #elif CONFIG_MB_COMM_MODE_RTU
//     comm_info.mode = MB_MODE_RTU;
// #endif
//     comm_info.slave_addr = MB_SLAVE_ADDR;
//     comm_info.port = MB_PORT_NUM;
//     comm_info.baudrate = MB_DEV_SPEED;
//     comm_info.parity = MB_PARITY_NONE;
//     ESP_ERROR_CHECK(mbc_slave_setup((void *)&comm_info));

//     /* The code below initializes Modbus register area descriptors
//      for Modbus Holding Registers, Input Registers, Coils and Discrete Inputs
//      Initialization should be done for each supported Modbus register area according to register map.
//      When external master trying to access the register in the area that is not initialized
//      by mbc_slave_set_descriptor() API call then Modbus stack
//      will send exception response for this register area. */
//     reg_area.type = MB_PARAM_HOLDING;                             // Set type of register area
//     reg_area.start_offset = MB_REG_HOLDING_START_AREA0;           // Offset of register area in Modbus protocol
//     reg_area.address = (void *)&holding_reg_params.holding_data0; // Set pointer to storage instance
//     // Set the size of register storage instance = 150 holding registers
//     reg_area.size = (size_t)(HOLD_OFFSET(holding_data4) - HOLD_OFFSET(test_regs));
//     ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
//     reg_area.type = MB_PARAM_HOLDING;                             // Set type of register area
//     reg_area.start_offset = MB_REG_HOLDING_START_AREA1;           // Offset of register area in Modbus protocol
//     reg_area.address = (void *)&holding_reg_params.holding_data4; // Set pointer to storage instance
//     reg_area.size = sizeof(float) << 2;                           // Set the size of register storage instance
//     ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

//     // Initialization of Input Registers area
//     reg_area.type = MB_PARAM_INPUT;
//     reg_area.start_offset = MB_REG_INPUT_START_AREA0;
//     reg_area.address = (void *)&input_reg_params.input_data0;
//     reg_area.size = sizeof(float) << 2;
//     ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
//     reg_area.type = MB_PARAM_INPUT;
//     reg_area.start_offset = MB_REG_INPUT_START_AREA1;
//     reg_area.address = (void *)&input_reg_params.input_data4;
//     reg_area.size = sizeof(float) << 2;
//     ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

//     // Initialization of Coils register area
//     reg_area.type = MB_PARAM_COIL;
//     reg_area.start_offset = MB_REG_COILS_START;
//     reg_area.address = (void *)&coil_reg_params;
//     reg_area.size = sizeof(coil_reg_params);
//     ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

//     // Initialization of Discrete Inputs register area
//     reg_area.type = MB_PARAM_DISCRETE;
//     reg_area.start_offset = MB_REG_DISCRETE_INPUT_START;
//     reg_area.address = (void *)&discrete_reg_params;
//     reg_area.size = sizeof(discrete_reg_params);
//     ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

//     setup_reg_data(); // Set values into known state

//     // Starts of modbus controller and stack
//     ESP_ERROR_CHECK(mbc_slave_start());

//     // Set UART pin numbers
//     ESP_ERROR_CHECK(uart_set_pin(MB_PORT_NUM, CONFIG_MB_UART_TXD,
//                                  CONFIG_MB_UART_RXD, CONFIG_MB_UART_RTS,
//                                  UART_PIN_NO_CHANGE));

//     // Set UART driver mode to Half Duplex
//     ESP_ERROR_CHECK(uart_set_mode(MB_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX));

//     ESP_LOGI(TAG, "Modbus slave stack initialized.");
//     ESP_LOGI(TAG, "Start modbus test...");

//     /* The cycle below will be terminated when parameter holdingRegParams.dataChan0
//      incremented each access cycle reaches the CHAN_DATA_MAX_VAL value. */
//     for (; holding_reg_params.holding_data0 < MB_CHAN_DATA_MAX_VAL;)
//     {
//         // Check for read/write events of Modbus master for certain events
//         (void)mbc_slave_check_event(MB_READ_WRITE_MASK);
//         ESP_ERROR_CHECK_WITHOUT_ABORT(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
//         const char *rw_str = (reg_info.type & MB_READ_MASK) ? "READ" : "WRITE";
//         // Filter events and process them accordingly
//         if (reg_info.type & (MB_EVENT_HOLDING_REG_WR | MB_EVENT_HOLDING_REG_RD))
//         {
//             // Get parameter information from parameter queue
//             ESP_LOGI(TAG, "HOLDING %s (%" PRIu32 " us), ADDR:%u, TYPE:%u, INST_ADDR:0x%" PRIx32 ", SIZE:%u",
//                      rw_str,
//                      reg_info.time_stamp,
//                      (unsigned)reg_info.mb_offset,
//                      (unsigned)reg_info.type,
//                      (uint32_t)reg_info.address,
//                      (unsigned)reg_info.size);
//             if (reg_info.address == (uint8_t *)&holding_reg_params.holding_data0)
//             {
//                 portENTER_CRITICAL(&param_lock);
//                 holding_reg_params.holding_data0 += MB_CHAN_DATA_OFFSET;
//                 if (holding_reg_params.holding_data0 >= (MB_CHAN_DATA_MAX_VAL - MB_CHAN_DATA_OFFSET))
//                 {
//                     coil_reg_params.coils_port1 = 0xFF;
//                 }
//                 portEXIT_CRITICAL(&param_lock);
//             }
//         }
//         else if (reg_info.type & MB_EVENT_INPUT_REG_RD)
//         {
//             ESP_LOGI(TAG, "INPUT READ (%" PRIu32 " us), ADDR:%u, TYPE:%u, INST_ADDR:0x%" PRIx32 ", SIZE:%u",
//                      reg_info.time_stamp,
//                      (unsigned)reg_info.mb_offset,
//                      (unsigned)reg_info.type,
//                      (uint32_t)reg_info.address,
//                      (unsigned)reg_info.size);
//         }
//         else if (reg_info.type & MB_EVENT_DISCRETE_RD)
//         {
//             ESP_LOGI(TAG, "DISCRETE READ (%" PRIu32 " us): ADDR:%u, TYPE:%u, INST_ADDR:0x%" PRIx32 ", SIZE:%u",
//                      reg_info.time_stamp,
//                      (unsigned)reg_info.mb_offset,
//                      (unsigned)reg_info.type,
//                      (uint32_t)reg_info.address,
//                      (unsigned)reg_info.size);
//         }
//         else if (reg_info.type & (MB_EVENT_COILS_RD | MB_EVENT_COILS_WR))
//         {
//             ESP_LOGI(TAG, "COILS %s (%" PRIu32 " us), ADDR:%u, TYPE:%u, INST_ADDR:0x%" PRIx32 ", SIZE:%u",
//                      rw_str,
//                      reg_info.time_stamp,
//                      (unsigned)reg_info.mb_offset,
//                      (unsigned)reg_info.type,
//                      (uint32_t)reg_info.address,
//                      (unsigned)reg_info.size);
//             if (coil_reg_params.coils_port1 == 0xFF)
//                 break;
//         }
//     }
//     // Destroy of Modbus controller on alarm
//     ESP_LOGI(TAG, "Modbus controller destroyed.");
//     vTaskDelay(100);
//     ESP_ERROR_CHECK(mbc_slave_destroy());
//     vTaskDelete(NULL);
// }

// // void slaveTaskStart()
// // {
// //   //xTaskCreate(slave_task, "mb_slave", 4096, NULL, 10, NULL);

// //   static StackType_t slTaskStack[CONFIG_SLAVE_TASK_STACK_SIZE];
// //   static StaticTask_t slTaskBuffer;

// //   xTaskCreateStatic(slave_task,
// //                     "mb_slave",
// //                     CONFIG_SLAVE_TASK_STACK_SIZE,   //                4096,
// //                           NULL,
// //                           CONFIG_SLAVE_TASK_PRIORITY ,   //    10,
// //                    slTaskStack,
// //                  &slTaskBuffer);
// // }

// void slaveTaskStart()
// {
//     static StackType_t slTaskStack[CONFIG_SLAVE_TASK_STACK_SIZE];
//     static StaticTask_t slTaskBuffer;

//     // Создание статической задачи
//     TaskHandle_t task_handle = xTaskCreateStaticPinnedToCore(
//         slave_task,                   // Функция задачи
//         "mb_slave",                   // Имя задачи
//         CONFIG_SLAVE_TASK_STACK_SIZE, // Размер стека
//         NULL,                         // Параметры
//         CONFIG_SLAVE_TASK_PRIORITY,   // Приоритет
//         slTaskStack,                  // Буфер стека
//         &slTaskBuffer,                // Буфер задачи
//         tskNO_AFFINITY);              // Без привязки к ядру или CONFIG_TASK_CORE_SLAVE

//     if (!task_handle)
//     {
//         ESP_LOGE(TAG, "Failed to create Slave Modbus task");
//         return;
//     }

//     ESP_LOGI(TAG, "Slave Modbus task created successfully");
// }
