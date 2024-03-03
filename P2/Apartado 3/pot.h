#ifndef pot_h
#define pot_h

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

//Module initialización
void POT_Initialize(void);
static void GPIO_init(void);
static int ADC_Init(void);
uint16_t ADC_getVoltage(uint32_t ch);

#endif
