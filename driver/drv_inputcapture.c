/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-20     liuc   first version
 */

/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-16     shelton      first version
 */

#include "drv_common.h"
#include "rtdevice.h"
#include "drv_inputcapture.h"


//#define DRV_DEBUG
#define LOG_TAG                         "drv.inputcapture"
#include <drv_log.h>

struct at32_inputcapture_device
{
    struct rt_inputcapture_device parent;
    tmr_type *tmr_x;
    uint32_t channel;
    rt_uint32_t tmr_interrupt_select;
    rt_uint32_t tmr_flag;
    IRQn_Type irqn;
    rt_uint32_t last_capture;
    char *name;
};

enum
{
#ifdef BSP_USING_IC4
    IC4_INDEX,
#endif
};

static struct at32_inputcapture_device at32_inputcapture_obj[] =
{
#ifdef BSP_USING_IC4
    IC4_CONFIG,
#endif
};




static rt_err_t at32_inputcapture_init(struct rt_inputcapture_device *device)
{
    struct at32_inputcapture_device *ic = (struct at32_inputcapture_device *)device->parent.user_data;
    at32_hw_inputcapture_init(ic->tmr_x);

    // 定时器初始化
    tmr_base_init(ic->tmr_x, 10000 - 1, 1500 - 1); // 定时器频率为 1 MHz
    tmr_cnt_dir_set(ic->tmr_x, TMR_COUNT_UP);      // 向上计数模式

    tmr_input_config_type input_config;
    input_config.input_channel_select = ic->channel; // 使用 CH1
    input_config.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
    input_config.input_polarity_select = TMR_INPUT_RISING_EDGE;
    input_config.input_filter_value = 0xF; // 滤波设置
    tmr_input_channel_init(ic->tmr_x, &input_config, TMR_CHANNEL_INPUT_DIV_1);

    return RT_EOK;
}

static rt_err_t at32_inputcapture_open(struct rt_inputcapture_device *device)
{
    struct at32_inputcapture_device *ic = (struct at32_inputcapture_device *)device->parent.user_data;

    tmr_counter_enable(ic->tmr_x, TRUE);
    tmr_interrupt_enable(ic->tmr_x, ic->tmr_interrupt_select, TRUE);
    nvic_irq_enable(ic->irqn, 0, 0);

    return RT_EOK;
}

static rt_err_t at32_inputcapture_close(struct rt_inputcapture_device *device)
{
    struct at32_inputcapture_device *ic = (struct at32_inputcapture_device *)device->parent.user_data;

    tmr_counter_enable(ic->tmr_x, FALSE);
    tmr_interrupt_enable(ic->tmr_x, ic->tmr_interrupt_select, FALSE);
    nvic_irq_disable(ic->irqn);

    return RT_EOK;
}


static rt_err_t at32_get_pulsewidth(struct rt_inputcapture_device *device, rt_uint32_t *pulsewidth)
{
    struct at32_inputcapture_device *ic = (struct at32_inputcapture_device *)device->parent.user_data;
    rt_uint32_t current_capture;
    rt_uint32_t timer_period = tmr_period_value_get(ic->tmr_x);
    current_capture = tmr_channel_value_get(ic->tmr_x, ic->channel);
    if (current_capture >= ic->last_capture)
    {
        *pulsewidth = current_capture - ic->last_capture;
    }
    else
    {
        *pulsewidth = (timer_period - ic->last_capture) + current_capture + 1;
    }

    ic->last_capture = current_capture;
    return RT_EOK;
}

#ifdef BSP_USING_IC4
void TMR4_GLOBAL_IRQHandler(void)
{
    struct at32_inputcapture_device *device = &at32_inputcapture_obj[IC4_INDEX] ;
    if (tmr_flag_get(TMR4, device->tmr_flag) != RESET)
    {
        tmr_flag_clear(TMR4, device->tmr_flag);
        rt_hw_inputcapture_isr(&device->parent,  RT_TRUE);
    }
}
#endif

static const struct rt_inputcapture_ops at32_inputcapture_ops =
{
    at32_inputcapture_init,
    at32_inputcapture_open,
    at32_inputcapture_close,
    at32_get_pulsewidth,
};

static void ic_get_channel(void)
{
#ifdef BSP_USING_IC4
    at32_inputcapture_obj[IC4_INDEX].channel = TMR_SELECT_CHANNEL_1;
    at32_inputcapture_obj[IC4_INDEX].tmr_interrupt_select = TMR_C1_INT;
    at32_inputcapture_obj[IC4_INDEX].tmr_flag = TMR_C1_FLAG;
#endif
}

static int rt_inputcapture_init(void)
{
    int i = 0;
    int result = RT_EOK;

    ic_get_channel();

    for(i = 0; i < sizeof(at32_inputcapture_obj) / sizeof(at32_inputcapture_obj[0]); i++)
    {
        at32_inputcapture_obj[i].parent.ops = &at32_inputcapture_ops;
        if(rt_device_inputcapture_register(&at32_inputcapture_obj[i].parent, at32_inputcapture_obj[i].name, &at32_inputcapture_obj[i]) == RT_EOK)
        {
          LOG_D("%s register success", at32_inputcapture_obj[i].name);
        }
        else
        {
          LOG_D("%s register failed", at32_inputcapture_obj[i].name);
          result = -RT_ERROR;
        }
    }

    return result;
}

INIT_BOARD_EXPORT(rt_inputcapture_init);


