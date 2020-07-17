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

#include <stdint.h>

#define SAMPLES_SIZE        10000

typedef enum
{
    RT  = 0,
    NRT = 1,
} SamplingMode_T;

typedef enum
{
    ONE   = 0,
    TWO   = 1,
    FOUR  = 2,
    EIGHT = 8,
} SamplingSources_T;

typedef struct
{
    int32_t baudrate;
    SamplingMode_T sampling_mode;
    SamplingSources_T sampling_sources;
} Config_T;

typedef struct Sample_Data_Tag Sample_Data_T;

struct Sample_Data_Tag
{
    uint8_t data[SAMPLES_SIZE];
    Sample_Data_T *next_data;
};
extern Sample_Data_T sample_arr[2];

void StartSamplingTask(Config_T *config);
HAL_StatusTypeDef SamplingTask_HALInit(void);


#endif /* INC_SAMPLINGTASK_H_ */
