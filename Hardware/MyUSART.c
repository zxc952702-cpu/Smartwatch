#include "stm32f10x.h"                  // Device header
#include <stdio.h>
void MyUSART_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
	/* Configure USART1 Tx (PA9) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
	USART_InitTypeDef USART_InitStrucute;
	USART_InitStrucute.USART_BaudRate = 115200;
	USART_InitStrucute.USART_HardwareFlowControl =USART_HardwareFlowControl_None; 
	USART_InitStrucute.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStrucute.USART_Parity = USART_Parity_No; 
	USART_InitStrucute.USART_StopBits = USART_StopBits_1;
	USART_InitStrucute.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStrucute);
	USART_Cmd(USART1, ENABLE);
}


int fputc(int ch, FILE *f)
{
	while((USART_GetFlagStatus(USART1, USART_FLAG_TXE)) == RESET)
	{
		
	}
    USART_SendData(USART1, (uint8_t)ch);
	
    return ch;
}
