#ifndef sleep_h
#define sleep_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "stm32f4xx_it.h"

//Module initialización
int Init_Thread_Sleep(void);
void Error_Handler(void);
void SleepMode_Measure(void);
void led_green(void);

#endif
