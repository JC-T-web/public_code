# public_code

## 2025.7.20

1.添加标准库下的debug使用

```c
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
```

