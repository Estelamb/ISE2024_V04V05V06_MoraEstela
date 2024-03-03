#ifndef lcd_h
#define lcd_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "Driver_SPI.h"
#include "string.h"
#include <stdio.h>

//Module initialización
void LCD_Initialize(void);
void LCD_reset(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void delay (uint32_t n_microsegundos);
void LCD_init(void);

#endif
