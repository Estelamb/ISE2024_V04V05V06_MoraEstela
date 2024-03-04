#ifndef rtc_h
#define rtc_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include <stdio.h>
#include "stm32f4xx_it.h"

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

//Module initialización
int RTC_Initialize(void);
void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_SetDate(uint8_t date, uint8_t month, uint8_t year);
void Error_Handler(void);
RTC_TimeTypeDef RTC_GetTime(void);
RTC_DateTypeDef RTC_GetDate(void);
RTC_HandleTypeDef RTC_GetHandle(void);

#endif
