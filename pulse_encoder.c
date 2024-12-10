#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_LEVEL DBG_LOG
#include <rtdbg.h>

#define PULSE_ENCODER_DEV_NAME    "pulse3"

rt_device_t pulse_encoder_dev = RT_NULL;

int pulse_encoder(void)
{
    rt_err_t ret = RT_EOK;
    pulse_encoder_dev = rt_device_find(PULSE_ENCODER_DEV_NAME);
    if (pulse_encoder_dev == RT_NULL)
    {
        return RT_ERROR;
    }

    ret = rt_device_open(pulse_encoder_dev, RT_DEVICE_OFLAG_RDONLY);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", PULSE_ENCODER_DEV_NAME);
        return ret;
    }

    rt_int32_t count = 0;
    while (1)
    {
        rt_thread_mdelay(500);
        rt_device_read(pulse_encoder_dev, 0, &count, 1);
        rt_kprintf("get count %d\n",count);
    }

    rt_device_close(pulse_encoder_dev);
    return ret;
}

