/*
 *
 *
*/

#include "sp_driver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define PACKET_READ_TICS        (100 / portTICK_PERIOD_MS)

const uart_port_t uart_num = SP_PORT_NUM;
uart_config_t uart_config = {
    .baud_rate = SP_DEV_SPEED,  // Скорость передачи в Бодах
    .data_bits = UART_DATA_8_BITS,  // Количество переданных битов
    .parity = UART_PARITY_DISABLE,  // Контроль четности
    .stop_bits = UART_STOP_BITS_1,  // Количество стоп-битов
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,  // Режим Аппаратного управления потоком
    .rx_flow_ctrl_thresh = 122,
};

// Configure UART parameters
ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));





// ==============================================================================

char rxSta;          // состояние процесса приема пакета
char rxPre;          // предыдущий принятый байт
char rxAdd;          // адрес, с которым сравнивается принятый
char rxCmd;          // принятая команда
char rxCrc;          // контрольная сумма принимаемого пакета
unsigned char rxPtr; // указатель на массив принимаемых данных

char txSta;          // состояние процесса передачи пакета
char txPre;          // предыдущий переданный байт
char txAdd;          // адрес, передававемый в пакете
char txCrc;          // контрольная сумма передаваемого пакета
unsigned char txPtr; // указатель на массив передаваемых данных

// Переменные - уточнить типы  
char    rxNbt;          // принятое количество байт в пакете
char    rxDat[FRAME];   // массив принятых данных
uint8_t command;        // код команды на выполнение

char    txCmd;          // команда, передаваемая в пакете
uint8_t txNbt;          // количество байт данных в пакете
char    txDat[FRAME];   // массив данных для передачи

uart_port_t port_sp;

// void wakeInit( uint8_t addr, long time )
// {
//   //const int uart_num = CONFIG_SP_UART_PORT_NUM;
//   port_sp = CONFIG_SP_UART_PORT_NUM;
//   // #ifdef UART2
//   //   //Serial2.begin(115200);            // это порт дрйвера
//   //   Serial2.begin(230400);            // это порт дрйвера - май 2022
//   //   Serial2.setTimeout( time );       // время тайм-аута для функции readBytes(), ms. 
//   // #endif

//   rxAdd   = addr;                     // адрес на прием
//   txAdd   = addr;                     // адрес на передачу
//   rxSta   = WAIT_FEND;                // ожидание пакета
//   txSta   = SEND_IDLE;                // ничего пока не передаем
//   command = CMD_NOP;                  // нет команды на выполнение
// }

// // Вычисление контрольной суммы
// static void doCrc8(char b, char *crc)
// {
//   char i;
//   for(i = 0; i < 8; b = b >> 1, i++)
//     if((b ^ *crc) & 1) *crc = ((*crc ^ 0x18) >> 1) | 0x80;
//     else *crc = (*crc >> 1) & ~0x80;
// }

/* Функция вычисляет и возвращает циклический код для 
  последовательности из len байтов, указанной *msg.
  Используется порождающий полином:
  (X в степени 16)+(X в степени 12)+(X в степени 5)+1.
  Полиному соответствует битовая маска 0x1021.
*/
int CRCode(char *msg, int len)
{
    int j, crc = 0;
    while (len-- > 0)
    {
        crc = crc ^ (int)*msg++ << 8;
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}
