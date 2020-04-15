/**
 * \file            main.c
 *
 * Cortex-M4 main.c file
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"

/* Ringbuffer variables */
volatile ringbuff_t* rb_cm4_to_cm7;
volatile ringbuff_t* rb_cm7_to_cm4;

/**
 * \brief           The application entry point
 */
int
main(void) {
    uint32_t i = 0;

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Assign pointers, no initialization at this stage */
    rb_cm4_to_cm7 = (void *)BUFF_CM4_TO_CM7_ADDR;
    rb_cm7_to_cm4 = (void *)BUFF_CM7_TO_CM4_ADDR;

    /* Enable hardware semaphore */
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_CM7_TO_CM4));

    while (1) {
        char c = '0' + (++i % 10);

        /* Write to buffer for Cortex-M4 */
        ringbuff_write(rb_cm4_to_cm7, "[CM4] Number: ", 14);
        ringbuff_write(rb_cm4_to_cm7, &c, 1);
        ringbuff_write(rb_cm4_to_cm7, "\r\n", 2);

        /* Delay not to spam CM7 too much */
        HAL_Delay(1000);
    }
}

/**
 * \brief           This function is executed in case of error occurrence
 */
void
Error_Handler(void) {

}
