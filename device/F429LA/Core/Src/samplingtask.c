/*
 * sampletask.c
 *
 *  Created on: Apr 14, 2020
 *      Author: dominik
 */

#include <stdint.h>

#include "samplingtask.h"
#include "cmsis_os.h"

#define GREEN_LED_Pin GPIO_PIN_13

extern UART_HandleTypeDef huart1;

#define ARRAY_SIZE	100
volatile static uint8_t array[ARRAY_SIZE] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};








void StartSamplingTask(void const * arg)
{
	/* uint8_t pData[] = { 'H', 'e', 'l', 'l', 'o', '!', '\r', '\n'}; */

#define SAMPLES		1000

	volatile static uint8_t samples[SAMPLES];
	volatile uint8_t *sample_ptr = samples;

	for(;;)
    {
        /*
        osDelay(1000);
        HAL_GPIO_TogglePin(GPIOG, GREEN_LED_Pin);
        HAL_UART_Transmit(&huart1, pData, 8, 100);
        */
		while (sample_ptr < samples + SAMPLES)
		{
			*sample_ptr = (uint8_t)((GPIOE->IDR) & 0x3C);
			++sample_ptr;
		}
		sample_ptr = samples;
		HAL_UART_Transmit(&huart1, samples, SAMPLES, 1000);
    }
}


TIM_HandleTypeDef		htim8;

DMA_HandleTypeDef		hdma2;


HAL_StatusTypeDef SamplingTask_HALInit(void)
{
#if 0
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;
  HAL_StatusTypeDef		retval = HAL_ERROR;

  /*Configure the TIM8 IRQ priority */
  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn , 10,0);

  /* Enable the TIM8 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn );

  /* Enable TIM8 clock */
  __HAL_RCC_TIM8_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Compute TIM8 clock */
  uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();

  /* Compute the prescaler value to have TIM8 counter clock equal to 1MHz */
  // uwPrescalerValue = 0;
  uwPrescalerValue = 7200;

  /* Initialize TIM6 */
  htim8.Instance = TIM8;

  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  // htim7.Init.Period = (uwTimclock / 1000000) - 1;
  htim8.Init.Period = 9999;
  htim8.Init.Prescaler = uwPrescalerValue;
  htim8.Init.ClockDivision = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&htim8) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    retval = HAL_TIM_Base_Start_IT(&htim8);
  }
  // dma2 ch7str1

  /* Return function status */
  return retval;
#else
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;
  HAL_StatusTypeDef		retval = HAL_ERROR;

  /*Configure the TIM8 IRQ priority */
  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn , 10,0);
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn , 9,0);
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn );
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn );

  /* Enable TIM8 clock */
  __HAL_RCC_TIM8_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Compute TIM8 clock */
  uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();

  /* Compute the prescaler value to have TIM8 counter clock equal to 1MHz */
  // uwPrescalerValue = 0;
  uwPrescalerValue = 7200;

  /* Initialize TIM6 */
  htim8.Instance = TIM8;

  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  // htim7.Init.Period = (uwTimclock / 1000000) - 1;
  htim8.Init.Period = 9999;
  htim8.Init.Prescaler = uwPrescalerValue;
  htim8.Init.ClockDivision = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;




  if(HAL_TIM_Base_Init(&htim8) == HAL_OK)
  {
	  /* 14 - trigger dma, 8 - update dma*/
	  TIM8->DIER = (uint16_t)(0x0101);

	  TIM8->CR1 |= 1;

	  retval = HAL_OK;
  }
  // dma2 ch7str1
  /* Clear Interrupt Status Bits */
  DMA2->LIFCR |= (uint32_t)(0x00000F40);

  DMA2_Stream1->CR = DMA_CHANNEL_7
                   | DMA_PRIORITY_VERY_HIGH
                   | DMA_MINC_ENABLE
				   | DMA_PINC_ENABLE
				   | DMA_CIRCULAR;

  /* Size of data transfer */
  DMA2_Stream1->NDTR = 1;

  DMA2_Stream1->PAR = (uint32_t)(0x40021010);
  DMA2_Stream1->M0AR = (uint32_t)(array);

  DMA2_Stream1->CR |= 1;

  return retval;
#endif
}

void TIM8_UP_TIM13_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_UPDATE) != RESET)      //In case other interrupts are also running

    {

        if (__HAL_TIM_GET_ITSTATUS(&htim8, TIM_IT_UPDATE) != RESET)

        {

            __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_UPDATE);
            array[0] = 0xAA;
            HAL_GPIO_TogglePin(GPIOG, GREEN_LED_Pin);
            /*put your code here */

        }

    }

}

void DMA2_Stream1_IRQHandler(void)
{
	array[10] = 99;

}





