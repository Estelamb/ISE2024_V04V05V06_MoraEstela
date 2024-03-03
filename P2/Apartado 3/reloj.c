#include "reloj.h"                        
 
/*----------------------------------------------------------------------------
 *      Thread 1 'reloj': Sample thread
 *---------------------------------------------------------------------------*/
 
/* RTC handler declaration */
//RTC_AlarmTypeDef AlarmHandle;
//RTC_HandleTypeDef RtcHandle_LCD;
 
osThreadId_t tid_Thread_RTC;                        // thread id
void Thread_RTC (void *argument);                   // thread function

char rtc_text[2][20+1] = { "12:00:00", "04-03-2024" };

//int cuenta;
 
int Init_Thread_RTC(void) {
	
  tid_Thread_RTC = osThreadNew(Thread_RTC, NULL, NULL);
  if (tid_Thread_RTC == NULL) {
    return(-1);
  }
	
	if (RTC_Initialize() != 0) {
    return(-1);
  }
	
	//RtcHandle_LCD = RTC_GetHandle();

  return(0);
}
 
void Thread_RTC (void *argument) {
	RTC_TimeTypeDef rtcTime;
	RTC_DateTypeDef rtcDate;
	
	RTC_SetTime(0x00, 0x00, 0x00);
	RTC_SetDate(0x04, 0x03, 0x24);
	
	//Alarm_Initialize();
	//cuenta = 0;
	
  while (1) {
			
		rtcTime = RTC_GetTime();
		sprintf(rtc_text[0], "%02d:%02d:%02d", rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds);
			
		rtcDate = RTC_GetDate();
		sprintf(rtc_text[1], "%02d-%02d-%02d", rtcDate.Date, rtcDate.Month, 2000 + rtcDate.Year);
		
		osDelay(1000);

		
    //osThreadYield();                            // suspend thread
  }
}

/*void RTC_Alarm_IRQHandler(void) {
  HAL_RTC_AlarmIRQHandler(&RtcHandle_LCD);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef * RtcHandle_LCD){
	
	if(cuenta < 60){
		cuenta++;
	} else {
		cuenta = 0;
		osThreadFlagsSet(tid_Thread_LED, 0x01);
	}
}

void Alarm_Initialize(void){
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	
	//Configurar la alarma para que se active cada minuto
	AlarmHandle.AlarmTime.Hours = 0;
	AlarmHandle.AlarmTime.Minutes = 0;
	AlarmHandle.AlarmTime.Seconds = 0;
	AlarmHandle.Alarm = RTC_ALARM_A;
	AlarmHandle.AlarmMask = RTC_ALARMMASK_ALL;
	AlarmHandle.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
	
	HAL_RTC_SetAlarm_IT(&RtcHandle_LCD, &AlarmHandle, RTC_FORMAT_BIN);

}
*/

void Error_Handler(void){}
