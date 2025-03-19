/*=====================================================================================
 * Description:
 *    ESP32 wake-master из проекта F:\GitHub\MKlon3.5v7\src\driver
 *====================================================================================*/
#ifndef _WAKE_H_
#define _WAKE_H_

//#include "stdint.h"
#include <stdint.h>

// Константы протокола
#define FEND  0xC0    //Frame END
#define FESC  0xDB    //Frame ESCape
#define TFEND 0xDC    //Transposed Frame END
#define TFESC 0xDD    //Transposed Frame ESCape

#define CRC_INIT 0xDE //Initial CRC value
#define FRAME 100     //

// RX process states:
enum { WAIT_FEND=0,     //ожидание приема FEND
       WAIT_ADDR,     //ожидание приема адреса
       WAIT_CMD,      //ожидание приема команды
       WAIT_NBT,      //ожидание приема количества байт в пакете
       WAIT_DATA,     //прием данных
       WAIT_CRC,      //ожидание окончания приема CRC
       WAIT_CARR };   //ожидание несущей


//TX process states:

enum { SEND_IDLE,     //состояние бездействия
       SEND_ADDR,     //передача адреса
       SEND_CMD,      //передача команды
       SEND_NBT,      //передача количества байт в пакете
       SEND_DATA,     //передача данных
       SEND_CRC,      //передача CRC
       SEND_END };    //окончание передачи пакета

//Коды универсальных команд:
#define CMD_NOP  0    //нет операции
#define CMD_ERR  1    //ошибка приема пакета
#define CMD_ECHO 2    //передать эхо
#define CMD_INFO 3    //передать информацию об устройстве

//Коды ошибок:
#define ERR_NO 0x00   //no error
#define ERR_TX 0x01   //Rx/Tx error
#define ERR_BU 0x02   //device busy error
#define ERR_RE 0x03   //device not ready error
#define ERR_PA 0x04   //parameters value error
#define ERR_NR 0x05   //no replay
#define ERR_NC 0x06   //no carrier






//     static constexpr uint8_t frame    = 0xFF; // Максимальная длина пакета 255 (полезных данных)

//     void wakeInit( uint8_t addr, long time );
    void wakeRead();

//     // передача ответа на команду
//     void txReplay(char n, char err);
    void configAsk(char n, uint8_t comm);         // для активного контроллера - без байта подтверждения

    // работа с буфером приема
    uint8_t  getCommand();
    uint8_t  get08(int i);
    uint16_t get16(int i);
    float    getF16(int i);
    uint8_t  getNbt();                        // получить число байт в ответе


    // работа с буфером передачи
    void setU8(int id, uint8_t value);

    int replyU08(int id, uint8_t  value);
    int replyU16(int id, uint16_t value);
    int replyU32(int id, uint32_t value);



    void testReply( int n );                  // тест отправить n байт из буфера приемника

    void wakeWrite();
    void wakeStartWrite();


#endif // _WAKE_H_
