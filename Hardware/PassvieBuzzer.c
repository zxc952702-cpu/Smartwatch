#include "stm32f10x.h"                  // Device header

static volatile uint32_t Tikclock = 0;

void PassiveBuzzer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000-1;  //aar
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;	//psc
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0xFF;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50;   //ccr
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE);
	TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);
}

void PassiveBuzzer_AlarmTask(uint32_t Time)
{
	uint32_t NowTime = Tikclock;
	uint32_t ElapsedTime;
	ElapsedTime = NowTime - Time;
	ElapsedTime %= 2000;
	if(ElapsedTime < 100 || (ElapsedTime >= 200 && ElapsedTime < 300))
	{
		TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
	}
	else
	{
		TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);
	}
}

uint32_t GetTik(void)
{
	return Tikclock;
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		Tikclock ++;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
