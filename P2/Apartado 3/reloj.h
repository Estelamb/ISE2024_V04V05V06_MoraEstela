#ifndef reloj_h
#define reloj_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include <stdio.h>
#include "stm32f4xx_it.h"
#include "rtc.h"

//Module initialización
int Init_Thread_RTC(void);
void Error_Handler(void);
void Alarm_Initialize(void);

#endif
