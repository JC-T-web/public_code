#ifndef __DEBUG_H
#define __DEBUG_H

#include "stm32f4xx_hal.h"

#include "usart.h"



#define UART1_RX_SIZE 2048  //缓冲区大小
#define UART1_RX_MAX 256    //单次接收最大量
#define NUM 10


typedef struct {
    uint8_t * start;
    uint8_t * end;
}UARTx_RXBuffptr_CB;

typedef struct {
    uint16_t RxCounter; 
    UARTx_RXBuffptr_CB Uart1RxbuffPtr[NUM];
    UARTx_RXBuffptr_CB *RxDataIn; 
    UARTx_RXBuffptr_CB *RxDataOut; 
    UARTx_RXBuffptr_CB *RxDataEnd;
}UART1_RX_CB;


extern  uint8_t  g_uart1_flag;
extern uint8_t rx_data1[1024];
extern uint8_t RxDataBuff[UART1_RX_SIZE];


void DWT_Init(void);
uint32_t DWT_GetMicros(void);
void  Error_Handle_Info(char* info,char *file,uint32_t line);
void Uart1_Printf(char *fmt,...);

void Debug_Init(void);
void UART1_RxData_Dispose();
void UART1_RxEventCallback();
#endif