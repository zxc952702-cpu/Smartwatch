#include "stm32f10x.h"                  // Device header

//void I2C_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
//{
//	uint16_t t=10000;
//	while(I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
//	{
//		t--;
//		if(t == 0)
//		{
//			break;
//		}
//	}
//}

void MyI2C_Hardware_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 200000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x20;
	I2C_Init(I2C1,&I2C_InitStructure);
	I2C_Cmd(I2C1,ENABLE);
}


void MyI2C_Software_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

//void MyI2C_Hardware_Sendbyte(uint8_t Data)
//{
//	I2C_GenerateSTART(I2C1, ENABLE);
//	I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT);
//	I2C_Send7bitAddress(I2C1, 0x3C, I2C_Direction_Transmitter);
//	I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
//	I2C_SendData(I2C1, 0x00);
//	I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
//	I2C_GenerateSTOP(I2C1, ENABLE);
//}
