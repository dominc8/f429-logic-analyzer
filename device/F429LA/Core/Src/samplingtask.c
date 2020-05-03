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

#define SAMPLES		1000

	volatile static uint8_t samples[SAMPLES];
	volatile uint8_t *sample_ptr = samples;

	for(;;)
    {
		while (sample_ptr < samples + SAMPLES)
		{
			*sample_ptr = (uint8_t)((GPIOE->IDR) & 0x3C);
			++sample_ptr;
		}
		sample_ptr = samples;
		HAL_UART_Transmit(&huart1, samples, SAMPLES, 1000);
		array[4] = 0xAA;
    }
}


TIM_HandleTypeDef		htim8;

HAL_StatusTypeDef SamplingTask_HALInit(void)
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;
  HAL_StatusTypeDef		retval = HAL_ERROR;

  /* Configure the IRQs */
  // HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn , 10,0);
  // HAL_NVIC_SetPriority(DMA2_Stream1_IRQn , 9,0);
  // HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn );
  // HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn );

  /* Enable clocks */
  __HAL_RCC_TIM8_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Compute TIM8 parameters */
  uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();
  uwPrescalerValue = 7200;

  /* Initialize TIM8 */
  htim8.Instance = TIM8;

  htim8.Init.Period = 9999;
  htim8.Init.Prescaler = uwPrescalerValue;
  htim8.Init.ClockDivision = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;

  if(HAL_OK == HAL_TIM_Base_Init(&htim8))
  {
	  /* 8 - update dma*/
	  TIM8->DIER = (uint16_t)(1 << 8);

	  retval = HAL_OK;
  }

  DMA2_Stream1->CR = 0;									/* Turn off DMA2 Stream1 */
  DMA2->LIFCR |= (uint32_t)(0x00000F40);				/* Clear Interrupt Status Bits */
  DMA2_Stream1->PAR = (volatile uint32_t)(0x40021010);	/* Set Peripheral (Source) Address */
  DMA2_Stream1->M0AR = (volatile uint32_t)(array + 4);	/* Set Memory (Destination) Address */
  DMA2_Stream1->NDTR = 4;								/* Size of single trasfer */

  DMA2_Stream1->CR = DMA_CHANNEL_7
                   | DMA_PRIORITY_HIGH
                   | DMA_MINC_ENABLE
				   | DMA_PINC_ENABLE
				   | DMA_CIRCULAR;;

  /* TODO: add FIFO if too big load */

  if (HAL_OK == retval)
  {
	  /* Turn on DMA2 Stream1 and TIM8 */
	  DMA2_Stream1->CR |= 1;
	  TIM8->CR1 |= 1;
  }

  return retval;
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
        }
    }
}






