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

#define SAMPLES_SIZE		10000

typedef struct Sample_Data_Tag Sample_Data_T;

struct Sample_Data_Tag
{
	uint8_t data[SAMPLES_SIZE];
	Sample_Data_T *next_data;
};
extern Sample_Data_T sample_arr[2];

void StartSamplingTask(void const * arg);
HAL_StatusTypeDef SamplingTask_HALInit(void);


#endif /* INC_SAMPLINGTASK_H_ */
