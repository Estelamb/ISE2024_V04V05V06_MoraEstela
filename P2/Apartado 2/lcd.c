#include "lcd.h"                          
 
/*----------------------------------------------------------------------------
 *      Thread 2 'lcd': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_lcd;                       // thread id
 
int Init_Thread_LCD(void);
void Thread_lcd(void *argument);

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

GPIO_InitTypeDef GPIO_InitStruct_LCD;
TIM_HandleTypeDef tim7;
unsigned char buffer[512];
uint16_t positionL1 = 0;
uint16_t positionL2 = 0;

MSGQUEUE_LCD_t localLCD;
osMessageQueueId_t mid_MsgQueue_LCD;
int Init_MsgQueue_LCD (void);

GPIO_InitTypeDef GPIO_InitStruct;

int Init_Thread_LCD(void) {
	
	LCD_Initialize();
	Init_MsgQueue_LCD();
 
  tid_lcd = osThreadNew(Thread_lcd, NULL, NULL);
  if (tid_lcd == NULL) {
    return(-1);
  }
 
  return(0);
}

void Thread_lcd (void *argument) {
	int i;
	char time[128], date[128];
	
	positionL1 = 0;
  positionL2 = 0;
	
	for(i=0;i<512;i++){
		buffer[i] = 0x00;
	}
	
	LCD_update();
	
  while (1) {
		
		osMessageQueueGet(mid_MsgQueue_LCD, &localLCD, NULL, osWaitForever);
    
		positionL1 = 0;
		positionL2 = 0;
		
		sprintf(time, "%02d:%02d:%02d", localLCD.hours, localLCD.minutes, localLCD.seconds);
		sprintf(date, "%02d-%02d-%2d", localLCD.month, localLCD.date, 2000 + localLCD.year);
		
		for(i=0; i<strlen(time); i++){
			if(positionL1 + Arial12x12[25*(time[i] - ' ')] < 128){
				LCD_symbolToLocalBuffer_L1(time[i]);
			}
		}

		for(i=0; i<strlen(date); i++){
			if(positionL2 + Arial12x12[25*(date[i] - ' ')] < 128){
				LCD_symbolToLocalBuffer_L2(date[i]);
			}
		}
	
		LCD_update();
  }
}

int Init_MsgQueue_LCD (void) {
 
  mid_MsgQueue_LCD = osMessageQueueNew(SIZE_MSGQUEUE_LCD , sizeof(MSGQUEUE_LCD_t), NULL);
  if (mid_MsgQueue_LCD == NULL) {
    return (-1);
  }
  return(0);
}

void LCD_Initialize(void) {
	int i = 0;
	LCD_reset();
	LCD_init();
	
	
	for(i=0; i<512; i++){
		buffer[i] = 0x00;
	}
	
	LCD_update();
	
}

void LCD_reset(void){
	//Inicialización y configuración del driver SPI para gestionar el LCD
	  /* Initialize the SPI driver */
    SPIdrv->Initialize(NULL);
    /* Power up the SPI peripheral */
    SPIdrv->PowerControl(ARM_POWER_FULL);
    /* Configure the SPI to Master, 8-bit mode @20 MBits/sec */
    SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
	
	//Configurar los pines de IO que sean necesarios y programar su valor por defecto
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	GPIO_InitStruct_LCD.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_LCD.Pull = GPIO_PULLUP;
  GPIO_InitStruct_LCD.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct_LCD.Pin = GPIO_PIN_6; //RESET
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_LCD);
	
	GPIO_InitStruct_LCD.Pin = GPIO_PIN_14; //CS
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct_LCD);
	
	GPIO_InitStruct_LCD.Pin = GPIO_PIN_13; //A0
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct_LCD);
	
	//Generar la señal de reset
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); //GPIO_PIN_SET = 1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); //GPIO_PIN_RESET = 0
	delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delay(1000);
	
}

void delay(uint32_t n_microsegundos){
	//Configurar y arrancar el timer para generar un evento pasados n_microsegundos
	HAL_TIM_Base_MspInit(&tim7);
	__HAL_RCC_TIM7_CLK_ENABLE();
	
	tim7.Instance = TIM7;
	tim7.Init.Prescaler = 83;
	tim7.Init.Period = n_microsegundos - 1;
	tim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	//tim7.Init.ClockDivision = 1;
	
	HAL_TIM_Base_Init(&tim7);
	HAL_TIM_Base_Start_IT(&tim7);
	
	//Esperar a que se active el flag del registro de Match correspondiente
	while(__HAL_TIM_GetCounter(&tim7) < (n_microsegundos - 1));
	
	//Borrar el flag
	__HAL_TIM_CLEAR_IT(&tim7, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_FLAG(&tim7, TIM_FLAG_UPDATE);
	
	//Parar el timer y ponerlo a 0 para la siguiente llamada a la función
	HAL_TIM_Base_Stop_IT(&tim7);
	HAL_TIM_Base_DeInit(&tim7);
}

void LCD_wr_data(unsigned char data){
	//Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	
	//Seleccionar A0 = 1;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	
	//Escribir un dato (data) usando la función SPIDrv->Send(...);
	SPIdrv->Send(&data,1);
	
	//Esperar a que se libere el bus SPI;
	while(SPIdrv->GetStatus().busy);
	
	//Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_wr_cmd(unsigned char cmd){
	//Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	
	//Seleccionar A0 = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
	
	//Escribir un comando (cmd) usando la función SPIDrv->Send(...);
	SPIdrv->Send(&cmd,1);
	
	//Esperar a que se libere el bus SPI;
	while(SPIdrv->GetStatus().busy);
	
	//Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_init(void){
	LCD_wr_cmd(0xAE); //LCD display -> OFF
	LCD_wr_cmd(0xA2); //LCD drive voltage bias ratio -> 1/9 bias
	LCD_wr_cmd(0xA0); //RAM address SEG output -> normal
	LCD_wr_cmd(0xC8); //COM output scan direction -> normal direction
	LCD_wr_cmd(0x22); //Select internal resistor ratio (Rb/Ra) mode -> 2
	LCD_wr_cmd(0x2F); //Select internal power supply operating mode -> 7
	LCD_wr_cmd(0x40); //The display RAM display start line address -> 0
	LCD_wr_cmd(0xAF); //LCD display -> ON
	LCD_wr_cmd(0x81); //V0 output voltage electronic volume register -> 1
	LCD_wr_cmd(0x0F); //The least significant 4 bits of the display RAM colmn address -> F
	LCD_wr_cmd(0xA4); //Display all points normal -> normal display
	LCD_wr_cmd(0xA6); //LCD display -> normal
}

void LCD_update(void){
	int i;
	LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB0); // Página 0
	
	for(i=0;i<128;i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB1); // Página 1
	
	for(i=128;i<256;i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB2); //Página 2
	
	for(i=256;i<384;i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB3); // Pagina 3
	
	for(i=384;i<512;i++){
		LCD_wr_data(buffer[i]);
	}
}

void LCD_symbolToLocalBuffer_L1(uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	offset = 25*(symbol - ' ');
	
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset + i*2 + 1];
		value2 = Arial12x12[offset + i*2 + 2];
		
		buffer[i + positionL1] = value1;
		buffer[i + 128 + positionL1] = value2;
	}
	
	positionL1 = positionL1 + Arial12x12[offset];
}

void LCD_symbolToLocalBuffer_L2(uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset = 0;

	offset = 25*(symbol - ' ');
	
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset + i*2 + 1];
		value2 = Arial12x12[offset + i*2 + 2];
		
		buffer[i + 256 + positionL2] = value1;
		buffer[i + 384 + positionL2] = value2;
	}
	
	positionL2 = positionL2 + Arial12x12[offset];
}


