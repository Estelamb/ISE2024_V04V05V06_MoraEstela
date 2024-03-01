#ifndef lcd_h
#define lcd_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "Driver_SPI.h"
#include "Arial12x12.h"
#include "string.h"
#include <stdio.h>

#define SIZE_MSGQUEUE_LCD			16

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
void LCD_Initialize(void);
void LCD_reset(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void delay (uint32_t n_microsegundos);
void LCD_init(void);
void LCD_update(void);
void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
void LCD_symbolToLocalBuffer_L2(uint8_t symbol);

#endif
