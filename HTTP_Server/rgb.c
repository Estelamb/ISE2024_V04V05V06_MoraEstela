#include "rgb.h"

/* GPIO Pin configuration structure */
typedef struct {
  GPIO_TypeDef *port;
  uint16_t      pin;
  GPIO_PinState inactive_state;
  GPIO_PinState active_state;
} GPIO_rgb_PIN;

/* LED GPIO Pins */
static const GPIO_rgb_PIN LED_rgb_PIN[] = {
  { GPIOB, GPIO_PIN_0,  GPIO_PIN_RESET, GPIO_PIN_SET },
  { GPIOB, GPIO_PIN_7,  GPIO_PIN_RESET, GPIO_PIN_SET },
  { GPIOB, GPIO_PIN_14, GPIO_PIN_RESET, GPIO_PIN_SET },
  { GPIOD, GPIO_PIN_11, GPIO_PIN_SET, GPIO_PIN_RESET },
  { GPIOD, GPIO_PIN_12, GPIO_PIN_SET, GPIO_PIN_RESET },
  { GPIOD, GPIO_PIN_13, GPIO_PIN_SET, GPIO_PIN_RESET }
};

#define LED_rgb_COUNT (sizeof(LED_rgb_PIN)/sizeof(GPIO_rgb_PIN))

/**
  \fn          int32_t LED_rgb_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_rgb_Initialize() {
	GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t          num;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	 for (num = 0U; num < LED_rgb_COUNT; num++) {
    GPIO_InitStruct.Pin = LED_rgb_PIN[num].pin;
    HAL_GPIO_Init    (LED_rgb_PIN[num].port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(LED_rgb_PIN[num].port, LED_rgb_PIN[num].pin, LED_rgb_PIN[num].inactive_state); 
  }

	return 0;
	
}

/**
  \fn          int32_t LED_rgb_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_rgb_Uninitialize (void) {
  uint8_t num;

  for (num = 0U; num < LED_rgb_COUNT; num++) {
    HAL_GPIO_DeInit(LED_rgb_PIN[num].port, LED_rgb_PIN[num].pin);
  }

  return 0;
}

/**
  \fn          int32_t LED_rgb_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_rgb_On (uint32_t num) {

  if (num >= LED_rgb_COUNT) {
    return -1;
  }

  HAL_GPIO_WritePin(LED_rgb_PIN[num].port, LED_rgb_PIN[num].pin, LED_rgb_PIN[num].active_state);

  return 0;
}

/**
  \fn          int32_t LED_mbed_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_rgb_Off (uint32_t num) {

  if (num >= LED_rgb_COUNT) {
    return -1;
  }

  HAL_GPIO_WritePin(LED_rgb_PIN[num].port, LED_rgb_PIN[num].pin, LED_rgb_PIN[num].inactive_state);

  return 0;
}

/**
  \fn          int32_t LED_rgb_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_rgb_SetOut (uint32_t val) {
  uint8_t num;

  for (num = 0U; num < LED_rgb_COUNT; num++) {
    if ((val & (1U << num)) != 0U) {
      LED_rgb_On (num);
    } else {
      LED_rgb_Off(num);
    }
  }

  return 0;
}

/**
  \fn          uint32_t LED_mbed_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_rgb_GetCount (void) {
  return LED_rgb_COUNT;
}
