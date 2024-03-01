#ifndef rtc_h
#define rtc_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include <stdio.h>
#include "stm32f4xx_it.h"

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

//Header file only contains datatypes and functions that we made available for other modules
typedef struct { 
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
  uint8_t month;
	uint8_t date;
	uint8_t year;
} MSGQUEUE_LCD_t;

//Module initialización
int Init_Thread_RTC(void);
static void RTC_CalendarConfig(uint8_t hours, uint8_t minutes, uint8_t seconds,
															 uint8_t year, uint8_t month, uint8_t date);
static void RTC_CalendarShow(void);
void Error_Handler(void);
void Alarm_Initialize(void);

#endif
