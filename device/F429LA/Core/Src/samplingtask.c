/*
 * sampletask.c
 *
 *  Created on: Apr 14, 2020
 *      Author: dominik
 */

#include "samplingtask.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#define GREEN_LED_Pin GPIO_PIN_13

void StartSamplingTask(void const * arg)
{
	for(;;)
    {
        osDelay(1000);
        HAL_GPIO_TogglePin(GPIOG, GREEN_LED_Pin);
    }
}



