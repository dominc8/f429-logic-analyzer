/*
 * sampletask.h
 *
 *  Created on: Apr 14, 2020
 *      Author: dominik
 */

#ifndef INC_SAMPLINGTASK_H_
#define INC_SAMPLINGTASK_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

void StartSamplingTask(void const * arg);
HAL_StatusTypeDef SamplingTask_HALInit(void);


#endif /* INC_SAMPLINGTASK_H_ */
