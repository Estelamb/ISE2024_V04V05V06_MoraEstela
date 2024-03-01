#include "rtc.h"                        
 
/*----------------------------------------------------------------------------
 *      Thread 1 'rtc': Sample thread
 *---------------------------------------------------------------------------*/
 
/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;
RTC_AlarmTypeDef AlarmHandle;
 
osThreadId_t tid_Thread_RTC;                        // thread id
void Thread_RTC (void *argument);                   // thread function

MSGQUEUE_LCD_t localRTC;
extern osMessageQueueId_t mid_MsgQueue_LCD;

extern osThreadId_t tid_Thread_LED; 
int cuenta;
 
int Init_Thread_RTC(void) {
	
  tid_Thread_RTC = osThreadNew(Thread_RTC, NULL, NULL);
  if (tid_Thread_RTC == NULL) {
    return(-1);
  }
	
 	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_RTC_ENABLE();
	
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    return -1;
  }
  return(0);
}
 
void Thread_RTC (void *argument) {
	RTC_CalendarConfig(0x00, 0x00, 0x00, 0x24, 0x03, 0x01);
	Alarm_Initialize();
	cuenta = 0;
	
  while (1) {
 
		RTC_CalendarShow();
		osMessageQueuePut(mid_MsgQueue_LCD, &localRTC, 0U, 0U);
		
		osDelay(1000);
		
    //osThreadYield();                            // suspend thread
  }
}

static void RTC_CalendarConfig(uint8_t hours, uint8_t minutes, uint8_t seconds,
															 uint8_t year, uint8_t month, uint8_t date) {
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  sdatestructure.Year = year; //0x14;
  sdatestructure.Month = month; //RTC_MONTH_FEBRUARY;
  sdatestructure.Date = date; //0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY; //RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  stimestructure.Hours = hours; //0x02;
  stimestructure.Minutes = minutes; //0x00;
  stimestructure.Seconds = seconds; //0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT_24;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

static void RTC_CalendarShow(void){
	RTC_DateTypeDef sdatestructureget;
	RTC_TimeTypeDef stimestructureget;
	
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	
	localRTC.hours = stimestructureget.Hours;
	localRTC.minutes = stimestructureget.Minutes;
	localRTC.seconds = stimestructureget.Seconds;

	localRTC.month = sdatestructureget.Month;
	localRTC.date = sdatestructureget.Date;
	localRTC.year = sdatestructureget.Year;
}

void RTC_Alarm_IRQHandler(void) {
  HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef * RtcHandle){
	
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
	
	HAL_RTC_SetAlarm_IT(&RtcHandle, &AlarmHandle, RTC_FORMAT_BIN);

}

