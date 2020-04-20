#include "test_gpio_tx_i2c.h"
#include <pthread.h>

extern pthread_mutex_t mutex_iic;

void  digit_led_writebyte(unsigned char DeviceAddr, unsigned char pBuffer)
{
    //printf("digit_led_writebyte \n");
    pthread_mutex_lock(&mutex_iic);
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
    pthread_mutex_unlock(&mutex_iic);
}