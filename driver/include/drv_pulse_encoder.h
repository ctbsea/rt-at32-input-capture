#ifndef __PULSE_ENCODER_CONFIG_H__
#define __PULSE_ENCODER_CONFIG_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BSP_USING_PULSE_ENCODER3
#ifndef PULSE_ENCODER3_CONFIG
#define PULSE_ENCODER3_CONFIG                  \
    {                                          \
       .tmr_handler     = TMR3,       \
       .encoder_irqn    = TMR3_GLOBAL_IRQn,  \
       .name            = "pulse3"    \
    }
#endif /* PULSE_ENCODER3_CONFIG */
#endif /* BSP_USING_PULSE_ENCODER3 */


#ifdef __cplusplus
}
#endif

#endif /* __PULSE_ENCODER_CONFIG_H__ */
