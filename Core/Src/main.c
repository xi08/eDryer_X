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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

char AP_SSID[] = "";
char AP_password[] = "";
char Server_IP[] = "";
uint16_t Server_port = 1347;

char uart1RxBuffer[128], uart1RxReg;
uint8_t uart1RxBufferIdx = 0;
uint8_t uart1RxOKFlag;

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

    // char tmp[65];

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_TIM4_Init();
    MX_ADC1_Init();
    /* USER CODE BEGIN 2 */
    WLAN_Init();
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
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}

uint8_t WLAN_Init(void)
{
    uint8_t retryTime = 0;
    HAL_StatusTypeDef uartStatus = 0;
    char cmd[127];

    /* Restore & Restart */
    sprintf(cmd, "at+restore\r\n");
    uartStatus = HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 5 + strlen(cmd) / 10);
    HAL_Delay(1500);
    uart1RxBufferIdx = 0;

    /* Command test after restore */
    /*
    retryTime = 5;
    while (retryTime--)
    {
        sprintf(cmd, "at\r\n");
        uartStatus = HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 5 + strlen(cmd) / 10);
        HAL_Delay(1500);
        if (!strcmp(uart1RxBuffer, "at") && uartStatus == HAL_OK)
            break;
        if (!retryTime)
            return 1;
    }*/

    /* Open WiFi station mode */
    retryTime = 5;
    while (retryTime--)
    {
        sprintf(cmd, "at+cwmode=1\r\n");
        HAL_UART_Receive_IT(&huart1, (uint8_t *)uart1RxBuffer, 4);
        uartStatus = HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 5 + strlen(cmd) / 10);
        HAL_Delay(1500);
        
        /*
        if (!strcmp(uart1RxBuffer, "OK") && uartStatus == HAL_OK)
            break;
        if (!retryTime)
            return 2;
        */
    }

    /* Change TCP socket to passthrough mode */
    retryTime = 5;
    while (retryTime--)
    {
        sprintf(cmd, "at+cipmode=1\r\n");
        HAL_UART_Receive_IT(&huart1, (uint8_t *)uart1RxBuffer, 50);
        uartStatus = HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 5 + strlen(cmd) / 10);
        HAL_Delay(1500);
        
        /*
        if (!strcmp(uart1RxBuffer, "OK") && uartStatus == HAL_OK)
            break;
        if (!retryTime)
            return 3;
        */
    }

    /* Connect to the WiFi */
    retryTime = 5;
    while (retryTime--)
    {
        sprintf(cmd, "at+cwjap=\"%s\",\"%s\"\r\n", AP_SSID, AP_password);
        HAL_UART_Receive_IT(&huart1, (uint8_t *)uart1RxBuffer, 50);
        uartStatus = HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 5 + strlen(cmd) / 10);
        //HAL_Delay(15000);
        /*
        if (!strcmp(uart1RxBuffer, "OK") && uartStatus == HAL_OK)
            break;
        if (!retryTime)
            return 4;
        */
    }

    /* Start TCP socket */
    retryTime = 5;
    while (retryTime--)
    {
        sprintf(cmd, "at+cipstart=\"TCP\",\"%s\",%u\r\n", Server_IP, Server_port);
        HAL_UART_Receive_IT(&huart1, (uint8_t *)uart1RxBuffer, 50);
        uartStatus = HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 5 + strlen(cmd) / 10);
        HAL_Delay(15000);
        /*
        if (!strcmp(uart1RxBuffer, "OK") && uartStatus == HAL_OK)
            break;
        if (!retryTime)
            return 5;
        */
    }

    /* Start passthrough transfer */
    sprintf(cmd, "at+cipsend\r\n");
    uartStatus = HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 5 + strlen(cmd) / 10);
    HAL_Delay(1500);

    return 0;
}
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if ((huart->Instance) == USART1)
    {

        if (uart1RxBuffer[uart1RxBufferIdx] == 0x0d && uart1RxReg == 0x0a)
        {
            uart1RxOKFlag = 1;
            uart1RxBuffer[uart1RxBufferIdx] = '\0';
            uart1RxBufferIdx = 0;
        }
        else if (uart1RxBufferIdx == 126)
            uart1RxBufferIdx = 0;
        else
            uart1RxBuffer[uart1RxBufferIdx++] = uart1RxReg;
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart1RxReg, 1);
    }
}
*/

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
