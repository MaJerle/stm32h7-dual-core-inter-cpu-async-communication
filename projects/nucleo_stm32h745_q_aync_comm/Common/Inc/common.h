/**
 * \file            common.h
 * \brief           Common configuration for both cores
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef COMMON_HDR_H
#define COMMON_HDR_H

#include "stm32h7xx.h"
#include "ringbuff/ringbuff.h"

/* Align X to 4 bytes */
#define MEM_ALIGN(x)                        (((x) + 0x00000003) & ~0x00000003)

/* Shared RAM between 2 cores is SRAM4 in D3 domain */
#define SHD_RAM_START_ADDR                  0x38000000
#define SHD_RAM_LEN                         0x0000FFFF

/* Buffer from CM4 to CM7 */
#define BUFF_CM4_TO_CM7_ADDR                MEM_ALIGN(SHD_RAM_START_ADDR)
#define BUFF_CM4_TO_CM7_LEN                 MEM_ALIGN(sizeof(ringbuff_t))
#define BUFFDATA_CM4_TO_CM7_ADDR            MEM_ALIGN(BUFF_CM4_TO_CM7_ADDR + BUFF_CM4_TO_CM7_LEN)
#define BUFFDATA_CM4_TO_CM7_LEN             MEM_ALIGN(0x00000400)

/* Buffer from CM7 to CM4 */
#define BUFF_CM7_TO_CM4_ADDR                MEM_ALIGN(BUFFDATA_CM4_TO_CM7_ADDR + BUFFDATA_CM4_TO_CM7_LEN)
#define BUFF_CM7_TO_CM4_LEN                 MEM_ALIGN(sizeof(ringbuff_t))
#define BUFFDATA_CM7_TO_CM4_ADDR            MEM_ALIGN(BUFF_CM7_TO_CM4_ADDR + BUFF_CM7_TO_CM4_LEN)
#define BUFFDATA_CM7_TO_CM4_LEN             MEM_ALIGN(0x00000400)

/* Define semaphores */
#define HSEM_TAKE_RELEASE(_id_)             do { HAL_HSEM_FastTake((_id_)); HAL_HSEM_Release((_id_), 0); } while (0)
#define HSEM_WAKEUP_CPU2                    0
#define HSEM_WAKEUP_CPU2_MASK               __HAL_HSEM_SEMID_TO_MASK(HSEM_WAKEUP_CPU2)
#define HSEM_CM4_TO_CM7                     1
#define HSEM_CM4_TO_CM7_MASK                __HAL_HSEM_SEMID_TO_MASK(HSEM_CM4_TO_CM7)
#define HSEM_CM7_TO_CM4                     2
#define HSEM_CM7_TO_CM4_MASK                __HAL_HSEM_SEMID_TO_MASK(HSEM_CM7_TO_CM4)

/* Flags management */
#define WAIT_COND_WITH_TIMEOUT(c, t)        do {        \
    int32_t timeout = (int32_t)(t);                     \
    while ((c) && timeout-- > 0) {}                     \
    if (timeout < 0) {                                  \
        Error_Handler();                                \
    }                                                   \
} while (0)

/* LEDs */
#define LD1_GPIO_CLK_EN                     __HAL_RCC_GPIOB_CLK_ENABLE
#define LD1_GPIO_PORT                       GPIOB
#define LD1_GPIO_PIN                        GPIO_PIN_0
#define LD2_GPIO_CLK_EN                     __HAL_RCC_GPIOE_CLK_ENABLE
#define LD2_GPIO_PORT                       GPIOE
#define LD2_GPIO_PIN                        GPIO_PIN_1
#define LD3_GPIO_CLK_EN                     __HAL_RCC_GPIOB_CLK_ENABLE
#define LD3_GPIO_PORT                       GPIOB
#define LD3_GPIO_PIN                        GPIO_PIN_14

#endif /* COMMON_HDR_H */
