/*
  Файл   mboard.h
  Проект
  pcb: spn.55
*/

#ifndef _MBOARD_H_
#define _MBOARD_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    class MBoard
    {
    public:
        MBoard();
        ~MBoard();

        void boardInit();

        void ledsOn();
        void ledsRed();
        void ledsGreen();
        void ledsBlue();
        void ledsOff();

        void ledRedToggle();
        void ledGreenToggle();
        void ledBlueToggle();

    private:
    };

#ifdef __cplusplus
}
#endif

#endif // !_MBOARD_H_
