#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyKey.h"
typedef enum {
	MyMenu_Time = 0,
	MyMenu_AlarmClock = 1,
	MyMenu_StopWatch = 2,
	MyMenu_Game = 3,
} MyMenu_t;


uint8_t MyMenuFlag = 0;
MyKeyEvent_t KeyEvent1,KeyEvent2;

void MyMenu_Init(void)
{
	MyKey_struct key1struct;
	MyKey_struct key2struct;
	MyKeyStruct_Init(&key1struct);
	MyKeyStruct_Init(&key2struct);
	MyKey_Init();
	OLED_Init();
	OLED_ShowString(0, 0, "1.时间", OLED_8X16);
	OLED_ShowString(0, 16, "2.闹钟", OLED_8X16);
	OLED_ShowString(0, 32, "3.秒表", OLED_8X16);
	OLED_ShowString(0, 48, "4.小游戏", OLED_8X16);
	OLED_Update();
}	

void MyMenu_First(MyMenu_t MyMenu_MainMenu)
{
	switch(MyMenu_MainMenu)
	{
		case MyMenu_Time:
			OLED_ReverseArea(0, 0, 128,16);
			OLED_Update();
			break;
		case MyMenu_AlarmClock:
			OLED_ReverseArea(0, 16, 128, 16);
			OLED_Update();
			break;
		case MyMenu_StopWatch:
			OLED_ReverseArea(0, 32, 128, 16);
			OLED_Update();
			break;
		case MyMenu_Game:
			OLED_ReverseArea(0, 48, 0, 128);
			OLED_Update();
			break;
		default :
			break;
	}
}

uint8_t MyMenu_FristFlag(void)
{	
	KeyEvent1 = MyKey_GetEvent(GPIOA,GPIO_Pin_0,&key1struct);
	KeyEvent2 = MyKey_GetEvent(GPIOA,GPIO_Pin_1,&key2struct);
	if(KeyEvent1 == KEY_Single)
	{
		MyMenuFlag++;
		if(MyMenuFlag >= 4)
		{
			MyMenuFlag = 0;
		}
	}
	if(KeyEvent2 == KEY_Single)
	{
		
		if(MyMenuFlag == 0)
		{
			MyMenuFlag = 3;
		}
		else 
		{
			MyMenuFlag--;
		}
	}
	return MyMenuFlag;
}

void MyMenu_MainMenu(void)
{
	MyMenu_First((MyMenu_t) MyMenuFlag);
}
