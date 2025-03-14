/*

*/
#ifndef _SP_DRIVER_H_
#define _SP_DRIVER_H_

#include <stdint.h>

#define SP_PORT_NUM (CONFIG_SP_UART_PORT_NUM)          // Number of UART port used for connection
#define SP_DEV_SPEED (CONFIG_SP_UART_BAUD_RATE)        // The communication speed of the UART
#define SP_TASK_STACK_SIZE (CONFIG_SP_TASK_STACK_SIZE) // =3072
#define SP_UART_RXD (CONFIG_SP_UART_RXD)               // =25
#define SP_UART_TXD (CONFIG_SP_UART_TXD)               // =26
#define SP_UART_RTS (CONFIG_SP_UART_RTS)               // =33
// #define SP_UART_CTS (CONFIG_SP_UART_NO_CHANGE)

#define BUF_SIZE (1024)


// Константы протокола
#define SOH 0x01        // Байт начала заголовка
#define ISI 0x1F        // Байт указателя кода функции
#define STX 0x02        // Байт начала тела сообщения
#define ETX 0x03        // Байт конца тела сообщения
#define DLE 0x10        // Байт символа-префикса
#define CRC_INIT 0x1021 // Битовая маска полинома
#define FRAME 128       //

// RX process states:
enum
{
    WAIT_SOH = 0,   // ожидание приема заголовка
    WAIT_DAD,       // ожидание приема адреса приёмника
    WAIT_SAD,       // ожидание приема адреса источника
    WAIT_FNC,       // ожидание приема кода функции
    WAIT_DATA_HEAD, // прием данных заголовка
    WAIT_DATA_SET,  // прием данных сообщения
    WAIT_CRC,       // ожидание окончания приема CRC
    WAIT_CARR       // ожидание несущей
};

// TX process states: (не корректировано)
enum
{
    SEND_IDLE, // состояние бездействия
    SEND_ADDR, // передача адреса
    SEND_CMD,  // передача команды
    SEND_NBT,  // передача количества байт в пакете
    SEND_DATA, // передача данных
    SEND_CRC,  // передача CRC
    SEND_END   // окончание передачи пакета
};

// Коды универсальных команд:

// Коды ошибок:

void wakeRead();

//     // передача ответа на команду
//     void txReplay(char n, char err);
void configAsk(char n, uint8_t comm); // для активного контроллера - без байта подтверждения

// работа с буфером приема
uint8_t getCommand();
uint8_t get08(int i);
uint16_t get16(int i);
float getF16(int i);
uint8_t getNbt(); // получить число байт в ответе

// работа с буфером передачи
void setU8(int id, uint8_t value);

int replyU08(int id, uint8_t value);
int replyU16(int id, uint16_t value);
int replyU32(int id, uint32_t value);

void testReply(int n); // тест отправить n байт из буфера приемника

void wakeWrite();
void wakeStartWrite();

#endif // _SP_DRIVER_H_
