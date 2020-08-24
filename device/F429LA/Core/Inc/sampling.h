#ifndef SAMPLING_H
#define SAMPLING_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

#include <stdint.h>

#define SAMPLES_SIZE        10000

typedef enum
{
    SamplingMode_RT  = 0,
    SamplingMode_NRT = 1,
} SamplingMode_T;

typedef enum
{
    SamplingSources_ONE   = 1,
    SamplingSources_TWO   = 2,
    SamplingSources_FOUR  = 4,
    SamplingSources_EIGHT = 8,
} SamplingSources_T;

typedef struct
{
    uint32_t baudrate;
    SamplingMode_T sampling_mode;
    SamplingSources_T sampling_sources;
    uint32_t sampling_freq;
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

