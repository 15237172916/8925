#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include "sharemem.h"
#include "crc.h"
#include "test_gpio_rx_i2c.h"
#include "EEPROM.h"
/******************24C08 address space***********************/
//deivce	A2 pin	block	r/w		address
//1010 		1 		00 		1/0 	0-255
//1010 		1 		01 		1/0 	256-512
//1010 		1 		10 		1/0 	513-768
//1010 		1	 	11 		1/0 	769-1024
/***********************************************************/
extern pthread_mutex_t mutex_iic;

addr_ctrl_byte_struct get_eigenbytes(uint16_t address_in_chip)
{
	addr_ctrl_byte_struct cbs;
	if((address_in_chip<0x00) || (address_in_chip>0x3FF))
	{
		printf("Cross-border error! The range of address_in_chip is 0x000-0x3FF.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		if((address_in_chip>=0x00) && (address_in_chip<0x100)) //256
		{
			//cbs.ctrl_byte  = 0xA0; //1010 0000
			cbs.ctrl_byte  = 0xA8; //1010 1000
			cbs.word_addr  = address_in_chip;
		}
		else if((address_in_chip>=0x100) && (address_in_chip<0x200)) //512
		{
			//cbs.ctrl_byte  = 0xA2; //1010 0010
			cbs.ctrl_byte  = 0xAA; //1010 1010
			cbs.word_addr  = address_in_chip%0x100;
		}
		else if((address_in_chip>=0x200) && (address_in_chip<0x300)) //768
		{
			//cbs.ctrl_byte  = 0xA4; //1010 0100
			cbs.ctrl_byte  = 0xAC; //1010 1100
			cbs.word_addr  = address_in_chip%0x200;
		}
		else
		{
			//cbs.ctrl_byte  = 0xA6; //1010 0110
			cbs.ctrl_byte  = 0xAE; //1010 1110
			cbs.word_addr  = address_in_chip%0x300; 
		}
	}
	return cbs;
}

// write one byte to e2prom
int i2c_byte_write(uint8_t ctrl_byte,uint8_t word_addr,uint8_t data_byte)
{
	int ret = 0;
	gpio_i2c_start();

	gpio_i2c_write_byte(ctrl_byte);
	if (1 == gpio_i2c_read_ack())
	{
		ret = -1;
		printf("read ack error \n");
	}
	
	gpio_i2c_write_byte(word_addr);
	if (1 == gpio_i2c_read_ack())
	{
		ret = -1;
		printf("read ack error \n");
	}

	gpio_i2c_write_byte(data_byte);
	if (1 == gpio_i2c_read_ack())
	{
		ret = -1;
		printf("read ack error \n");
	}
	
	gpio_i2c_end();

	return ret;
}

// write bytes to e2prom (page write)
int i2c_page_write(uint8_t ctrl_byte,uint8_t word_addr,uint8_t *source_data_addr,uint8_t data_len)
{
	int ret = 0;
	uint8_t i;
	if (data_len<=0)
	{
		printf("i2c_page_write: data_len should be a positive number.\n");
		ret = -1;
	}
	else
	{
		gpio_i2c_start();

		gpio_i2c_write_byte(ctrl_byte);
		if(1 == gpio_i2c_read_ack())
		{
			ret = -1;
			printf("read ack error \n");
		}

		gpio_i2c_write_byte(word_addr);
		if(1 == gpio_i2c_read_ack())
		{
			ret = -1;
			printf("read ack error \n");
		}

		for(i=0;i<data_len;i++)
		{
			gpio_i2c_write_byte(*(source_data_addr+i));
			if(1 == gpio_i2c_read_ack())
			{
				ret = -1;
				printf("read ack error \n");
			}
		}
		
		gpio_i2c_end();
	}
	//printf("i2c_page_write finished.\n");
	return ret;
}

// read current address
uint8_t i2c_current_addr_read(uint8_t ctrl_byte)
{
	uint8_t data;
	gpio_i2c_start();
	gpio_i2c_write_byte(ctrl_byte);
	if(gpio_i2c_read_ack() == 0)
	{
		data = gpio_i2c_read_byte();
		gpio_i2c_send_nack();
		gpio_i2c_end();
		return data;
	}
	else 
		return 0;
}

// read one byte from eeprom
int i2c_rand_read(uint8_t ctrl_byte,uint8_t word_addr)
{
	//printf("ctrl_byte: %x, word_addr: %x \n", ctrl_byte, word_addr);
	int data = 0;
	gpio_i2c_start();

	gpio_i2c_write_byte(ctrl_byte);
	if (1 == gpio_i2c_read_ack())
	{
		return -1;
		printf("read ack error \n");
	}

	gpio_i2c_write_byte(word_addr);
	if (1 == gpio_i2c_read_ack())
	{
		return -1;
		printf("read ack error \n");
	}

	gpio_i2c_start();

	gpio_i2c_write_byte((ctrl_byte+1));
	if (1 == gpio_i2c_read_ack())
	{
		return -1;
		printf("read ack error \n");
	}

	data = gpio_i2c_read_byte();
	gpio_i2c_send_nack();
	gpio_i2c_end();
	//printf("%x", data);
	return data;
}

// read sequential bytes from eeprom 
// it is the ADDRESS that is transferred, but not DATA!
//uint32_t i2c_sequential_read(uint8_t ctrl_byte,uint8_t word_addr,uint16_t data_num)
int i2c_sequential_read(uint8_t ctrl_byte,uint8_t word_addr,uint16_t data_len,uint8_t *data_addr_in_master_mem)
{
	int ret = 0;
	uint16_t i=0;
	
	//printf("ctrl_byte: %d, word_addr: %d, data_len: %d, data: %d \n", ctrl_byte, word_addr, data_len, *data_addr_in_master_mem);
	if(data_len<=0)
	{
		printf("i2c_sequential_read: data_len should be a positive number.\n");
		return -1;
	}
	else
	{
		gpio_i2c_start();

		gpio_i2c_write_byte(ctrl_byte);	//write device address and write cmd
		if (1 == gpio_i2c_read_ack())
		{
			ret = -1;
			printf("read ack error \n");
		}
		
		gpio_i2c_write_byte(word_addr); //write register address
		if (1 == gpio_i2c_read_ack())
		{
			ret = -1;
			printf("read ack error \n");
		}
		
		gpio_i2c_start(); //start signal

		gpio_i2c_write_byte((ctrl_byte+1)); //write read cmd
		if (1 == gpio_i2c_read_ack())
		{
			ret = -1;
			printf("read ack error \n");
		}
		
		for(i=0;i<data_len-1;i++)
		{
			*(data_addr_in_master_mem + i) = gpio_i2c_read_byte();
			gpio_i2c_send_ack();		// master send ACK
		}
		
		*(data_addr_in_master_mem + data_len) = gpio_i2c_read_byte();
		gpio_i2c_send_nack();		// master send NACK

		gpio_i2c_end();
	}
	//printf("i2c_sequential_read finished.\n");
	return ret;
}

// memory_write within one single block
int i2c_write_within_block(uint8_t ctrl_byte,uint8_t word_addr,uint8_t *source_data_addr,uint16_t data_len)
{
	int ret = 0;
	uint8_t page_size = 0x10;
	uint16_t bolck_size = 0x100;
	uint8_t extra_page = 0;
	uint8_t i = 0;
	uint8_t page_offset = word_addr % page_size;
	uint8_t len_left = page_size - page_offset;
	
	// beyond the scope of the current block
	if ((word_addr + data_len) > bolck_size)
	{
		printf("(B1)i2c_write_within_block:beyond the scope of the current block, JUST RETURN.");
		ret = -1;
	}
	else// within the current block
	{
		if(data_len <= len_left)
		{
			printf("(B2)i2c_write_within_block:within the current page.\n");
			ret = i2c_page_write(ctrl_byte,word_addr,source_data_addr,data_len); // pointer as function parameter?
			while(i2c_ack_check(ctrl_byte));
		}
		else
		{
			//printf("(B3)i2c_write_within_block:within the current block but beyond the current page.\n");
			if( (data_len - len_left)%page_size != 0 )
				extra_page = floor( (data_len - len_left)/(float)page_size+1 );
			else
				extra_page = (data_len - len_left)/(float)page_size;
			
			//printf("extra_page = %d.\n data_len = %d.\n len_left = %d.\n",extra_page,data_len,len_left);
			// first, write the current page
			ret = i2c_page_write(ctrl_byte,word_addr,source_data_addr,len_left);
			while(i2c_ack_check(ctrl_byte));
			// then, write the following complete page except the last maybe-incomplete page
			for (i=1;i<extra_page;i++)
			{
				ret = i2c_page_write(ctrl_byte,(word_addr+len_left+(i-1)*page_size),(source_data_addr+len_left+(i-1)*page_size),page_size);
				while(i2c_ack_check(ctrl_byte));
			}
			// finally, write the last maybe-incomplete page
			ret = i2c_page_write(ctrl_byte,(word_addr+len_left+(extra_page-1)*page_size),(source_data_addr+len_left+(extra_page-1)*page_size),
			(data_len-len_left-(extra_page-1)*page_size));
			while(i2c_ack_check(ctrl_byte));
		}
	}
	//printf("i2c_write_within_block finished.\n");
	return ret;
}


/******************** the following are 2 highest API:Write/Read in chip********************/

// memory_write within one 24c08 Chip
int i2c_write_within_chip(uint16_t address_in_chip,uint8_t *source_data_addr,uint16_t data_len)
{
	int ret = 0;
	uint16_t block_size = 0x100;
	uint8_t extra_block = 0;
	uint8_t i = 0;
	
	addr_ctrl_byte_struct Scb;
	Scb = get_eigenbytes(address_in_chip);
	uint8_t ctrl_byte = Scb.ctrl_byte;
	//printf("%x \n", Scb.ctrl_byte);
	uint8_t word_addr = Scb.word_addr;
	//printf("%x \n", Scb.word_addr);
	uint8_t left_block_num = 4 - ((ctrl_byte & 0x06) >> 1);
	uint16_t total_mem_left = 1024-256*(4-left_block_num)-word_addr;
	uint16_t current_block_mem_left = block_size-word_addr;
	
	// do not beyond current block
	if ((word_addr+data_len) <= block_size)
	{
		//printf("(C1)i2c_write_within_chip:do not beyond current block.\n");
		ret = i2c_write_within_block(ctrl_byte,word_addr,source_data_addr,data_len);
	}
	// the chip itself is not large enough
	// just write the chip full and abandon the left part
	else if (data_len > total_mem_left)
	{	
		//printf("(C2)i2c_write_within_chip:the chip itself is not large enough.\n");
		data_len = total_mem_left;
		
		if( (data_len-current_block_mem_left)%block_size != 0 )
			extra_block = floor((data_len-current_block_mem_left)/(float)block_size + 1);
		else
			extra_block = (data_len-current_block_mem_left)/(float)block_size;
		
		// first, write the current block
		ret = i2c_write_within_block(ctrl_byte,word_addr,source_data_addr,current_block_mem_left);
		// just write the chip full and abandon the left part
		for (i=1;i <= extra_block;i++)
		{
			ret = i2c_write_within_block(ctrl_byte+0x02*i,0x00,source_data_addr+current_block_mem_left+(i-1)*block_size,block_size);
		}
	}
	// occupy more than one block of memory but not beyond chip's memory range
	else
	{
		//printf("(C3)i2c_write_within_chip:occupy more than one block of memory but not beyond chip's memory range.\n");
		
		// judge whether extra_block is intergals or float
		if( (data_len-current_block_mem_left)%block_size != 0 )
			extra_block = floor((data_len-current_block_mem_left)/(float)block_size + 1);
		else
			extra_block = (data_len-current_block_mem_left)/(float)block_size;
		
		//printf("extra_block = %d.\n",extra_block);
		// first, write the current block
		ret = i2c_write_within_block(ctrl_byte,word_addr,source_data_addr,current_block_mem_left);
		// then, write the following complete block except the last maybe-incomplete block
		for (i=1;i<extra_block;i++)
		{
			ret = i2c_write_within_block(ctrl_byte+0x02*i,0x00,source_data_addr+current_block_mem_left+(i-1)*block_size,block_size);
		}
		// finally, write the last maybe-incomplete block
		ret = i2c_write_within_block(ctrl_byte+0x02*extra_block,0x00,
			source_data_addr+current_block_mem_left+(extra_block-1)*block_size,
			data_len-current_block_mem_left-(extra_block-1)*block_size);
	}
	//printf("i2c_write_within_chip finished.\n");
}

int i2c_read_within_chip(uint16_t address_in_chip,uint8_t *data_addr_in_master_mem,uint16_t data_len)
{
	int ret = 0;
	uint16_t block_size = 0x100; //256
	uint8_t extra_block = 0;
	uint8_t i = 0;
	
	addr_ctrl_byte_struct Scb;
	Scb = get_eigenbytes(address_in_chip);
	uint8_t ctrl_byte = Scb.ctrl_byte;
	uint8_t word_addr = Scb.word_addr;
	uint8_t left_block_num = 4 - ((ctrl_byte & 0x06) >> 1);
	uint16_t total_mem_left = 1024-256*(4-left_block_num)-word_addr;
	uint16_t current_block_mem_left = block_size-word_addr;
	
	// donot beyond current block
	if ((word_addr+data_len) <= block_size)
	{
		printf("(C1)i2c_read_within_chip:do not beyond current block.\n");
		ret = i2c_sequential_read(ctrl_byte,word_addr,data_len,data_addr_in_master_mem);
	}
	// the chip itself is not large enough
	// just read the chip full and abandon the left part
	else if (data_len > total_mem_left)
	{
		//printf("(C2)i2c_read_within_chip:the chip itself is not large enough.\n");
		data_len = total_mem_left;
		
		if( (data_len-current_block_mem_left)%block_size != 0 )
			extra_block = floor((data_len-current_block_mem_left)/(float)block_size + 1);
		else
			extra_block = (data_len-current_block_mem_left)/(float)block_size;
		
		//printf("data_len=%d\n current_block_mem_left=%d\n extra_block=%d\n",data_len,current_block_mem_left,extra_block);
		
		// first, read the current block
		ret = i2c_sequential_read(ctrl_byte,word_addr,current_block_mem_left,data_addr_in_master_mem);
		// then, read the following complete block till the end
		for (i = 1;i <= extra_block;i++)
		{
			ret = i2c_sequential_read(ctrl_byte+0x02*i,0x00,block_size,(data_addr_in_master_mem+current_block_mem_left+(i-1)*block_size));
		}
	}
	// occupy more than one block of memory but not beyond chip's memory range
	else
	{
		//printf("(C3)i2c_read_within_chip:occupy more than one block of memory but not beyond chip's memory range.\n");
		
		if( (data_len-current_block_mem_left)%block_size != 0 )
			extra_block = floor((data_len-current_block_mem_left)/(float)block_size + 1);
		else
			extra_block = (data_len-current_block_mem_left)/(float)block_size;
		
		//printf("data_len=%d\n current_block_mem_left=%d\n extra_block=%d\n",data_len,current_block_mem_left,extra_block);
		// first, read the current block
		ret = i2c_sequential_read(ctrl_byte,word_addr,current_block_mem_left,data_addr_in_master_mem);
		// then, read the following complete block till the end
		for (i = 1;i < extra_block;i++)
		{
			ret = i2c_sequential_read(ctrl_byte+0x02*i,0x00,block_size,(data_addr_in_master_mem+current_block_mem_left+(i-1)*block_size));
		}
		// finally, write the last maybe-incomplete block
		ret = i2c_sequential_read(ctrl_byte+0x02*extra_block,0x00,data_len-current_block_mem_left-(extra_block-1)*block_size,
		(data_addr_in_master_mem+current_block_mem_left+(extra_block-1)*block_size));
	}
	//printf("i2c_read_within_chip finished.\n");
	return ret;
}

int InitShareMemFromE2prom(SHARE_MEM *pShareMemory)
{
	int ret = 0;

	pthread_mutex_lock(&mutex_iic);
	//SHARE_MEM *pRead = malloc(sizeof(SHARE_MEM) * sizeof(char));
	printf("---------- eeprom read start ----------\n");
	printf("buffer size : %d \n", sizeof(SHARE_MEM));
	
	ret = i2c_read_within_chip(0x00, pShareMemory, sizeof(SHARE_MEM));
	if (ret >= 0)
	{
		printf("uuid: %d \n", pShareMemory->uuid);
		printf("ip : %s \n", pShareMemory->sm_eth_setting.strEthIp);
		printf("gateway : %s \n", pShareMemory->sm_eth_setting.strEthGateway);
		printf("multicast : %s \n", pShareMemory->sm_eth_setting.strEthMulticast);
		printf("---------- eeprom read finish ----------\n");
		if (0 != strncmp(pShareMemory->sm_eth_setting.strEthMask, "255.255.255.0", 13))
		{
			printf("eeprom data error \n");
			return 2;
		}
	}
	else
	{
		printf("---------- eeprom read error ----------\n");
	}
	
	pthread_mutex_unlock(&mutex_iic);

	return ret;
}

int WriteConfigIntoE2prom(SHARE_MEM *pShareMemory)
{
	int ret = 0;

	pthread_mutex_lock(&mutex_iic);
	printf("------ eeprom write size: %d -------\n", sizeof(SHARE_MEM));
	printf("------ eeprom write start --------\n");
	ret = i2c_write_within_chip(0x00, pShareMemory, sizeof(SHARE_MEM));
	printf("------ eeprom write finish -------- \n");
	pthread_mutex_unlock(&mutex_iic);

	return ret;
}

#if 0
void test(void)
{

	char rbuffer[sizeof(SHARE_MEM)] = {0};
	char w = 'A';
	char r;
	int i = 0;
	//i2c_gpio_init();
	sleep(1);
	int checksum = 0;
	//i2c_init();
	SHARE_MEM *pRead = malloc(sizeof(SHARE_MEM) * sizeof(char));
	while (1)
	{
		#if 1
		sleep(1);
		//i2c_sequential_read(0xA8, 0x00, sizeof(SHARE_MEM), pRead);
		printf("---------- eeprom read start ----------\n");
		printf("buffer size : %d \n", sizeof(SHARE_MEM));
		//i2c_read_within_chip(0x00, pRead, sizeof(SHARE_MEM));
		
		i2c_read_within_chip(0x00, pRead, sizeof(SHARE_MEM));
		printf("---------- eeprom read finish ----------\n");
		if (checksum == sum_check(pRead, sizeof(SHARE_MEM)))
		{
			printf("uuid: %d \n", pRead->uuid);
			printf("ip : %s \n", pRead->sm_eth_setting.strEthIp);
			printf("gateway : %s \n", pRead->sm_eth_setting.strEthGateway);
			printf("multicast : %s \n", pRead->sm_eth_setting.strEthMulticast);
		}
		else
		{
			printf("uuid: %d \n", pRead->uuid);
			printf("ip : %s \n", pRead->sm_eth_setting.strEthIp);
			printf("gateway : %s \n", pRead->sm_eth_setting.strEthGateway);
			printf("multicast : %s \n", pRead->sm_eth_setting.strEthMulticast);
			//printf("rbuffer : %s \n", rbuffer);
			printf("e2prom checksum error \n");
		}

		checksum = sum_check(share_mem, sizeof(SHARE_MEM));

		sleep(1);
		

		
		//while (1)
			sleep(1);
		#endif
	}
	//free(pRead);

	return 0;
}
#endif



