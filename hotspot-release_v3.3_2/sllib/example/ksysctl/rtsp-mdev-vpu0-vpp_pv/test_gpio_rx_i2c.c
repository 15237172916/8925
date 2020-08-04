#include <stdio.h>
#include <unistd.h>
#include <sl_gpio.h>

#include "app_rx_io_ctl.h"   //app_rx_io_ctl.h 中有GPIO的操作定义 
#include "test_gpio_rx_i2c.h"

#define TRUE 1
#define FALSE   0
#define TIME 5

void i2c_gpio_init(void)
{
    GPIO_openFd(I2C_SDA);
    GPIO_export(I2C_SDA);
    GPIO_setDir(I2C_SDA, GPIO_OUTPUT);
    GPIO_setValue(I2C_SDA, GPIO_HIG_STA);

    GPIO_openFd(I2C_SCL);
    GPIO_export(I2C_SCL);
    GPIO_setDir(I2C_SCL, GPIO_OUTPUT);
    GPIO_setValue(I2C_SCL, GPIO_HIG_STA);
}

static void delay_us(SL_U16 time)
{
   usleep(TIME);
}

static int get_SDA_level(void)
{
	int value;
	
    GPIO_setDir(I2C_SDA, GPIO_INPUT);
    usleep(TIME);
	GPIO_getValue(I2C_SDA, &value);
	usleep(TIME);
	
	return value;
}


void  gpio_i2c_start(void)
{
    GPIO_setDir(I2C_SDA, GPIO_OUTPUT); //SDA output
    GPIO_setDir(I2C_SCL, GPIO_OUTPUT); //SCL output
    
    GPIO_setValue(I2C_SDA, GPIO_HIG_STA); //SDA = 1
    GPIO_setValue(I2C_SCL, GPIO_HIG_STA); //SCL = 1 
    usleep(TIME);
    GPIO_setValue(I2C_SDA, GPIO_LOW_STA); //SDA = 0
    usleep(TIME);
}

 /** 产生I2C停止信号 **/
void  gpio_i2c_end(void)
{
    GPIO_setDir(I2C_SDA, GPIO_OUTPUT);  //SDA output
    GPIO_setDir(I2C_SCL, GPIO_OUTPUT);  //SCL output
    usleep(TIME);
    GPIO_setValue(I2C_SDA, GPIO_LOW_STA); //SDA = 0
    GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0

    usleep(TIME);
    GPIO_setValue(I2C_SCL, GPIO_HIG_STA); //SCL = 1
    usleep(TIME);
    GPIO_setValue(I2C_SDA, GPIO_HIG_STA); //SDA = 1
    usleep(TIME);
    GPIO_setDir(I2C_SDA, GPIO_INPUT);  //SDA input
    GPIO_setDir(I2C_SCL, GPIO_INPUT);  //SCL input
    usleep(1000);
}


/*
 * return : 
 *      1 : no ack
 *      0 : ack
*/
unsigned char gpio_i2c_read_ack(void) 
{
    unsigned char r;

    GPIO_setDir(I2C_SDA, GPIO_INPUT); //SDA input
    GPIO_setValue(I2C_SCL, GPIO_HIG_STA); //SCL = 1
    usleep(TIME);
    r = get_SDA_level(); //get SDA value
    usleep(TIME);
    GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0
    usleep(TIME);
    
    return r;
}

/* I2C发出ACK信号(读数据时使用) */  
void gpio_i2c_send_ack(void)
{
    GPIO_setDir(I2C_SDA, GPIO_OUTPUT);
    //GPIO_setDir(I2C_SCL, GPIO_OUTPUT);

    GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0
    GPIO_setValue(I2C_SDA, GPIO_LOW_STA); //SDA = 0
    usleep(TIME);
    GPIO_setValue(I2C_SCL, GPIO_HIG_STA); //SCL = 1
    usleep(TIME);
    GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0
    usleep(TIME);
}

void gpio_i2c_send_nack(void)
{
    GPIO_setDir(I2C_SDA, GPIO_OUTPUT); //SDA output
    //GPIO_setDir(I2C_SCL, GPIO_OUTPUT);

    GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0
    GPIO_setValue(I2C_SDA, GPIO_HIG_STA); //SDA = 1
    usleep(TIME);
    GPIO_setValue(I2C_SCL, GPIO_HIG_STA); //SCL = 1
    usleep(TIME);
    GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0
    usleep(TIME);
}

/* I2C字节读 */  
unsigned char gpio_i2c_read_byte(void)  
{  
    int i;
    unsigned char r = 0x00;

    GPIO_setDir(I2C_SDA, GPIO_INPUT); //SDA input
    usleep(TIME);
    for (i=7; i>=0; i--) 
    {
        GPIO_setValue(I2C_SCL, GPIO_HIG_STA); //SCL = 1
        usleep(TIME);
        r = ((r << 1) | get_SDA_level());      //从高位到低位依次准备数据进行读取  
        usleep(TIME);
        GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0
        usleep(TIME);
    }

    return r;  
}

/* I2C字节写 */  
void gpio_i2c_write_byte(unsigned char byte)  
{
    int i;

    GPIO_setDir(I2C_SDA, GPIO_OUTPUT); //SDA output
    GPIO_setDir(I2C_SCL, GPIO_OUTPUT); //SDA output
    usleep(TIME);
    for (i=7; i>=0; i--)
    {
        GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0
        usleep(TIME);
        if (byte & (1 << i)) //从高位到低位依次准备数据进行发送  
        {
            GPIO_setValue(I2C_SDA, GPIO_HIG_STA); //SDA = 1
        }
        else
        {
            GPIO_setValue(I2C_SDA, GPIO_LOW_STA); //SDA = 0
        }
        usleep(TIME);
        GPIO_setValue(I2C_SCL, GPIO_HIG_STA); //SCL = 1
        usleep(TIME);
    }
    GPIO_setValue(I2C_SCL, GPIO_LOW_STA); //SCL = 0
    
    usleep(TIME);
}

unsigned char i2c_ack_check(unsigned char ctrl_byte)
{
	gpio_i2c_start();
	gpio_i2c_write_byte(ctrl_byte);
	if(gpio_i2c_read_ack() == 0)
	{
		//sl_printf("i2c_read_ack() == 0.\n");
		// time delay here is not necessary, just to make waveforms more readable
		delay_us(30);
		//i2c_stop();
		//io_input(SDA);		// set SDA as input

		//io_input(SCL);		// set SCL as input
		return 0;
	}
	else
	{
		//sl_printf("i2c_read_ack() == 1.\n");
		// time delay here is to save computing resource
		delay_us(100);
		//io_input(SDA);		// set SDA as input
		//io_input(SCL);		// set SCL as input
		return 1;
	}
}

