#ifndef __IIC_HAL_H
#define __IIC_HAL_H

#include "stm32f1xx_hal.h"

typedef struct
{
	GPIO_TypeDef * IIC_SDA_PORT;
	GPIO_TypeDef * IIC_SCL_PORT;
	uint16_t IIC_SDA_PIN;
	uint16_t IIC_SCL_PIN;
}IIC_HandleTypeDef;

void IICStart(IIC_HandleTypeDef *bus);
void IICStop(IIC_HandleTypeDef *bus);
unsigned char IICWaitAck(IIC_HandleTypeDef *bus);
void IICSendAck(IIC_HandleTypeDef *bus);
void IICSendNotAck(IIC_HandleTypeDef *bus);
void IICSendByte(IIC_HandleTypeDef *bus, unsigned char cSendByte);
unsigned char IICReceiveByte(IIC_HandleTypeDef *bus);
void IICInit(IIC_HandleTypeDef *bus);

uint8_t IIC_Write_One_Byte(IIC_HandleTypeDef *bus, uint8_t daddr,uint8_t reg,uint8_t data);
uint8_t IIC_Write_Multi_Byte(IIC_HandleTypeDef *bus, uint8_t daddr,uint8_t reg,uint8_t length,uint8_t buff[]);
unsigned char IIC_Read_One_Byte(IIC_HandleTypeDef *bus, uint8_t daddr,uint8_t reg);
uint8_t IIC_Read_Multi_Byte(IIC_HandleTypeDef *bus, uint8_t daddr, uint8_t reg, uint8_t length, uint8_t buff[]);
#endif
