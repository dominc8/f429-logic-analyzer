/**
-  ******************************************************************************
-  * @file           : main.c
-  * @brief          : Main program body
-  ******************************************************************************
-  * @attention
-  *
-  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
-  * All rights reserved.</center></h2>
-  *
-  * This software component is licensed by ST under Ultimate Liberty license
-  * SLA0044, the "License"; You may not use this file except in compliance with
-  * the License. You may obtain a copy of the License at:
-  *                             www.st.com/SLA0044
-  *
-  ******************************************************************************
-  */
#include "sampling.h"
#include "main.h"
#include "stm32f429i_discovery_sdram.h"

#define CMD_LEN 20
static char rx_buffer[CMD_LEN] = { 0 };
static volatile uint8_t new_cmd = 0;


static void SystemClock_Config(void);
static void GPIO_Init(void);
static void DMA_Uart_Init(void);
static void ParseCommand(Config_T *config);


int main (void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();

    if (BSP_SDRAM_Init() == SDRAM_OK)
    {
        HAL_GPIO_TogglePin(GPIOG, LD3_Pin);
    }

    DMA_Uart_Init();

    // while (HAL_OK != Timer_Init())
    // {
    // }

    Config_T config =
    {
        .baudrate = 115200,
        .sampling_mode = RT,
        .sampling_sources = EIGHT,
        .sampling_rate = 200000,
    };

    while (1)
    {
        while (new_cmd == 0);

        ParseCommand(&config);

        new_cmd = 0;
        USART1->CR3 |= USART_CR3_DMAR;
        // TODO: Parse Incoming Uart Data,
        // possibly change configuration (Config_T config)
        // and start transmission by calling StartSampling()
        // StartSampling(&config);

        //HAL_GPIO_TogglePin(GPIOG, LD4_Pin);
    }
}


static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 72;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 3;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
      Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
}


static void DMA_Uart_Init(void)
{
    /* DMA2 Channel4 Stream7 for USART1 Tx initial configuration.
     * DMA2 Channel4 Stream5 for USART1 Rx initial configuration.
     * The rest is configured before sampling. */

    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    DMA2_Stream7->CR = 0;                                       /* Turn off DMA2 Stream7 */
    DMA2_Stream5->CR = 0;                                       /* Turn off DMA2 Stream5 */
    DMA2->HIFCR |= (uint32_t)(0x0F400F40);                      /* Clear Interrupt Status Bits */

    DMA2_Stream5->PAR  = (uint32_t)&(USART1->DR);               /* Set Peripheral Rx Source Address */
    DMA2_Stream5->M0AR = (uint32_t)&(rx_buffer[0]);             /* Set Memory Rx Destination Address */
    DMA2_Stream5->NDTR = CMD_LEN;
    DMA2_Stream5->CR   = DMA_CHANNEL_4          |
                         DMA_CIRCULAR           |
                         // DMA_SxCR_DBM           |
                         DMA_PRIORITY_VERY_HIGH |
                         DMA_MINC_ENABLE        |
                         DMA_PERIPH_TO_MEMORY   |
                         DMA_IT_TC              |
                         DMA_SxCR_EN;

    DMA2_Stream7->PAR  = (uint32_t)&(USART1->DR);                /* Set Peripheral Tx Destination Address */
    DMA2_Stream7->CR   = DMA_CHANNEL_4          |
                         DMA_PRIORITY_VERY_HIGH |
                         DMA_MINC_ENABLE        |
                         DMA_MEMORY_TO_PERIPH;

    USART1->SR  = 0;
    USART1->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), 115200);
    USART1->CR1 = UART_STATE_ENABLE | UART_MODE_TX_RX | UART_FLAG_RXNE;
    USART1->CR3 = USART_CR3_DMAR | USART_CR3_DMAT;


    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}



static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIOs Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Levels */
    HAL_GPIO_WritePin(GPIOC, CSX_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, RDX_Pin|WRX_DCX_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOG, LD3_Pin|LD4_Pin, GPIO_PIN_RESET);


    /* Configure GPIO pins : LD3_Pin LD4_Pin */
    GPIO_InitStruct.Pin     = LD3_Pin|LD4_Pin;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull    = GPIO_NOPULL;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* Configure GPIO pins for sampling data (E2-E5) */
    GPIO_InitStruct.Pin = (uint16_t)(0x000F << 2);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* Configure GPIO pins for sampling data (B8-B11) */
    GPIO_InitStruct.Pin = (uint16_t)(0x000F << 8);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : CSX_Pin  */
    GPIO_InitStruct.Pin = CSX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : B1_Pin TP_INT1_Pin */
    GPIO_InitStruct.Pin = B1_Pin|TP_INT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : BOOT1_Pin */
    GPIO_InitStruct.Pin = BOOT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : TE_Pin */
    GPIO_InitStruct.Pin = TE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TE_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : RDX_Pin WRX_DCX_Pin */
    GPIO_InitStruct.Pin = RDX_Pin|WRX_DCX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* Configure GPIO pins : USART1 */
    GPIO_InitStruct.Pin     = STLINK_TX_Pin | STLINK_RX_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    GPIO_InitStruct.Mode    = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull    = GPIO_NOPULL;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(STLINK_RX_TX_GPIO_Port, &GPIO_InitStruct);


    /*Configure GPIO pins : User button B1 */
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

void ParseCommand(Config_T *config)
{
}




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM6) {
      HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}


void Error_Handler(void)
{
}


void DMA2_Stream5_IRQHandler(void)
{
	DMA2->HIFCR = (uint32_t)(0x00000F40);
    USART1->CR3 &= ~USART_CR3_DMAR;
    HAL_GPIO_TogglePin(GPIOG, LD4_Pin);
    new_cmd = 1;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
