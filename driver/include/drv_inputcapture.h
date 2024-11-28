/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-03-10     leo          first version
 */

#ifndef __IC_CONFIG_H__
#define __IC_CONFIG_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BSP_USING_IC4
#ifndef IC4_CONFIG
#define IC4_CONFIG                             \
    {                                           \
       .tmr_x                   = TMR4,         \
       .name                    = "inputcapture4",       \
       .irqn                    = TMR4_GLOBAL_IRQn,       \
       .channel                 = 0             \
    }
#endif /* PWM1_CONFIG */
#endif /* IC4_CONFIG */


#ifdef __cplusplus
}
#endif

#endif /* __IC_CONFIG_H__ */
