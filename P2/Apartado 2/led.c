#include "led.h" 
 
/*----------------------------------------------------------------------------
 *      Thread 3 'led': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread_LED;                        // thread id
 
void Thread_LED (void *argument);                   // thread function
 
GPIO_InitTypeDef GPIO_InitStruct_LED;

int Init_Thread_LED (void) {
  GPIO_Init();
  tid_Thread_LED = osThreadNew(Thread_LED, NULL, NULL);
  if (tid_Thread_LED == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_LED (void *argument) {
 
  while (1) {
    osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
		alarmLED();
    //osThreadYield();                            // suspend thread
  }
}

void GPIO_Init(void){
	__HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct_LED.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_LED.Pull = GPIO_PULLUP;
  GPIO_InitStruct_LED.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  GPIO_InitStruct_LED.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_LED);
}

void alarmLED(void){
	int i;
	
	for(i=0;i<10;i++){
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		osDelay(250);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		osDelay(250);
	}
}