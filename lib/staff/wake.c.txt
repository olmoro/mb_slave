


#include "wake.h"

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

#define PACKET_READ_TICS        (100 / portTICK_PERIOD_MS)


char          rxSta;        // состояние процесса приема пакета
char          rxPre;        // предыдущий принятый байт
char          rxAdd;        // адрес, с которым сравнивается принятый
char          rxCmd;        // принятая команда
char          rxCrc;        // контрольная сумма принимаемого пакета
unsigned char rxPtr;        // указатель на массив принимаемых данных

char          txSta;        // состояние процесса передачи пакета
char          txPre;        // предыдущий переданный байт
char          txAdd;        // адрес, передававемый в пакете
char          txCrc;        // контрольная сумма передаваемого пакета
unsigned char txPtr;        // указатель на массив передаваемых данных


// Переменные - уточнить типы  
char    rxNbt;          // принятое количество байт в пакете
char    rxDat[FRAME];   // массив принятых данных
uint8_t command;        // код команды на выполнение

char    txCmd;          // команда, передаваемая в пакете
uint8_t txNbt;          // количество байт данных в пакете
char    txDat[FRAME];   // массив данных для передачи

uart_port_t port_sp;

void wakeInit( uint8_t addr, long time )
{
  //const int uart_num = CONFIG_SP_UART_PORT_NUM;
  port_sp = CONFIG_SP_UART_PORT_NUM;
  // #ifdef UART2
  //   //Serial2.begin(115200);            // это порт дрйвера
  //   Serial2.begin(230400);            // это порт дрйвера - май 2022
  //   Serial2.setTimeout( time );       // время тайм-аута для функции readBytes(), ms. 
  // #endif

  rxAdd   = addr;                     // адрес на прием
  txAdd   = addr;                     // адрес на передачу
  rxSta   = WAIT_FEND;                // ожидание пакета
  txSta   = SEND_IDLE;                // ничего пока не передаем
  command = CMD_NOP;                  // нет команды на выполнение
}


// Вычисление контрольной суммы
static void doCrc8(char b, char *crc)
{
  char i;
  for(i = 0; i < 8; b = b >> 1, i++)
    if((b ^ *crc) & 1) *crc = ((*crc ^ 0x18) >> 1) | 0x80;
    else *crc = (*crc >> 1) & ~0x80;
}

// Чтение байта
static int read_bytes(void* buf, uint32_t length)
{
  return uart_read_bytes( port_sp, buf, length, PACKET_READ_TICS);
}

// Чение данных порта
void wakeRead()
{
  char error_flags = 0;               // чтение флагов ошибок UART - пока нет
  
  uint8_t dataByte;
  //Serial2.readBytes( &dataByte, 1 );  // чтение одного байта с удалением из приемного буфера
  read_bytes( &dataByte, 1 );  // чтение одного байта с удалением из приемного буфера
  char pre = rxPre;                   // сохранение старого пре-байта

  if( error_flags )                   // если обнаружены ошибки при приеме байта
  {
    rxSta = WAIT_FEND;                // ожидание нового пакета
    command = CMD_ERR;                // сообщаем об ошибке
    return;
  }

  if( dataByte == FEND )              // если обнаружено начало фрейма,
  {
    rxPre = dataByte;                 // то сохранение пре-байта,
    rxCrc = CRC_INIT;                 // инициализация CRC,
    rxSta = WAIT_ADDR;                // сброс указателя данных,
    doCrc8( dataByte, &rxCrc );       // обновление CRC,
    return;                           // выход
  }

  if( rxSta == WAIT_FEND )            // -----> если ожидание FEND,
    return;                           // то выход

  rxPre = dataByte;                   // обновление пре-байта
  if( pre == 0xDB )                   // если пре-байт равен FESC,
  {
    if( dataByte == TFESC )           // а байт данных равен TFESC,
      dataByte = 0xDB;                // то заменить его на FESC
    else if( dataByte == TFEND )      // если байт данных равен TFEND,
          dataByte = FEND;           // то заменить его на FEND
    else
    {
      rxSta = WAIT_FEND;         // для всех других значений байта данных,
      command = CMD_ERR;         // следующего за FESC, ошибка
      return;
    }
  }
  else
  {
    if( dataByte == 0xDB )            // если байт данных равен FESC, он просто
      return;                         // запоминается в пре-байте
  }

  switch( rxSta )
  {
    case WAIT_ADDR :                   // -----> ожидание приема адреса
    {
      if( dataByte & 0x80 )           // если dataByte.7 = 1, то это адрес
      {
        dataByte = dataByte & 0x7F;   //обнуляем бит 7, получаем истинный адрес
        if( !dataByte || dataByte == rxAdd )   // если нулевой или верный адрес,
        {
          doCrc8( dataByte, &rxCrc ); // то обновление CRC и
          rxSta = WAIT_CMD;           // переходим к приему команды
          break;
        }
        rxSta = WAIT_FEND;            // адрес не совпал, ожидание нового пакета
        break;
      }                               // если dataByte.7 = 0, то
      rxSta = WAIT_CMD;               // сразу переходим к приему команды
    }
    case WAIT_CMD :                    // -----> ожидание приема команды
    {
      if( dataByte & 0x80 )           // проверка бита 7 данных
      {
        rxSta = WAIT_FEND;            // если бит 7 не равен нулю,
        command = CMD_ERR;            // то ошибка
        break;
      }
      rxCmd = dataByte;               // сохранение команды
      doCrc8( dataByte, &rxCrc );     // обновление CRC
      rxSta = WAIT_NBT;               // переходим к приему количества байт
      break;
    }
    case WAIT_NBT :                    // -----> ожидание приема количества байт
    {
      if( dataByte > FRAME )          // если количество байт > FRAME,
      {
        rxSta = WAIT_FEND;
        command = CMD_ERR;            // то ошибка
        break;
      }
      rxNbt = dataByte;
      doCrc8( dataByte, &rxCrc );     // обновление CRC
      rxPtr = 0;                      // обнуляем указатель данных
      rxSta = WAIT_DATA;              // переходим к приему данных
      break;
    }
    case WAIT_DATA :                   // -----> ожидание приема данных
    {
      if( rxPtr < rxNbt )             // если не все данные приняты,
      {
        rxDat[rxPtr++] = dataByte;    // то сохранение байта данных,
        doCrc8( dataByte, &rxCrc );   // обновление CRC
        break;
      }

      if(dataByte != rxCrc)           // если приняты все данные, то проверка CRC
      {
        rxSta = WAIT_FEND;            // если CRC не совпадает,
        command = CMD_ERR;            // то ошибка
        break;
      }
      rxSta = WAIT_FEND;              // прием пакета завершен,
      command = rxCmd;                // загрузка команды на выполнение
      break;
    }
  }
}

// Передача байта
static void write_bytes(char b, uint32_t length)
{
  uart_write_bytes(port_sp, txDat, length );
}

// Передача пакета
void wakeWrite()
{
  char dataByte;

  if( txPre == FEND )                     // если производится стаффинг,
  {
    dataByte = TFEND;                     // передача TFEND вместо FEND
    txPre = dataByte;
    write_bytes( dataByte, 1 );            // dataByte -> UART
    return;
  }

  if( txPre == 0xDB )                     // если производится стаффинг,
  {
    dataByte = TFESC;                     // передача TFESC вместо FESC
    txPre = dataByte;
    write_bytes( dataByte, 1 );            // dataByte -> UART
    return;
  }

  switch( txSta )
  {
    case SEND_ADDR :                       // -----> передача адреса
    {
      if( txAdd )                         // если адрес не равен нулю, передаем его
      {
        dataByte = txAdd;
        doCrc8( dataByte, &txCrc );       // вычисление CRC для истинного адреса
        dataByte |= 0x80;                 // установка бита 7 для передачи адреса
        txSta = SEND_CMD;
        break;
      }                                   // иначе сразу передаем команду
    }
    case SEND_CMD :                        // -----> передача команды
    {
      dataByte = txCmd & 0x7F;
      txSta = SEND_NBT;
      break;
    }
    case SEND_NBT :                        // -----> передача количества байт
    {
      dataByte = txNbt;
      txSta = SEND_DATA;
      txPtr = 0;                          // обнуление указателя данных для передачи
      break;
    }
    case SEND_DATA :                       // -----> передача данных
    {
      if(txPtr < txNbt)
      {
        dataByte = txDat[ txPtr++ ]; 

//SerialUSB.print(" ->0x");   SerialUSB.print( dataByte, HEX );           // dataByte -> USB
      }
      else
      {
        dataByte = txCrc;                 // передача CRC
        txSta = SEND_CRC;
      }
      break;
    }
    default:
    {
      txSta = SEND_IDLE;                  // передача пакета завершена
      return;
    }
  }

  if(txSta != SEND_CMD)                   // если не передача адреса, то
  {
    doCrc8( dataByte, &txCrc );           // вычисление CRC
  }

  txPre = dataByte;                       // сохранение пре-байта
  
  if( dataByte == FEND || dataByte == 0xDB )
  {
    dataByte = 0xDB;                      // передача FESC, если нужен стаффинг
  }

  write_bytes( dataByte, 1 );
}

// Инициализация передачи пакета
void wakeStartWrite()
{
  char dataByte = FEND;
  int countByte = 0;
  txCrc = CRC_INIT;                       // инициализация CRC,
  doCrc8( dataByte, &txCrc );             // обновление CRC
  txSta = SEND_ADDR;
  txPre = TFEND;
  write_bytes( dataByte, 1 );              // dataByte -> UART
    
  do
  {
    wakeWrite();                          // all bytes -> UART
    countByte++;
  }
  while( txSta != SEND_IDLE || countByte < 518 );

  if( countByte >= 518 )
  {
    txSta = SEND_IDLE;
  }
}

// передача ответа на команду 
//void MWake::txReplay(char n, char err)
void txReplay(char n, char err)
{
  txNbt = n;                      // количество байт
  txDat[0] = err;                 // код ошибки
  txCmd = command;                // команда
  wakeStartWrite();               // инициализация передачи
  command = CMD_NOP;              // команда обработана
}

static void configReply(char n, char err, uint8_t comm)
{
  txNbt = n;                      // количество байт
  txDat[0] = err;                 // код ошибки
  txCmd = comm;                   // команда
  wakeStartWrite();               // инициализация передачи
  command = CMD_NOP;              // команда обработана    
}

void configAsk(char n, uint8_t comm)
{
  txNbt = n;                      // количество байт
  txCmd = comm;                   // команда
  wakeStartWrite();               // инициализация передачи
  command = CMD_NOP;              // команда обработана    
}

  // работа с буфером приема
uint8_t getCommand() { return command; }

uint8_t get08(int i) { return rxDat[i]; }

uint16_t get16(int i)
{
  uint16_t par  = rxDat[i] << 8;
  return(  par |= rxDat[i+1]); 
}

float getF16(int i)
{
  uint16_t par  = (rxDat[i] << 8) & 0xff00;
  par |= rxDat[i+1];
  return (float)(par / 256); 
}

uint8_t getNbt() {return rxNbt;}



// работа с буфером передачи
void setU8(int id, uint8_t value)
{
  txDat[id] = value;   //uint8_t(( val       ) & 0xff);
}

// ======== Запись в буфер передатчика ========
// Ответить одним байтом
int replyU08(int id, uint8_t value)
{
  txDat[id] = value & 0xFF;
  return id+1;
}

// Ответить двухбайтовым по индексу
int replyU16(int id, uint16_t value)
{
  txDat[id]    = ( value >> 8) & 0xFF; // Hi
  txDat[id+1]  =   value & 0xFF;       // Lo
  return id+2;
}

// Ответить четырехбайтовым по индексу
int replyU32(int id, uint32_t value)
{
  txDat[id]    = ( value >> 24) & 0xFF; // Hi
  txDat[id]    = ( value >> 16) & 0xFF; // Lo
  txDat[id]    = ( value >>  8) & 0xFF; // Hi
  txDat[id+1]  =   value        & 0xFF; // Lo
  return id+4;
}

void testReply( int rxNbt )
{
  for( int i = 0; i < rxNbt ; i++ )
  txDat[i] = rxDat[i];
  txReplay( rxNbt, txDat[0] );
}
