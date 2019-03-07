#ifndef _SILAN_LED_H_
#define _SILAN_LED_H_

#define LED_IOC_MAGIC 'k' 
#define LED0_SET_VAL		_IOW(LED_IOC_MAGIC, 0, int)
#define LED1_SET_VAL		_IOW(LED_IOC_MAGIC, 1, int)
#define LED_SET_ALL			_IOW(LED_IOC_MAGIC, 2, int)
#define LED_CLEAR_ALL		_IOW(LED_IOC_MAGIC, 3, int)

#define SILAN_LED0		(3*32 + 27)	
#define SILAN_LED1		(3*32 + 26)

#endif
