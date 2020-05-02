/*
 * sampletask.c
 *
 *  Created on: Apr 14, 2020
 *      Author: dominik
 */

#include <stdint.h>

#include "samplingtask.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

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



