#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define I2C_RETRIES 0x0701
#define I2C_TIMEOUT 0x0702
#define I2C_RDWR 0X0707

#define	I2C_M_WR 0
struct i2c_msg {
	unsigned short addr;	
	unsigned short flags;
#define I2C_M_TEN		0x0010	
#define I2C_M_RD		0x0001	
#define I2C_M_NOSTART		0x4000	
#define I2C_M_REV_DIR_ADDR	0x2000	
#define I2C_M_IGNORE_NAK	0x1000	
#define I2C_M_NO_RD_ACK		0x0800	
#define I2C_M_RECV_LEN		0x0400	
	unsigned short len;		
	unsigned char *buf;		
};

struct i2c_rdwr_ioctl_data {
	struct i2c_msg *msgs;	
	int nmsgs;			
};

static unsigned char add=0x49; //hotspot board
static int fd; //i2c 

struct i2c_rdwr_ioctl_data e2prom_data;

int WriteIICRegister(unsigned char add,unsigned int  reg, unsigned char data)
{
	int ret;
	e2prom_data.nmsgs = 1;
	//IIC write
	(e2prom_data.msgs[0]).len = 0x2;
	(e2prom_data.msgs[0]).addr = add/2;
	(e2prom_data.msgs[0]).flags = I2C_M_WR;
	(e2prom_data.msgs[0]).buf[0] =reg;
	(e2prom_data.msgs[0]).buf[1] =data;

	ret = ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret < 0)
	{
		perror("ioctl error2");
		return -1;
	}

	return 0;
}

int ReadIICRegister(unsigned char add, unsigned int reg, unsigned char * data)
{
	//IIC read
	int ret;
	e2prom_data.nmsgs = 2;
	(e2prom_data.msgs[0]).len = 1;
	(e2prom_data.msgs[0]).addr = add/2;   //
	(e2prom_data.msgs[0]).flags = I2C_M_WR;
	(e2prom_data.msgs[0]).buf[0] =reg;
	(e2prom_data.msgs[1]).len = 1;
	(e2prom_data.msgs[1]).addr = add/2;  //
	(e2prom_data.msgs[1]).flags = I2C_M_RD;
	(e2prom_data.msgs[1]).buf = data;

	ret = ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret < 0)
	{
		perror("ioctl error2");
		return -1;
	}

	return 0;
}

static int i2c_init(void)
{
	fd = open("/dev/i2c-1",O_RDWR);

	if(fd < 0)
	{
		perror("open /dev/i2c-1 failed\n");
		return -1;
	}
	e2prom_data.nmsgs = 2;
	e2prom_data.msgs = (struct i2c_msg*)malloc(e2prom_data.nmsgs*sizeof(struct i2c_msg));
	if(!e2prom_data.msgs)
	{
		perror("malloc error\n");
		return -1;
	}

	(e2prom_data.msgs[0]).buf = (unsigned char*)malloc(2);
	if(!(e2prom_data.msgs[0]).buf)
	{
		perror("malloc error\n");
		return -1;
	}

	return 0;
}

static void i2c_uinit(void)
{

	free(e2prom_data.msgs[0].buf); 
	free(e2prom_data.msgs); 
	close(fd);

}

//ccx add 2018.8.7
int iic_display_test(int data)
{
	
	int i = 0,j = 0, ret = -1;
	unsigned char tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};  //0~F
	
	//(1)初始化
	ret = i2c_init();
	if(ret<0)
	{
		printf("i2c_init error \n");
		return -1;
	}
	//(2)发写命令	
	ret = WriteIICRegister(add,0x0,0x1);
	if(ret<0)
	{
		printf("WriteIICRegister error ====1 \n");
		return -1;
	}
	//(3)写数据		
	if(data<10 )		
	{
		WriteIICRegister(0x68+4,tab[data%16],0x0); //个位
	}
	else
	{
		i=data/10;
		j=data%10;	
		//WriteIICRegister(0x68,0x0,tab[i%16]); //十位
		WriteIICRegister(0x68+2,tab[i%16],0x0); //十位
		WriteIICRegister(0x68+4,tab[j%16],0x0); //个位
		//WriteIICRegister(0x68+2,0x0,tab[j%16]|0x80);  //0x80 表示的是小数点
	}
	

	

/*
//(3)写数据
for(i=0; i<16; i++)
{
	
	if(tab[i] == data)
	{
		ret = WriteIICRegister(0x68+6,0x0,tab[i%16]|0x80);    //1
		ret = WriteIICRegister(0x68,0x0,tab[i%16]|0x80);    //2
		ret = WriteIICRegister(0x68+2,0x0,tab[i%16]|0x80);  //3
		ret = WriteIICRegister(0x68+4,0x0,tab[i%16]|0x80);  //4
		if(ret<0)
		{
			printf("WriteIICRegister error ====== 3 \n");
			return -1;
		}
		i++;
		sleep(1);
	}	
}
*/	
	return 0;
}
