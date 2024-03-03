#include "pot.h"                          
#define RESOLUTION_12B 4096U
#define VREF 3.3f

ADC_HandleTypeDef hadc;

void POT_Initialize(void){
		
	GPIO_init();
	ADC_Init();
}

static void GPIO_init(void){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	/*PA3     ------> ADC1_IN3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 }

static int ADC_Init(void){
	ADC_ChannelConfTypeDef sConfig = {0};
	
   /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.ScanConvMode = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    return -1;
  }
 
	sConfig.Channel = 3;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    return -1;
  }
	
	return 0;

}

uint16_t ADC_getVoltage(uint32_t ch){
	uint16_t voltage = 0;
		
	if (ch == 0) {
		HAL_ADC_Start(&hadc);
				
		voltage = HAL_ADC_GetValue(&hadc);
		
		HAL_ADC_Stop(&hadc);
	}
		
	return voltage;

}
