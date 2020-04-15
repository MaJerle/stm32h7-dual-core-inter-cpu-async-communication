/**
 * \file            main.c
 *
 * Cortex-M7 main.c file
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;

/* Ringbuffer variables */
volatile ringbuff_t* rb_cm4_to_cm7;
volatile ringbuff_t* rb_cm7_to_cm4;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);

/**
 * \brief           The application entry point
 */
int
main(void) {
    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART3_UART_Init();

    /* Reset memory */
    memset((void *)SHD_RAM_START_ADDR, 0x00, SHD_RAM_LEN);

    /* Assign pointers and initialize */
    rb_cm4_to_cm7 = (void *)BUFF_CM4_TO_CM7_ADDR;
    rb_cm7_to_cm4 = (void *)BUFF_CM7_TO_CM4_ADDR;
    ringbuff_init(rb_cm7_to_cm4, (void *)BUFFDATA_CM7_TO_CM4_ADDR, BUFFDATA_CM7_TO_CM4_LEN);
    ringbuff_init(rb_cm4_to_cm7, (void *)BUFFDATA_CM4_TO_CM7_ADDR, BUFFDATA_CM4_TO_CM7_LEN);

    /* Enable hardware semaphore */
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_CM4_TO_CM7));

    /* Start second core */
    HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);

    /* Send test message */
    HAL_UART_Transmit(&huart3, (void *)"[CM7] Core ready\r\n", 18, 100);

    while (1) {
        size_t len;
        void* addr;

        /* Check if CM4 has written some data to CM7 core */
        len = ringbuff_get_linear_block_read_length(rb_cm4_to_cm7);
        if (len > 0) {
            addr = ringbuff_get_linear_block_read_address(rb_cm4_to_cm7);

            /* Transmit data */
            HAL_UART_Transmit(&huart3, addr, len, 1000);

            /* Mark buffer as read */
            ringbuff_skip(rb_cm4_to_cm7, len);
        }
    }
}

/**
 * \brief           System Clock Configuration
 */
void
SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* Supply configuration update enable */
    HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

    /* Configure the main internal regulator output voltage */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

    /** Initializes the CPU, AHB and APB busses clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /* Initializes the CPU, AHB and APB busses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                                  | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * \brief           USART3 Initialization Function
 */
static void
MX_USART3_UART_Init(void) {
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart3) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * \brief           GPIO Initialization function
 */
static void
MX_GPIO_Init(void) {
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
}

/**
 * @brief           This function is executed in case of error occurrence
 */
void
Error_Handler(void) {

}
