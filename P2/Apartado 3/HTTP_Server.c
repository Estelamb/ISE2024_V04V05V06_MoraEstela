/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_Buttons.h"              // ::Board Support:Buttons
//#include "Board_ADC.h"                  // ::Board Support:A/D Converter
#include "rgb.h"
#include "lcd.h"
#include "Arial12x12.h"
#include "pot.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];

extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;

extern int32_t LED_rgb_Initialize (void);
extern int32_t LED_rgb_SetOut (uint32_t val);	

unsigned char bufferWeb[512];
uint16_t positionL1 = 0;
uint16_t positionL2 = 0;

extern void LCD_Initialize(void);
void LCD_update(void);
void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
void LCD_symbolToLocalBuffer(char text0[21], char text1[21]);

extern void POT_Initialize(void);

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1", "LCD line 2" };

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;

extern osThreadId_t tid_rtc;   

/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);

__NO_RETURN void app_main (void *arg);

/* Read digital inputs */
uint8_t get_button (void) 
{
  return ((uint8_t)Buttons_GetState ());
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) {
	(void)arg;
	
	while(1){
		/* Wait for signal from DHCP */
    osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever);
		osThreadFlagsSet (tid_rtc, 0x02);
		
		LCD_symbolToLocalBuffer(lcd_text[0], lcd_text[1]);
	}	
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) 
{
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  uint32_t cnt = 0U;

  (void)arg;

  LEDrun = true;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_rgb_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) 
{
  int i;
	(void)arg;

  LED_rgb_Initialize();
  Buttons_Initialize();
	
	LCD_Initialize();
	
	for(i=0; i<512; i++){
		bufferWeb[i] = 0x00;
	}
	
	LCD_update();
  POT_Initialize();

  netInitialize ();

  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);

  osThreadExit();
}

void LCD_symbolToLocalBuffer_L1(uint8_t symbol)
{
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	offset = 25*(symbol - ' ');
	
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset + i*2 + 1];
		value2 = Arial12x12[offset + i*2 + 2];
		
		bufferWeb[i + positionL1] = value1;
		bufferWeb[i + 128 + positionL1] = value2;
	}
	
	positionL1 = positionL1 + Arial12x12[offset];
}

void LCD_symbolToLocalBuffer_L2(uint8_t symbol)
{
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	offset = 25*(symbol - ' ');
	
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset + i*2 + 1];
		value2 = Arial12x12[offset + i*2 + 2];
		
		bufferWeb[i + 256 + positionL2] = value1;
		bufferWeb[i + 384 + positionL2] = value2;
	}
	
	positionL2 = positionL2 + Arial12x12[offset];
}

void LCD_update(void)
{
	int i;
	LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB0); // Página 0
	
	for(i=0;i<128;i++){
		LCD_wr_data(bufferWeb[i]);
	}
	
	LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB1); // Página 1
	
	for(i=128;i<256;i++){
		LCD_wr_data(bufferWeb[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB2); //Página 2
	
	for(i=256;i<384;i++){
		LCD_wr_data(bufferWeb[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB3); // Pagina 3
	
	for(i=384;i<512;i++){
		LCD_wr_data(bufferWeb[i]);
	}
}

void LCD_symbolToLocalBuffer(char text0[21], char text1[21])
{
	int i;
	
	/* Display user text lines */
	positionL1 = 0;
	positionL2 = 0;
		
	for(i=0; i<512; i++){
		bufferWeb[i] = 0x00;
	}
		
	for(i=0; i<strlen(text0); i++){
		LCD_symbolToLocalBuffer_L1(text0[i]);
	}
		
	for(i=0; i<strlen(text1); i++){
		LCD_symbolToLocalBuffer_L2(text1[i]);
	}
		
	LCD_update();
}
