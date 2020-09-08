/**
 * \file            main.c
 *
 * Cortex-M4 main.c file
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"

/* Ringbuffer variables */
volatile ringbuff_t* rb_cm4_to_cm7 = (void *)BUFF_CM4_TO_CM7_ADDR;
volatile ringbuff_t* rb_cm7_to_cm4 = (void *)BUFF_CM7_TO_CM4_ADDR;
static void led_init(void);

/**
 * \brief           The application entry point
 */
int
main(void) {
    uint32_t i = 0, time, t1, t2;

    /* CPU2 goes to STOP mode and waits CPU1 to initialize all the steps first */
    /* CPU1 will wakeup CPU2 with semaphore take and release events */
    /* HW semaphore Clock enable */
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_HSEM_ActivateNotification(HSEM_WAKEUP_CPU2_MASK);
    HAL_PWREx_ClearPendingEvent();
    HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
    __HAL_HSEM_CLEAR_FLAG(HSEM_WAKEUP_CPU2_MASK);

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Init LED3 */
    led_init();

    /*
     * Wait for buffers to be ready by CM7
     *
     * This should not wait and should go straight through
     */
    while (!ringbuff_is_ready(rb_cm4_to_cm7) || !ringbuff_is_ready(rb_cm7_to_cm4)) {}

    /* Write message to buffer */
    ringbuff_write(rb_cm4_to_cm7, "[CM4] Core ready\r\n", 18);

    /* Set default time */
    time = t1 = t2 = HAL_GetTick();
    while (1) {
        time = HAL_GetTick();

        /* Send data to CM7 */
        if (time - t1 >= 1000) {
            t1 = time;
            char c = '0' + (++i % 10);

            /* Write to buffer for Cortex-M4 */
            ringbuff_write(rb_cm4_to_cm7, "[CM4] Number: ", 14);
            ringbuff_write(rb_cm4_to_cm7, &c, 1);
            ringbuff_write(rb_cm4_to_cm7, "\r\n", 2);
        }

        /* Toggle LED */
        if (time - t2 >= 500) {
            t2 = time;
            HAL_GPIO_TogglePin(LD3_GPIO_PORT, LD3_GPIO_PIN);
        }
    }
}

/**
 * \brief           Initialize LEDs controlled by core
 */
void
led_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    LD3_GPIO_CLK_EN();

    GPIO_InitStruct.Pin = LD3_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD3_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * \brief           This function is executed in case of error occurrence
 */
void
Error_Handler(void) {

}
