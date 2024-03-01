#include "rtc.h"   

RTC_HandleTypeDef RtcHandle;

int RTC_Initialize(void){
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
	
	return 0;
}

void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds){
  RTC_TimeTypeDef stimestructure;

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
															 
void RTC_SetDate(uint8_t date, uint8_t month, uint8_t year) {
  RTC_DateTypeDef sdatestructure;

  sdatestructure.Year = year; //0x14;
  sdatestructure.Month = month; //RTC_MONTH_FEBRUARY;
  sdatestructure.Date = date; //0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY; //RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

RTC_TimeTypeDef RTC_GetTime(void){
	RTC_TimeTypeDef stimestructureget;
	
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
	
	return stimestructureget;
}

RTC_DateTypeDef RTC_GetDate(void){
	RTC_DateTypeDef sdatestructureget;
	
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);

	return sdatestructureget;
}

RTC_HandleTypeDef RTC_GetHandle(void){
	return RtcHandle;
}