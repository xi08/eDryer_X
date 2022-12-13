/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define retryTime 5
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

char AP_SSID[] = "";
char AP_password[] = "";
char Server_IP[] = "";
uint16_t Server_port = 0;

char uartRxBuffer[128];
uint8_t uartRxBufferIdx = 0;
uint8_t uartRxOKFlag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t WLAN_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* System interrupt init*/
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

    /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
     */
    LL_GPIO_AF_Remap_SWJ_NOJTAG();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM4_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    WLAN_Init();
    printf("Init OK");
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
    {
    }
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while (LL_RCC_HSE_IsReady() != 1)
    {
    }
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_6);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1)
    {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
    }
    LL_Init1msTick(48000000);
    LL_SetSystemCoreClock(48000000);
    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_4);
}

/* USER CODE BEGIN 4 */

int fputc(int ch, FILE *f)
{
    while (!LL_USART_IsActiveFlag_TXE(USART1))
        ;
    LL_USART_TransmitData8(USART1, (uint8_t)ch);
    return ch;
}

uint8_t WLAN_Init(void)
{
    uint8_t retry;

    /* Restore & Restart */
    /* at+restore*/
    printf("at+restore\r\n");
    LL_mDelay(3500);

    /* Open WiFi station mode */
    /* at+cwmode=1 */
    retry = retryTime;
    while (retry--)
    {
        printf("at+cwmode=1\r\n");
        LL_mDelay(500);

        if ((!strcmp(uartRxBuffer, "ok")) || (!strcmp(uartRxBuffer, "OK")))
            break;
        if (!retry)
            return 2;
    }

    /* Change TCP socket to passthrough mode */
    /* at+cipmode=1 */
    retry = 5;
    while (retry--)
    {
        printf("at+cipmode=1\r\n");
        LL_mDelay(500);

        if ((!strcmp(uartRxBuffer, "ok")) || (!strcmp(uartRxBuffer, "OK")))
            break;
        if (!retry)
            return 3;
    }

    /* Connect to the WiFi */
    /* at+cwjap="%AP_SSID","%AP_password" */
    retry = 5;
    while (retry--)
    {
        printf("at+cwjap=\"%s\",\"%s\"\r\n", AP_SSID, AP_password);
        LL_mDelay(2500);

        if ((!strcmp(uartRxBuffer, "ok")) || (!strcmp(uartRxBuffer, "OK")))
            break;
        if (!retry)
            return 4;
    }

    /* Start TCP socket */
    /* at+cipstart="TCP","%Server_IP",%Server_port */
    retry = 5;
    while (retry--)
    {
        printf("at+cipstart=\"TCP\",\"%s\",%u\r\n", Server_IP, Server_port);
        LL_mDelay(2500);

        if ((!strcmp(uartRxBuffer, "ok")) || (!strcmp(uartRxBuffer, "OK")))
            break;
        if (!retry)
            return 5;
    }

    /* Start passthrough transfer */
    /* at+cipsend */
    printf("at+cipsend\r\n");
    LL_mDelay(500);

    return 0;
}

inline void uart_ReceiveIRQ(void)
{
    uint8_t ch = LL_USART_ReceiveData8(USART1);

    if (uartRxBuffer[uartRxBufferIdx - 1] == (uint8_t)('\r') && ch == (uint8_t)('\n'))
    {
        uartRxBuffer[uartRxBufferIdx - 1] = 0;
        uartRxBufferIdx = 0;
        uartRxOKFlag = 1;
    }
    else
        uartRxBuffer[uartRxBufferIdx++] = ch;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
