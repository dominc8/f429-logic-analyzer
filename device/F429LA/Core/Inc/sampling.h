#ifndef SAMPLING_H
#define SAMPLING_H

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
    uint32_t baudrate;
    SamplingMode_T sampling_mode;
    SamplingSources_T sampling_sources;
    uint32_t sampling_rate;
} Config_T;

typedef struct Sample_Data_Tag Sample_Data_T;

struct Sample_Data_Tag
{
    uint8_t data[SAMPLES_SIZE];
    Sample_Data_T *next_data;
};
extern Sample_Data_T sample_arr[2];

void StartSampling(Config_T *config);
HAL_StatusTypeDef Timer_Init(void);


#endif /* SAMPLING_H */

