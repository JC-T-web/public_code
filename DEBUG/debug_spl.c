#include "debug_spl.h"
#include <stdarg.h>
#include <string.h>
#include "stdio.h"


USART_Handle usart1_t;

static void usart1_config(uint32_t bound) 
{
// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE); // 使能USART1，GPIOA时钟

	// USART1_TX GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIOA.9

	// USART1_RX GPIOA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIOA.10

	// USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound; // 串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
	USART_Init(USART1, &USART_InitStructure); // 初始化串口1

    // 添加NVIC配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能IRQ通道
    NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启串口接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  //开启串口空闲中断
	
	USART_Cmd(USART1, ENABLE); // 使能串口1
}


static void Serial_Rev(USART_Handle *t)
{
	if(USART_GetITStatus(t->USARTx,USART_IT_RXNE) != RESET) //接收中断触发
	{		
		if(t->rev_len < t->rev_max)
		{
			//读DR把数据一个字节一个字节读出，运行这个会自动清除中断标志位，	  
			t->rev_buff[t->rev_len++] = USART_ReceiveData(t->USARTx);			
		}
		else //一次性接收大于150个字节，数组从头开始接收
		{
			USART_ReceiveData(t->USARTx);              //清除空闲标志
			t->rev_len = 0;
		}
	}	
	if(USART_GetITStatus(t->USARTx, USART_IT_IDLE) != RESET) //空闲中断触发
	{	
		// 1. 清除空闲中断标志（软件清除）
        USART_ClearITPendingBit(t->USARTx, USART_IT_IDLE);
        // 2. 必须读取USART_DR寄存器（硬件清除）
        volatile uint16_t temp = t->USARTx->DR;  // 读取DR后，IDLE标志才会真正清除
		
		 //USART_ReceiveData(t->USARTx);
		 t->rev_flag = 1;
		 if(t->rev_printf == 1)
		 {
		   for(uint16_t i=0;i<t->rev_len;i++)
		   {
			   Serial_Printf(t,"%c",t->rev_buff[i]);
		   }
	     }
		 sscanf((const char *)t->rev_buff,"device=%hhu,data=%d", &t->device_id, &t->data_value);
		 t->rev_len = 0;
    }
		
	 
	
}


void USART1_IRQHandler(void)  //串口1中断服务程序
{
	Serial_Rev(&usart1_t);
}


void Serial_Printf(USART_Handle *t,char* fmt, ...) 
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)t->tx_buff, fmt, ap);
    va_end(ap);
    char len = strlen((const char *)t->tx_buff);
	USART_ClearFlag(t->USARTx, USART_FLAG_TC); //不加的话第一个字符会丢失
	for (int i = 0; i<len; i++) 
	{
		USART_SendData(t->USARTx, (uint8_t)t->tx_buff[i]);
		while (USART_GetFlagStatus(t->USARTx, USART_FLAG_TC) == RESET);
	}
}


void Serial_Init(USART_Handle *p,USART_TypeDef *USARTx,uint32_t bound,uint8_t rev_printf)
{
	p->rev_printf = rev_printf;
	p->rev_max = 128;
	p->device_id = 0;
	p->rev_flag = 0;
	p->rev_len = 0;
	p->data_value = 0;
	p->USARTx = USARTx;
	usart1_config(bound);
	
//	p->serial_config = usart1_config;
//	p->serial_config(p->bound);
}
