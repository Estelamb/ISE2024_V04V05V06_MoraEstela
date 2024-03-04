#include "reloj.h"                        
 
/*----------------------------------------------------------------------------
 *      Thread 1 'reloj': Sample thread
 *---------------------------------------------------------------------------*/
 
/* RTC handler declaration */
RTC_AlarmTypeDef AlarmHandle;
RTC_HandleTypeDef RtcHandle_LCD;

const NET_ADDR4 ntp_server = { NET_ADDR_IP4, 0, 208, 85, 20, 220 };
 
osThreadId_t tid_rtc;                        // thread id
void Thread_RTC (void *argument);                   // thread function

char rtc_text[2][20+1] = { "00:00:00", "01-01-2000" };

extern void LCD_symbolToLocalBuffer(char text0[21], char text1[21]);
int cuenta;

GPIO_InitTypeDef GPIO_InitStruct;

extern osThreadId_t TID_Led;

static void time_callback (uint32_t seconds, uint32_t seconds_fraction);

int Init_Thread_RTC(void) 
{
	
  tid_rtc = osThreadNew(Thread_RTC, NULL, NULL);
  if (tid_rtc == NULL) {
    return(-1);
  }
	
	if (RTC_Initialize() != 0) {
    return(-1);
  }
	RtcHandle_LCD = RTC_GetHandle();
	
	init_Pulsador();

  return(0);
}
 
void Thread_RTC (void *argument) 
{
	RTC_TimeTypeDef rtcTime;
	RTC_DateTypeDef rtcDate;
	uint32_t flag = 0;
	
	osDelay(5000);
	
	RTC_SetTime(0x00, 0x00, 0x00);
	RTC_SetDate(0x01, 0x01, 0x00);
	
	get_time();
	
	Alarm_Initialize();
	cuenta = 0;
	
  while (1) {
		
		if(flag == 0x02) {
			flag = osThreadFlagsWait (0x01U | 0x02U, osFlagsWaitAny, osWaitForever);
		} else {
			rtcTime = RTC_GetTime();
			sprintf(rtc_text[0], "%02d:%02d:%02d", rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds);
			
			rtcDate = RTC_GetDate();
			sprintf(rtc_text[1], "%02d-%02d-%02d", rtcDate.Date, rtcDate.Month, 2000 + rtcDate.Year);
		
			LCD_symbolToLocalBuffer(rtc_text[0], rtc_text[1]);
			flag = osThreadFlagsWait (0x02U, osFlagsWaitAny, 100);
		}
		
		osThreadFlagsClear(0x01U | 0x02U);
		
    //osThreadYield();                            // suspend thread
  }
}

void RTC_Alarm_IRQHandler(void) 
{
  HAL_RTC_AlarmIRQHandler(&RtcHandle_LCD);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef * RtcHandle_LCD)
{
	
	if(cuenta < 180){
		cuenta++;
	} else {
		cuenta = 0;
		sincronizacion();
	}
}

void Alarm_Initialize(void)
{
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


void Error_Handler(void){}

void get_time (void) 
{
  if (netSNTPc_GetTime ((NET_ADDR *)&ntp_server, time_callback) != netOK) {
    Error_Handler();
  }
}

static void time_callback (uint32_t seconds, uint32_t seconds_fraction) 
{	
	if (seconds == 0) {
    Error_Handler();
  } else {
		RTC_SetTimeDate(seconds);
	}
}

void RTC_SetTimeDate(uint32_t seconds)
{
	struct tm *fecha;
  time_t tiempo = seconds + 3600;

  fecha = localtime(&tiempo);
	
	RTC_SetTime(fecha->tm_hour+6, fecha->tm_min, fecha->tm_sec);
	RTC_SetDate(fecha->tm_mday, fecha->tm_mon+1, (uint8_t)(fecha->tm_year-100+6));
}

void sincronizacion(void)
{
	get_time();
	osThreadFlagsSet (TID_Led, 0x01);
}

void init_Pulsador(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void EXTI15_10_IRQHandler(void) 
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
	
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_13){
		RTC_SetTime(0x00, 0x00, 0x00);
		RTC_SetDate(0x01, 0x01, 0x00);
	}
}
