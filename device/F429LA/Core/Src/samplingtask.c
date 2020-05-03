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


TIM_HandleTypeDef		htim7;



HAL_StatusTypeDef SamplingTask_HALInit(void)
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;

  /*Configure the TIM7 IRQ priority */
  HAL_NVIC_SetPriority(TIM7_IRQn, 10,0);

  /* Enable the TIM7 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM7_IRQn);

  /* Enable TIM7 clock */
  __HAL_RCC_TIM7_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Compute TIM7 clock 2*? */
  uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();

  /* Compute the prescaler value to have TIM7 counter clock equal to 1MHz */
  // uwPrescalerValue = 0;
  uwPrescalerValue = 7200;

  /* Initialize TIM6 */
  htim7.Instance = TIM7;

  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  // htim7.Init.Period = (uwTimclock / 1000000) - 1;
  htim7.Init.Period = 9999;
  htim7.Init.Prescaler = uwPrescalerValue;
  htim7.Init.ClockDivision = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&htim7) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    return HAL_TIM_Base_Start_IT(&htim7);
  }

  /* Return function status */
  return HAL_ERROR;
}

void TIM7_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE) != RESET)      //In case other interrupts are also running

    {

        if (__HAL_TIM_GET_ITSTATUS(&htim7, TIM_IT_UPDATE) != RESET)

        {

            __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);

            HAL_GPIO_TogglePin(GPIOG, GREEN_LED_Pin);
            /*put your code here */

        }

    }

}





