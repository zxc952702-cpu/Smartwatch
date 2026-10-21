#include "stm32f10x.h"                  // Device header
#include "MyTIM.h" 
#include "OLED.h"

uint8_t SWSecond;
uint8_t SWMin;
uint16_t SWHour;
uint16_t SWMs;

void MyMenuStopWatch_Init(void)
{
	MyTIM_Init();
}

void MyStopWatch_Show(void)
{
	OLED_ShowString(0,0,"秒表：",OLED_8X16);
	OLED_ShowNum(48,0,SWHour, 3,OLED_8X16);    	
	OLED_ShowString(72,0,"小时",OLED_8X16);
	
	OLED_ShowNum(0,16,SWMin, 2,OLED_8X16);		//分
	OLED_ShowString(16,16,"分:",OLED_8X16);
	OLED_ShowNum(40,16,SWSecond, 2,OLED_8X16);	//秒
	OLED_ShowString(56,16,"秒",OLED_8X16);  	
	
	OLED_ShowNum(0,32,SWMs, 3,OLED_8X16);		//毫秒
}

void MyMenu_SecondStopWatch_Show(void)
{
	SWSecond =0;
	SWMin =0;
	SWHour =0;
	SWMs =0;
	MyTIM_Clear();
	MyStopWatch_Show();
	
	OLED_ShowString(80,48,"开始",OLED_8X16);
	
	OLED_Update();
}

void MyMenu_ThirdStopWatch_Start(void)
{
	uint32_t sec;
	uint16_t ms;
	
	MyTIM_Start();
	
	MyTIM_TimeGet(&sec,&ms);
	SWSecond = sec%60;
	SWMin = (sec%3600)/60;
	SWHour = sec/3600;
	SWMs = ms;
	
	MyStopWatch_Show();
	
	OLED_ShowString(80,48,"停止",OLED_8X16);
	OLED_ShowString(0,48,"清除",OLED_8X16);
	
	OLED_Update();
}

void MyMenu_ThirdStopWatch_Stop(void)
{
	uint32_t sec;
	uint16_t ms;
	MyTIM_Stop();
	MyTIM_TimeGet(&sec,&ms);
	
	SWSecond = sec%60;
	SWMin = (sec%3600)/60;
	SWHour = sec/3600;
	SWMs = ms;
	
	MyStopWatch_Show();
	
	OLED_ShowString(80,48,"开始",OLED_8X16);
	OLED_ShowString(0,48,"清除",OLED_8X16);
	OLED_Update();
}

void MyMenu_ThirdStopWatch_Clear(void)
{
	SWSecond =0;
	SWMin =0;
	SWHour =0;
	SWMs =0;
	MyTIM_Clear();
	MyStopWatch_Show();
	OLED_ShowString(80,48,"开始",OLED_8X16);
	OLED_ShowString(0,48,"清除",OLED_8X16);
	OLED_Update();
}


