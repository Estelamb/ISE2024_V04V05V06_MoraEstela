#ifndef reloj_h
#define reloj_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include <stdio.h>
#include "stm32f4xx_it.h"
#include "rtc.h"
#include <time.h>
#include <rl_net.h>
#include "Net_Config_SNTP_Client.h"

//Module initialización
int Init_Thread_RTC(void);
void Error_Handler(void);
void Alarm_Initialize(void);
void get_time(void);
void RTC_SetTimeDate(uint32_t sntp);
void sincronizacion(void);
void init_Pulsador(void);

#endif
