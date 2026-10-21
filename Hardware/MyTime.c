#include "stm32f10x.h"                  // Device header
#include <time.h>
#include "PassiveBuzzer.h" 
#include "MyTIM.h"


static time_t AlarmTime = 0;
static uint8_t AlarmSetFlag = 0;
static volatile uint8_t Alarmitflag = 0;

void MyTime_Init(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();

	if(BKP_ReadBackupRegister(BKP_DR1) !=0xA5A5)
	{
		RTC_SetPrescaler(32768-1);
		RTC_WaitForLastTask();
		RTC_SetCounter(946684800);
		RTC_WaitForLastTask();
		BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
		RTC_WaitForLastTask();
	}
	
	PWR_BackupAccessCmd(DISABLE);
	RTC_ITConfig(RTC_IT_ALR,ENABLE);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

time_t MyTime_Now(void)
{
	RTC_WaitForSynchro();
	time_t curtime = RTC_GetCounter();
	return curtime;
}

void MyTime_Setting(uint16_t *Array)
{
	struct tm MyTime_locatime;
	MyTime_locatime.tm_hour = Array[0];
	MyTime_locatime.tm_min = Array[1];
	MyTime_locatime.tm_sec = Array[2];
	MyTime_locatime.tm_year = Array[3] - 1900;
	MyTime_locatime.tm_mon = Array[4] - 1;
	MyTime_locatime.tm_mday = Array[5];
	MyTime_locatime.tm_isdst = -1;
	time_t curtime = mktime(&MyTime_locatime);
	PWR_BackupAccessCmd(ENABLE);
	RTC_WaitForLastTask();
	RTC_SetCounter(curtime);
	RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);
}

void MyRTC_SetAlarm(time_t AlarmTime)
{
	RTC_ClearFlag(RTC_FLAG_ALR);
	EXTI_ClearFlag(EXTI_Line17);
    PWR_BackupAccessCmd(ENABLE);
    RTC_WaitForLastTask();
    RTC_SetAlarm(AlarmTime);
    RTC_WaitForLastTask();
    PWR_BackupAccessCmd(DISABLE);
}

void MyRtc_WriteBkpReg(uint16_t BKP_DR, uint16_t Data)
{
    PWR_BackupAccessCmd(ENABLE);
	RTC_WaitForLastTask();
	BKP_WriteBackupRegister(BKP_DR,Data);
	RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);
}

void MyAlarm_SaveFlag(uint16_t *Array)
{
	if(AlarmSetFlag == 0)
	{
		RTC_ClearFlag(RTC_FLAG_ALR);
		MyRtc_WriteBkpReg(BKP_DR3,0);
		MyRtc_WriteBkpReg(BKP_DR4,0);
		MyRtc_WriteBkpReg(BKP_DR5,0);
		MyRtc_WriteBkpReg(BKP_DR2,0xA2A2);
	}
	else if(AlarmSetFlag ==1)
	{
		MyRtc_WriteBkpReg(BKP_DR3,Array[0]);
		MyRtc_WriteBkpReg(BKP_DR4,Array[1]);
		MyRtc_WriteBkpReg(BKP_DR5,Array[2]);
		MyRtc_WriteBkpReg(BKP_DR2,0xA1A1);
	}
}


void MyAlarm_Setting(uint16_t *Array)
{
	time_t timenow = MyTime_Now();
	struct tm MyAlarm_tm = *localtime(&timenow);
	MyAlarm_tm.tm_hour = Array[0];
	MyAlarm_tm.tm_min = Array[1];
	MyAlarm_tm.tm_sec = Array[2];
	
	AlarmTime = mktime(&MyAlarm_tm);
	
	if(AlarmTime <= timenow)
	{
		MyAlarm_tm.tm_mday ++;
		AlarmTime = mktime(&MyAlarm_tm);		
	}
	MyRTC_SetAlarm(AlarmTime);
	AlarmSetFlag = 1;
	MyAlarm_SaveFlag(Array);
	RTC_ITConfig(RTC_IT_ALR,ENABLE);
}

time_t MyAlarm_Get(void)
{
	time_t timenow = MyTime_Now();
	struct tm MyAlarm_tm = *localtime(&timenow);
	MyAlarm_tm.tm_hour = BKP_ReadBackupRegister(BKP_DR3);
	MyAlarm_tm.tm_min = BKP_ReadBackupRegister(BKP_DR4);
	MyAlarm_tm.tm_sec = BKP_ReadBackupRegister(BKP_DR5);
	AlarmTime = mktime(&MyAlarm_tm);
	return AlarmTime;
}

void RTCAlarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALR) == SET)
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);
		EXTI_ClearITPendingBit(EXTI_Line17);
		Alarmitflag = 1;
	}
}

uint8_t RTC_AlarmSetFlag_Get(void)
{
	return AlarmSetFlag;
}

void MyAlarm_Clear(void)
{
	RTC_ITConfig(RTC_IT_ALR,DISABLE);
	RTC_ClearFlag(RTC_FLAG_ALR);
	EXTI_ClearFlag(EXTI_Line17);
	MyRtc_WriteBkpReg(BKP_DR3,0);
	MyRtc_WriteBkpReg(BKP_DR4,0);
	MyRtc_WriteBkpReg(BKP_DR5,0);
	MyRtc_WriteBkpReg(BKP_DR2,0xA2A2);
	AlarmSetFlag = 0;
    Alarmitflag = 0;
}

void RTCAlarm_Work(void)
{	
	uint32_t Nowtime = GetTik();
	static uint32_t StartTime = 0;
	static uint8_t Time = 0;
	if(Alarmitflag == 1)
	{
		if(Time == 0)
		{
			StartTime = Nowtime;
			Time = 1;
		}
		if(Nowtime - StartTime < 30000)
		{
			PassiveBuzzer_AlarmTask(StartTime);
		}
		else
		{
			TIM_CCxCmd(TIM3, TIM_Channel_1 ,TIM_CCx_Disable);
			MyAlarm_Clear();
			Time = 0;
		}
	}
}
