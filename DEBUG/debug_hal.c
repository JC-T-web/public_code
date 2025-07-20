#include "debug.h"
#include <stdarg.h>
#include <string.h>
#include "stdio.h"


static uint8_t UART1_TX_BUF[512];
UART1_RX_CB Uart1Rx;
uint8_t RxDataBuff[UART1_RX_SIZE];

void Uart1_Printf(char *fmt,...)
{
    va_list ap;
    uint16_t len;
    va_start(ap, fmt);
    vsprintf((char *)UART1_TX_BUF, fmt, ap);
    va_end(ap);

    len = strlen((const char *)UART1_TX_BUF);
    HAL_UART_Transmit(&huart1, (uint8_t*)UART1_TX_BUF,len,100);
}

void Debug_Init(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxDataBuff, UART1_RX_MAX+1);
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    Uart1Rx.RxDataIn = &Uart1Rx.Uart1RxbuffPtr[0];
    Uart1Rx.RxDataOut = &Uart1Rx.Uart1RxbuffPtr[0];
    Uart1Rx.RxDataEnd = &Uart1Rx.Uart1RxbuffPtr[NUM-1];
    Uart1Rx.RxCounter = 0;
    Uart1Rx.RxDataIn->start = RxDataBuff;
}

// 不定长数据接收完成回调函数
void UART1_RxEventCallback()
{
    Uart1Rx.RxCounter +=(UART1_RX_MAX+1) - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    if (Uart1Rx.RxCounter >= UART1_RX_SIZE)
    {
        Uart1Rx.RxCounter = 0;
    }

    Uart1Rx.RxDataIn->end = &RxDataBuff[Uart1Rx.RxCounter-1];

    Uart1Rx.RxDataIn++;
    if(Uart1Rx.RxDataIn == Uart1Rx.RxDataEnd)
    {
        Uart1Rx.RxDataIn = &Uart1Rx.Uart1RxbuffPtr[0];
    }

    if(UART1_RX_SIZE - Uart1Rx.RxCounter > UART1_RX_MAX)
    {
        Uart1Rx.RxDataIn->start = &RxDataBuff[Uart1Rx.RxCounter];
    }
    else
    {
        Uart1Rx.RxDataIn->start = RxDataBuff;
        Uart1Rx.RxCounter = 0;
    }
    HAL_UART_DMAStop(&huart1);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Uart1Rx.RxDataIn->start, UART1_RX_MAX+1);
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    HAL_UART_DMAResume(&huart1);
}


void UART1_RxData_Dispose()
{
    if(Uart1Rx.RxDataOut != Uart1Rx.RxDataIn)
    {
       Uart1_Printf("%p\r\n",Uart1Rx.RxDataOut->start);
       uint16_t len = Uart1Rx.RxDataOut->end - Uart1Rx.RxDataOut->start + 1;
       Uart1_Printf("this time get %d bytes data\r\n",len);
       for(uint16_t i=0;i<len;i++)
       {
           Uart1_Printf("%c",Uart1Rx.RxDataOut->start[i]);
       }
//       Uart1_Printf("\r\n\r\n");
        Uart1Rx.RxDataOut++;
      //  Uart1_Printf("指针地址 = %p\n",Uart1Rx.RxDataOut);
        if(Uart1Rx.RxDataOut == Uart1Rx.RxDataEnd)
        {
            Uart1Rx.RxDataOut = &Uart1Rx.Uart1RxbuffPtr[0];
        }
    }
}


void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; 
    DWT->CYCCNT = 0;                                
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            
}

uint32_t DWT_GetMicros(void)
{
    return DWT->CYCCNT / (SystemCoreClock / 1000000);  // ??????????
}
/**
 * @param 错误信息定位   
 * @param level  
 * @param file   
 * @param line   
 *
 * Error_Handle_Info("AT24C04 Init Error",__FILE__,__LINE__);
 */
void  Error_Handle_Info(char* info,char *file,uint32_t line)
{
    if(line == 0) return;
	Uart1_Printf("%s <%s><%d>\r\n",info,file,line);
}