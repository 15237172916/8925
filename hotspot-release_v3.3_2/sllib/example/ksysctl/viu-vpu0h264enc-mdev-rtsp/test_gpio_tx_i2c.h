#ifndef  TEST_GPIO_TX_I2C_H
#define  TEST_GPIO_TX_I2C_H

//*****TEST GPIO_i2c
#define  I2C_SCL     100 
#define  I2C_SDA     101

void i2c_gpio_init();

void  gpio_i2c_start(void);
void  gpio_i2c_end(void);
void gpio_i2c_write(unsigned char addr, unsigned char* buf, int len);
void gpio_i2c_read(unsigned char addr, unsigned char* buf, int len);
unsigned char i2c_ack_check(unsigned char ctrl_byte);
unsigned char gpio_i2c_read_byte(void);
void gpio_i2c_write_byte(unsigned char byte);
void gpio_i2c_send_nack(void);
int gpio_i2c_send_ack(void);
unsigned char gpio_i2c_read_ack(void);
#endif

