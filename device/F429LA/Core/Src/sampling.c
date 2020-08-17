#include "sampling.h"

#include "stm32f4xx_hal_uart.h"
#include <stdint.h>


#define GREEN_LED_Pin GPIO_PIN_13
#define RED_LED_Pin   GPIO_PIN_14


static TIM_HandleTypeDef htim8;

Sample_Data_T sample_arr[2] = {{.data = {0}, .next_data = &sample_arr[1] }, {.data = {0}, .next_data = &sample_arr[0]}};
static volatile Sample_Data_T *curr_sample = sample_arr;
static volatile uint8_t *sample_ptr = sample_arr[0].data;

static volatile uint8_t ready_to_transmission = 0;
static volatile uint8_t sampling_enabled = 0;
static volatile uint8_t sample_ptr_offset = 0;

static volatile uint32_t *sdram_ptr = (uint32_t *)0xD0000000;

static void RunSampling_RT(void);
static void RunSampling_NRT(void);
static void SampleSrc_1(void);
static void SampleSrc_2(void);
static void SampleSrc_4(void);
static void SampleSrc_8(void);

static void (*sample_func_ptr)(void) = &SampleSrc_8;

void StartSampling(Config_T *config)
{
    USART1->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), config->baudrate);

    switch (config->sampling_sources)
    {
        case SamplingSources_ONE:
            sample_func_ptr = &SampleSrc_1;
            break;
        case SamplingSources_TWO:
            sample_func_ptr = &SampleSrc_2;
            break;
        case SamplingSources_FOUR:
            sample_func_ptr = &SampleSrc_4;
            break;
        case SamplingSources_EIGHT:
        default:
            sample_func_ptr = &SampleSrc_8;
            break;
    }

    sampling_enabled = 1;
    sample_ptr_offset = 0;
    curr_sample = sample_arr;
    sample_ptr = curr_sample->data;

    /* Reset and restart timer */
    TIM8->ARR = 2 * HAL_RCC_GetPCLK1Freq() / config->sampling_freq;
    TIM8->CNT = 0;
    TIM8->CR1 |= TIM_CR1_CEN;

    switch (config->sampling_mode)
    {
        case SamplingMode_RT:
            RunSampling_RT();
            break;
        case SamplingMode_NRT:
            RunSampling_NRT();
            break;
        default:
            break;
    }
}

HAL_StatusTypeDef Timer_Init(void)
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              baseclock;
  uint32_t              pFLatency;
  HAL_StatusTypeDef     retval = HAL_ERROR;

  /* Configure the IRQs */
  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn , 10, 0);
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);

  /* Enable clocks */
  __HAL_RCC_TIM8_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Compute TIM8 parameters */
  baseclock = 2 * HAL_RCC_GetPCLK1Freq();

  /* Initialize TIM8 */
  htim8.Instance = TIM8;

  htim8.Init.Period = baseclock / 200000; /* 200 kHz, but it is overwritten by actual value */
  htim8.Init.Prescaler = 0;
  htim8.Init.ClockDivision = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;

  if(HAL_OK == HAL_TIM_Base_Init(&htim8))
  {
      __HAL_TIM_ENABLE_IT(&htim8, TIM_IT_UPDATE); /* HAL_TIM_Base_Start_IT(&htim8) but without starting timer just yet */
      retval = HAL_OK;
  }

  return retval;
}


void RunSampling_RT(void)
{
    int32_t left_samples;

    while(sampling_enabled)
    {
        while (ready_to_transmission == 0)
        {
        	if (sampling_enabled == 0)
                goto leave_loop;
        }

        ready_to_transmission = 0;

        USART1->SR = ~UART_FLAG_TC;
        DMA2_Stream7->CR &= ~DMA_SxCR_EN;
        DMA2_Stream7->NDTR = SAMPLES_SIZE;
        DMA2->HIFCR = (uint32_t)(0x0F400000);
        DMA2_Stream7->M0AR = (uint32_t)(curr_sample->next_data->data);
        DMA2_Stream7->CR |= DMA_SxCR_EN;

        HAL_GPIO_TogglePin(GPIOG, RED_LED_Pin);
    }


leave_loop:
    left_samples = (int32_t)sample_ptr - (int32_t)curr_sample->data;
    if ((left_samples > 0) && (left_samples < SAMPLES_SIZE))
    {
        USART1->SR = ~UART_FLAG_TC;
        DMA2_Stream7->CR &= ~DMA_SxCR_EN;
        DMA2_Stream7->NDTR = (uint32_t)left_samples;
        DMA2->HIFCR = (uint32_t)(0x0F400000);
        DMA2_Stream7->M0AR = (uint32_t)(curr_sample->data);
        DMA2_Stream7->CR |= DMA_SxCR_EN;
    }
}


void RunSampling_NRT(void)
{
}



void TIM8_UP_TIM13_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_UPDATE) != RESET)      //In case other interrupts are also running
    {
        if (__HAL_TIM_GET_ITSTATUS(&htim8, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_UPDATE);
            sample_func_ptr();
        }
    }
}


static void SampleSrc_1(void)
{
    *sample_ptr |= (uint8_t)((GPIOB->IDR & 0x0100) >> (sample_ptr_offset + 1));
    if (sample_ptr_offset == 0x07)
    {
        sample_ptr_offset = 0;
        sample_ptr++;

        if (sample_ptr >= curr_sample->data + SAMPLES_SIZE)
        {
            ready_to_transmission = 1;
            curr_sample = curr_sample->next_data;
            sample_ptr = curr_sample->data;
        }

        *sample_ptr = 0;
    }
    else
    {
        sample_ptr_offset++;
    }
}

static void SampleSrc_2(void)
{
    /* Get pin values from B8-B9, format: [B9 B8 B9 B8 B9 B8 B9 B8] */
    *sample_ptr |= (uint8_t)((GPIOB->IDR & 0x0300) >> (sample_ptr_offset + 2));
    if (sample_ptr_offset == 0x06)
    {
        sample_ptr_offset = 0;
        sample_ptr++;

        if (sample_ptr >= curr_sample->data + SAMPLES_SIZE)
        {
            ready_to_transmission = 1;
            curr_sample = curr_sample->next_data;
            sample_ptr = curr_sample->data;
        }

        *sample_ptr = 0;
    }
    else
    {
        sample_ptr_offset += 2;
    }
}

static void SampleSrc_4(void)
{
    /* Get pin values from B8-B11, format: [B11 B10 B9 B8 B11 B10 B9 B8] */
    *sample_ptr |= (uint8_t)((GPIOB->IDR & 0x0F00) >> (sample_ptr_offset + 4));
    if (sample_ptr_offset == 0x04)
    {
        sample_ptr_offset = 0;
        sample_ptr++;

        if (sample_ptr >= curr_sample->data + SAMPLES_SIZE)
        {
            ready_to_transmission = 1;
            curr_sample = curr_sample->next_data;
            sample_ptr = curr_sample->data;
        }

        *sample_ptr = 0;
    }
    else
    {
        sample_ptr_offset += 4;
    }
}


static void SampleSrc_8(void)
{
    /* Get pin values from E2-E5 and B8-B11, format: [B11 B10 B9 B8 E5 E4 E3 E2] */
    *sample_ptr = (uint8_t)( ((GPIOE->IDR & 0x003C) >> 2) | ((GPIOB->IDR & 0x0F00) >> 4));
    sample_ptr++;
    if (sample_ptr >= curr_sample->data + SAMPLES_SIZE)
    {
        ready_to_transmission = 1;
        curr_sample = curr_sample->next_data;
        sample_ptr = curr_sample->data;
    }
}




void EXTI0_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_FLAG(1);
    HAL_GPIO_TogglePin(GPIOG, RED_LED_Pin);
    TIM8->CR1 &= ~(TIM_CR1_CEN);
    sampling_enabled = 0;
    ready_to_transmission = 1;
}






