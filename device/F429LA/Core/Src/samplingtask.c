/*
 * sampletask.c
 *
 *  Created on: Apr 14, 2020
 *      Author: dominik
 */

#include <stdint.h>

#include "samplingtask.h"

#define GREEN_LED_Pin GPIO_PIN_13
#define RED_LED_Pin   GPIO_PIN_14


extern UART_HandleTypeDef huart1;


Sample_Data_T sample_arr[2] = {{.data = {0}, .next_data = &sample_arr[1] }, {.data = {0}, .next_data = &sample_arr[0]}};

static volatile Sample_Data_T *curr_sample = sample_arr;

volatile uint8_t *sample_ptr = sample_arr[0].data;

static volatile uint8_t ready_to_transmission = 0;


static volatile uint32_t *sdram_ptr = (uint32_t *)0xD0000000;

void StartSamplingTask(void const * arg)
{


	// *sdram_ptr = (uint32_t)0x12345678;
	//HAL_GPIO_TogglePin(GPIOG, RED_LED_Pin);

	for(;;)
    {
		while (ready_to_transmission == 0);
		ready_to_transmission = 0;
		USART1->SR = ~UART_FLAG_TC;
		DMA2_Stream7->CR &= ~DMA_SxCR_EN;
		DMA2_Stream7->NDTR = SAMPLES_SIZE;
		DMA2->HIFCR = (uint32_t)(0x0F400000);
		DMA2_Stream7->M0AR = (uint32_t)(curr_sample->next_data->data);
		DMA2_Stream7->CR |= DMA_SxCR_EN;
		// HAL_UART_Transmit(&huart1, samples, SAMPLES_SIZE_2, 1000);

		// USART1->DR = 'A';
		// __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);
	    /* Enable the DMA transfer for transmit request by setting the DMAT bit
	    SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);
	       in the UART CR3 register */

		//if ((*sdram_ptr) == 0x12345678)
		HAL_GPIO_TogglePin(GPIOG, RED_LED_Pin);

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
  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn , 10, 0);
  // HAL_NVIC_SetPriority(DMA2_Stream7_IRQn , 9,0);
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
  // HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn );

  /* Enable clocks */
  __HAL_RCC_TIM8_CLK_ENABLE();
  // __HAL_RCC_DMA2_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Compute TIM8 parameters */
  uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();
  uwPrescalerValue = 72;

  /* Initialize TIM8 */
  htim8.Instance = TIM8;

  htim8.Init.Period = 72 * 10 / 2;
  htim8.Init.Prescaler = 0; //uwPrescalerValue;
  htim8.Init.ClockDivision = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;

  if(HAL_OK == HAL_TIM_Base_Init(&htim8))
  {
	  HAL_TIM_Base_Start_IT(&htim8);
	  retval = HAL_OK;
  }

  if (HAL_OK == retval)
  {
	  TIM8->CR1 |= 1;
  }

  return retval;
}




void TIM8_UP_TIM13_IRQHandler(void)
{
	static uint8_t move_mask = 4;
    if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_UPDATE) != RESET)      //In case other interrupts are also running
    {
        if (__HAL_TIM_GET_ITSTATUS(&htim8, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_UPDATE);
            /* Get pin values from E2-E5 and B12-B15 */
            *sample_ptr = (uint8_t)( ((GPIOE->IDR & 0x003C) >> 2) | ((GPIOB->IDR & 0xF000) >> 8));
            //*sample_ptr = 'A';
            sample_ptr++;
            if (sample_ptr >= curr_sample->data + SAMPLES_SIZE)
            {
            	ready_to_transmission = 1;
            	curr_sample = curr_sample->next_data;
            	sample_ptr = curr_sample->data;
            	HAL_GPIO_TogglePin(GPIOG, GREEN_LED_Pin);
            }
        }
    }
}

void EXTI0_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_FLAG(1);
	HAL_GPIO_TogglePin(GPIOG, RED_LED_Pin);
}






