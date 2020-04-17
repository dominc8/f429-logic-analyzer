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
	uint8_t pData[] = { 'H', 'e', 'l', 'l', 'o', '!', '\r', '\n'};
	for(;;)
    {
        osDelay(1000);
        HAL_GPIO_TogglePin(GPIOG, GREEN_LED_Pin);
        HAL_UART_Transmit(&huart1, pData, 8, 100);
    }
}



