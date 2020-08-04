#include "test_gpio_rx_i2c.h"
#include <pthread.h>

#ifdef MUTEX_IIC
extern pthread_mutex_t mutex_iic;
#endif
void  digit_led_writebyte(unsigned char DeviceAddr, unsigned char pBuffer)
{
    //printf("digit_led_writebyte \n");
#ifdef MUTEX_IIC
	pthread_mutex_lock(&mutex_iic);
#endif
	gpio_i2c_start();

	gpio_i2c_write_byte(DeviceAddr);    // 1.发送数据命令(01001000)
    if (1 == gpio_i2c_read_ack())
	{
		printf("read ack error \n");
	}

	gpio_i2c_write_byte(pBuffer);   // 1.发送数据命令(01001000)
	if (1 == gpio_i2c_read_ack())
	{
		printf("read ack error \n");
	}

	gpio_i2c_end();
#ifdef MUTEX_IIC
	pthread_mutex_unlock(&mutex_iic);
#endif
}
