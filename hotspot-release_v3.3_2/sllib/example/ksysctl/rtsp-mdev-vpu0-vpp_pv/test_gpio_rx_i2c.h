#ifndef  TEST_GPIO_RX_I2C_H
#define  TEST_GPIO_RX_I2C_H

//*****TEST GPIO_i2c
#define  I2C_SCL     100 
#define  I2C_SDA     101

void i2c_gpio_init(void);
void  gpio_i2c_start(void);
void  gpio_i2c_end(void);
void gpio_i2c_send_ack(void);
void gpio_i2c_send_nack(void);
void gpio_i2c_write_byte(unsigned char byte);
unsigned char gpio_i2c_read_ack(void);
unsigned char i2c_ack_check(unsigned char ctrl_byte);
unsigned char gpio_i2c_read_byte(void);
#endif

