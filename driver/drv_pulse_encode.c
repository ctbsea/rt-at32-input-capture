#include "drv_common.h"
#include "drv_pulse_encoder.h"
#include <rtdevice.h>

#ifdef RT_USING_PULSE_ENCODER

#define DRV_DEBUG
#define LOG_TAG             "drv.pulse_encoder"

#include <drv_log.h>

#if !defined(BSP_USING_PULSE_ENCODER3)
    #error "Please define at least one BSP_USING_PULSE_ENCODERx"
    /* this driver can be disabled at menuconfig → RT-Thread Components → Device Drivers */
#endif

#define AUTO_RELOAD_VALUE 0xFFFF

enum
{
#ifdef BSP_USING_PULSE_ENCODER3
    PULSE_ENCODER3_INDEX,
#endif
};

struct at32_pulse_encoder_device
{
    struct rt_pulse_encoder_device pulse_encoder;
    tmr_type* tmr_handler;
    IRQn_Type encoder_irqn;
    rt_int32_t over_under_flowcount;
    char *name;
};

static struct at32_pulse_encoder_device at32_pulse_encoder_obj[] =
{
#ifdef BSP_USING_PULSE_ENCODER3
    PULSE_ENCODER3_CONFIG,
#endif
};

rt_err_t at32_pulse_encoder_init(struct rt_pulse_encoder_device *pulse_encoder)
{
    tmr_type *tmr_x = RT_NULL;
    RT_ASSERT(pulse_encoder != RT_NULL);

    struct at32_pulse_encoder_device *at32_encoder_device;
    at32_encoder_device = (struct at32_pulse_encoder_device*)pulse_encoder;

    tmr_x = (tmr_type *)pulse_encoder->parent.user_data;

    at32_msp_pulse_encode_init(tmr_x);

    tmr_base_init(tmr_x, AUTO_RELOAD_VALUE, TMR_CLOCK_DIV2);
    tmr_cnt_dir_set(tmr_x, TMR_COUNT_UP);

    tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
    tmr_period_buffer_enable(TMR3, FALSE);

    tmr_sub_sync_mode_set(TMR3, FALSE);
    tmr_primary_mode_select(TMR3, TMR_PRIMARY_SEL_RESET);

    tmr_input_config_type  tmr_input_struct;
    /* configure encoder mode */
    tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_1;
    tmr_input_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
    tmr_input_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
    tmr_input_struct.input_filter_value = 0;
    tmr_input_channel_init(tmr_x, &tmr_input_struct, TMR_CHANNEL_INPUT_DIV_1);

    tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_2;
    tmr_input_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
    tmr_input_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
    tmr_input_struct.input_filter_value = 0;
    tmr_input_channel_init(tmr_x, &tmr_input_struct, TMR_CHANNEL_INPUT_DIV_1);

    tmr_encoder_mode_config(tmr_x, TMR_ENCODER_MODE_A, TMR_INPUT_FALLING_EDGE, TMR_INPUT_RISING_EDGE);

    LOG_D("%s init success", at32_encoder_device->name);
    return RT_EOK;
}

rt_err_t pulse_encoder_clear_count(struct rt_pulse_encoder_device *pulse_encoder)
{
    struct at32_pulse_encoder_device *at32_device;
    at32_device = (struct at32_pulse_encoder_device*)pulse_encoder;
    at32_device->over_under_flowcount = 0;

    tmr_type *tmr_x = RT_NULL;
    RT_ASSERT(pulse_encoder != RT_NULL);

    tmr_x = (tmr_type *)pulse_encoder->parent.user_data;
    /* set tmr_x count */
    tmr_counter_value_set(tmr_x, 0);
    return RT_EOK;
}

rt_int32_t pulse_encoder_get_count(struct rt_pulse_encoder_device *pulse_encoder)
{
    tmr_type *tmr_x = RT_NULL;
    RT_ASSERT(pulse_encoder != RT_NULL);
    tmr_x = (tmr_type *)pulse_encoder->parent.user_data;

    return tmr_counter_value_get(tmr_x);
}

rt_err_t pulse_encoder_control(struct rt_pulse_encoder_device *pulse_encoder, rt_uint32_t cmd, void *args)
{
    tmr_type *tmr_x = RT_NULL;
    RT_ASSERT(pulse_encoder != RT_NULL);
    tmr_x = (tmr_type *)pulse_encoder->parent.user_data;

    rt_err_t result = RT_EOK;

    switch (cmd)
    {
    case PULSE_ENCODER_CMD_ENABLE:
        /* start timer */
        tmr_counter_enable(tmr_x, TRUE);
        break;
    case PULSE_ENCODER_CMD_DISABLE:
        /* stop timer */
        tmr_counter_enable(tmr_x, FALSE);
        /* set tmr_x count */
        tmr_counter_value_set(tmr_x, 0);
        break;
    default:
        result = -RT_ENOSYS;
        break;
    }

    return result;
}


static const struct rt_pulse_encoder_ops _ops =
{
    .init = at32_pulse_encoder_init,
    .get_count = pulse_encoder_get_count,
    .clear_count = pulse_encoder_clear_count,
    .control = pulse_encoder_control,
};

int hw_at32_pulse_encoder_init(void)
{
    int i;
    int result;

    result = RT_EOK;
    for (i = 0; i < sizeof(at32_pulse_encoder_obj) / sizeof(at32_pulse_encoder_obj[0]); i++)
    {
        at32_pulse_encoder_obj[i].pulse_encoder.type = AB_PHASE_PULSE_ENCODER;
        at32_pulse_encoder_obj[i].pulse_encoder.ops = &_ops;

        if (rt_device_pulse_encoder_register(&at32_pulse_encoder_obj[i].pulse_encoder, at32_pulse_encoder_obj[i].name, at32_pulse_encoder_obj[i].tmr_handler) != RT_EOK)
        {
            LOG_E("%s register failed", at32_pulse_encoder_obj[i].name);
            result = -RT_ERROR;
        }
    }

    return result;
}
INIT_BOARD_EXPORT(hw_at32_pulse_encoder_init);

#endif
