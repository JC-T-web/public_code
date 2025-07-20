#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"  
typedef struct{
	uint8_t  tx_buff[256];
	uint8_t  rev_buff[256];
	uint8_t  rev_max;  //单次接收最大值
	uint8_t  rev_printf;  //开启后可以打印接收到的数据
	uint8_t  device_id;
	uint8_t  rev_flag; //置1表示接收完成
	uint16_t rev_len;
    uint32_t data_value;
	USART_TypeDef *USARTx;
	void (*serial_config)(uint32_t bound);
}USART_Handle;

void Serial_Init(USART_Handle *p,USART_TypeDef *USARTx,uint32_t bound,uint8_t rev_printf);
void Serial_Printf(USART_Handle *t,char* fmt, ...);
extern USART_Handle usart1_t;
#endif
