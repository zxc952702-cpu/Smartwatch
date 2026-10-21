#include "stm32f10x.h"                  // Device header


static uint32_t millisecond = 0;

void MyTIM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0xFF;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_Init(&NVIC_InitStructure);
}

void MyTIM_Clear(void)
{
	TIM_Cmd(TIM2,DISABLE);
	TIM_SetCounter(TIM2,0);
	millisecond = 0;
}

void MyTIM_Start(void)
{
	TIM_Cmd(TIM2,ENABLE);
}

void MyTIM_Stop(void)
{
	TIM_Cmd(TIM2,DISABLE);
}

void MyTIM_TimeGet(uint32_t *sec, uint16_t *ms)
{
	uint32_t millisecond1 = 0;
	millisecond1 = millisecond;
	*sec = millisecond1/1000;
	*ms = millisecond1 %1000;
}

uint32_t MyTIM_millisecondGet(void)
{
	return millisecond;
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{
		millisecond++;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
