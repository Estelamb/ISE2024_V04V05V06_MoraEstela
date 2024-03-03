#ifndef rgb_h
#define rgb_h

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

//Module initialización
int32_t LED_rgb_Initialize (void);
int32_t LED_rgb_Uninitialize (void);
int32_t LED_rgb_On (uint32_t num);
int32_t LED_rgb_Off (uint32_t num);
int32_t LED_rgb_SetOut (uint32_t val);
uint32_t LED_rgb_GetCount (void);

#endif
