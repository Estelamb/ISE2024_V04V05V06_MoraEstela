#ifndef led_h
#define led_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include <stdio.h>
#include "stm32f4xx_it.h"

//Module initializaci�n
int Init_Thread_LED(void);
void GPIO_Init(void);
void alarmLED(void);
#endif

