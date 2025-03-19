/*
  Файл   mboard.h
  Проект fsm25
  pcb: spn.55
*/

#ifndef _MBOARD_H_
#define _MBOARD_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h" 
#include "driver/gpio.h"

//typedef QueueHandle_t ledQueue_t;

// With this callback function, you can control the LEDs connected via the GPIO expansion boards
//typedef void (*ledCustomControl_t) (int8_t ledGPIO, bool ledValue);

#ifdef __cplusplus
extern "C" {
#endif

// Create LED control task
//ledQueue_t ledTaskCreate(int8_t ledGPIO, bool ledHigh, bool blinkPriority, const char* taskName, const uint32_t taskStackSize, ledCustomControl_t customControl);

void boardInit();

void ledsOn();
void ledsRed();
void ledsGreen();
void ledsBlue();
void ledsOff();

void ledRedToggle();
void ledGreenToggle();
void ledBlueToggle();

#ifdef __cplusplus
}
#endif

#endif // !_MBOARD_H_
