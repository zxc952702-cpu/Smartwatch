#include "stm32f10x.h"                  // Device header

//void I2C_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
//{
//	uint32_t t=10000;
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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  //选择引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_DeInit(I2C1);
	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 200000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
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

void MyI2C_DMASend_Init(uint8_t *Data,uint16_t Size)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitTypeDef DMA_InitStrucute;
	DMA_InitStrucute.DMA_BufferSize = Size;
	DMA_InitStrucute.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStrucute.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStrucute.DMA_MemoryBaseAddr = (uint32_t)Data;
	DMA_InitStrucute.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStrucute.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStrucute.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStrucute.DMA_PeripheralBaseAddr =(uint32_t)&(I2C1 -> DR);
	DMA_InitStrucute.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStrucute.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStrucute.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_Init(DMA1_Channel6,&DMA_InitStrucute);
}

void MyI2C_Hardware_Sendbyte(uint8_t Address, uint8_t Data)
{
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	
	I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Transmitter);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
	
	I2C_SendData(I2C1, Data);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
	
	I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t MyI2C_Hardware_Read(uint8_t Address,uint8_t RegAddress)
{
	uint8_t temp;
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	
	I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Transmitter);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
	
	I2C_SendData(I2C1, RegAddress);  //要读的寄存器地址
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	
	I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Receiver);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);
	
	I2C_AcknowledgeConfig(I2C1,DISABLE);	
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	temp = I2C_ReceiveData(I2C1);
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	return temp;
}
