#include <rtthread.h>
#include <rtdevice.h>
//
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void input_capture(void)
{
    rt_device_t dev = rt_device_find("inputcapture4");
    if (dev)
    {
        rt_device_open(dev, RT_DEVICE_OFLAG_RDONLY);

        struct rt_inputcapture_data data;
        while (1)
        {
            if (rt_device_read(dev, 0, &data, 1) > 0)
            {
                rt_kprintf("Pulse width: %u us, Level: %s\n",
                           data.pulsewidth_us,
                           data.is_high ? "High" : "Low");
            }
            rt_thread_mdelay(500);
        }
    }
}
MSH_CMD_EXPORT(input_capture, input capture);
