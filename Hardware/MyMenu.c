#include "stm32f10x.h"                  // Device header
//#include "FreeRTOS.h"
//#include "task.h"
#include "OLED.h"
#include "MyI2C.h"
#include "MyKey.h"
#include "MyTime.h"

#include "MyMenuStopWatch.h"
#include "MyMenuPedometer.h"
#include "PassiveBuzzer.h"
#include "MyMenuOLEDSet.h"
#include <Time.h>
/*********************************************

功能	主要作用
时间	显示时分秒、年月日，提供时间和日期设置入口
闹钟	设置闹钟时间、开关闹钟，到点提醒
秒表	开始、暂停、继续、清零及计圈
计步器	通过 MPU6050 识别步行动作并记录步数
设置	调整屏幕亮度、对比度及自动休眠


********************************************/

typedef enum {
	MyMenu_Time = 0,
	MyMenu_AlarmClock = 1,
	MyMenu_StopWatch = 2,
	MyMenu_Pedometer = 3,
	MyMenu_Settings =4,
	MyMenu_None,
} MyMenu_First_t;

typedef enum {
	MyMenu_Hour = 0,
	MyMenu_Minute = 1,
	MyMenu_Second = 2,
	MyMenu_Year = 3,
	MyMenu_Month = 4,
	MyMenu_Day = 5,
} MyMenu_SeconedTime_t;

typedef enum {
	MyMenu_AlarmClockHour = 0,
	MyMenu_AlarmClockMinute = 1,
	MyMenu_AlarmClockSecond = 2,
} MyMenu_SeconedAlarmClock_t;

//typedef enum {
//	MyMenu_AlarmClockHour = 0,
//	MyMenu_AlarmClockMinute = 1,
//	MyMenu_AlarmClockSecond = 2,
//} MyMenu_SeconedAlarmClock_t;

typedef enum {
	MyMenu_Level1 = 0,
	MyMenu_Level2 = 1,
	MyMenu_Level3 = 2,
	MyMenu_Level4 = 3,
} MyMenu_Level_t;
/*
typedef enum {
	MyMenu_Time = 0,
	MyMenu_AlarmClock = 1,
	MyMenu_StopWatch = 2,
	MyMenu_Pedometer = 3,
	MyMenu_Settings =4,
} MyMenu_Second_AlarmClock_t;

typedef enum {
	MyMenu_Time = 0,
	MyMenu_AlarmClock = 1,
	MyMenu_StopWatch = 2,
	MyMenu_Pedometer = 3,
	MyMenu_Settings =4,
} MyMenu_Second_StopWatch_t;

typedef enum {
	MyMenu_Time = 0,
	MyMenu_AlarmClock = 1,
	MyMenu_StopWatch = 2,
	MyMenu_Pedometer = 3,
	MyMenu_Settings =4,
} MyMenu_Second_Pedometer;

typedef enum {
	MyMenu_Time = 0,
	MyMenu_AlarmClock = 1,
	MyMenu_StopWatch = 2,
	MyMenu_Pedometer = 3,
	MyMenu_Settings =4,
} MyMenu_Second_Settings;
*/

static  MyMenu_Level_t MyMenu_LevelState = MyMenu_Level1;
//一级菜单光标变量
MyMenu_First_t MyMenu_FirstIndex;
//二级菜单时间光标变量
MyMenu_SeconedTime_t MyMenu_SeconedTimeIndex;
MyMenu_SeconedAlarmClock_t MyMenu_SeconedAlarmClockIndex;

//按键事件，按键1单击，双击，长按
MyKeyEvent_t Keytemp1 = KEYEvent_None;
//按键事件，按键2单击，双击，长按
MyKeyEvent_t Keytemp2 = KEYEvent_None;
//菜单跳转页面刷新标志位
static uint8_t MyMenu_PageRefresh = 0;

//static uint8_t MyMenu_ThirdRefresh = 0;

//菜单初始化
void MyMenu_Init(void)
{
	PassiveBuzzer_Init();  //有ms定时器
//	MyKey_Init();
//	MyI2C_Hardware_Init();  //OLED的这一个i2c1
	OLED_Init();
//	MyMenuPedometer_Init(); 	  //mpu6050在这里i2c
	MyTime_Init();
	MyMenuStopWatch_Init();    //这里面就是定时器的初始化

	OLED_ShowString(0, 0, "1.时间", OLED_8X16);
	OLED_ShowString(0, 16, "2.闹钟", OLED_8X16);
	OLED_ShowString(0, 32, "3.秒表", OLED_8X16);
	OLED_ShowString(0, 48, "4.计步器", OLED_8X16);
	OLED_ReverseArea(0, 0, 128,16);
//	OLED_ShowString(0, 0, "5.设置", OLED_8X16);
	OLED_Update();
}

//一级菜单光标
MyMenu_First_t MyMenu_FirstMenu_GetIndex(void)
{
	static uint8_t flag = 0;
	if(Keytemp1 == KEYEvent_Single)
	{
		if(flag == 0 || flag > 4)
		{
			flag = 4;
		}
		else 
		{
			flag --;
		}
	}
	else if(Keytemp2 == KEYEvent_Single)
	{
		if(flag >= 4)
		{
			flag = 0;
		}
		else
		{
			flag ++;
		}
	}
	return (MyMenu_First_t)flag;
}

//一级菜单
void MyMenu_First(void)
{
	static MyMenu_First_t MyMenu_LastIndex = MyMenu_Time;
	MyMenu_FirstIndex = MyMenu_FirstMenu_GetIndex();
	if(MyMenu_FirstIndex == MyMenu_LastIndex && MyMenu_PageRefresh == 0)
	{
		return;
	}
	else
	{
		switch(MyMenu_FirstIndex)
		{
			case MyMenu_Time:
					OLED_Clear();
					OLED_ShowString(0, 0, "1.时间", OLED_8X16);
					OLED_ShowString(0, 16, "2.闹钟", OLED_8X16);
					OLED_ShowString(0, 32, "3.秒表", OLED_8X16);
					OLED_ShowString(0, 48, "4.计步器", OLED_8X16);			
					OLED_ReverseArea(0, 0, 128,16);
					OLED_Update();
					MyMenu_LastIndex = MyMenu_Time;
			break;
			case MyMenu_AlarmClock:
					OLED_Clear();
					OLED_ShowString(0, 0, "1.时间", OLED_8X16);
					OLED_ShowString(0, 16, "2.闹钟", OLED_8X16);
					OLED_ShowString(0, 32, "3.秒表", OLED_8X16);
					OLED_ShowString(0, 48, "4.计步器", OLED_8X16);
					OLED_ReverseArea(0, 16, 128, 16);
					OLED_Update();
					MyMenu_LastIndex = MyMenu_AlarmClock;
			break;
			case MyMenu_StopWatch:
					OLED_Clear();
					OLED_ShowString(0, 0, "1.时间", OLED_8X16);
					OLED_ShowString(0, 16, "2.闹钟", OLED_8X16);
					OLED_ShowString(0, 32, "3.秒表", OLED_8X16);
					OLED_ShowString(0, 48, "4.计步器", OLED_8X16);
					OLED_ReverseArea(0, 32, 128, 16);
					OLED_Update();
					MyMenu_LastIndex = MyMenu_StopWatch;
			break;
			case MyMenu_Pedometer:
					OLED_Clear();
					OLED_ShowString(0, 0, "1.时间", OLED_8X16);
					OLED_ShowString(0, 16, "2.闹钟", OLED_8X16);
					OLED_ShowString(0, 32, "3.秒表", OLED_8X16);
					OLED_ShowString(0, 48, "4.计步器", OLED_8X16);		
					OLED_ReverseArea(0, 48, 128, 16);
					OLED_Update();
					MyMenu_LastIndex = MyMenu_Pedometer;
			break;
			case MyMenu_Settings:
					OLED_Clear();
					OLED_ShowString(0, 0, "5.设置", OLED_8X16);
					OLED_ReverseArea(0, 0, 128,16);
					OLED_Update();
					MyMenu_LastIndex = MyMenu_Settings;
			break;
			default :
				break;
		}
		MyMenu_PageRefresh = 0;	
	}
}
//二级菜单时间
void MyMenu_SecondTime(void)
{
	time_t TimeTemp = MyTime_Now();
	static time_t refushtime = 0;
	if(MyMenu_PageRefresh == 1)
	{
		OLED_Clear();
		OLED_Update();
		MyMenu_PageRefresh = 0;
	}
	if(refushtime != TimeTemp)
	{
		struct tm curtimestruct;
		char Buffer1[10];
		char Buffer2[15];
		time_t TimeTemp = MyTime_Now();
		curtimestruct = *localtime(&TimeTemp);
		strftime(Buffer1, sizeof(Buffer1), "%H:%M:%S", &curtimestruct);
		OLED_ShowString(0,0,Buffer1,OLED_8X16);
		strftime(Buffer2, sizeof(Buffer2), "%Y-%m-%d", &curtimestruct);
		OLED_ShowString(0,16,Buffer2,OLED_8X16);
		OLED_UpdateArea(0, 0, 128, 32);
		refushtime = TimeTemp;
	}
}

////二级菜单闹钟
void MyMenu_SecondAlarmClock(void)
{
	time_t AlarmClocktime = MyAlarm_Get();
	if(MyMenu_PageRefresh == 0)
	{
		return;
	}
	else
	{
		OLED_Clear();
		OLED_ShowString(0,0,"闹钟：",OLED_8X16);
		if(BKP_ReadBackupRegister(BKP_DR2) == 0xA1A1)//判断 如果有闹钟就显示真实的闹钟时间，如果没有就显示未设置.
	//显示方式暂时选择拆分的形式，后面再来看有没有更好的 字符串数组直接显示的办法。
		{
			
			struct tm AlarmClock = *localtime(&AlarmClocktime);			
			OLED_ShowNum(48,0,AlarmClock.tm_hour, 2,OLED_8X16);    	//时
			OLED_ShowString(64,0,":",OLED_8X16);
			OLED_ShowNum(72,0,AlarmClock.tm_min, 2,OLED_8X16);		//分
			OLED_ShowString(88,0,":",OLED_8X16);
			OLED_ShowNum(96,0,AlarmClock.tm_sec, 2,OLED_8X16);		//秒
			OLED_ShowString(0,16,"请设置闹钟",OLED_8X16);
			OLED_ShowString(0,32,"双击上按键",OLED_8X16);
			OLED_ShowString(0,48,"已设置闹钟",OLED_8X16);
		}
		else
		{
			OLED_ShowNum(48,0,0, 2,OLED_8X16);    	//时
			OLED_ShowString(64,0,":",OLED_8X16);
			OLED_ShowNum(72,0,0, 2,OLED_8X16);		//分
			OLED_ShowString(88,0,":",OLED_8X16);
			OLED_ShowNum(96,0,0, 2,OLED_8X16);		//秒
			OLED_ShowString(0,16,"请设置闹钟",OLED_8X16);
			OLED_ShowString(0,32,"双击上按键",OLED_8X16);
			OLED_ShowString(0,48,"未设置闹钟",OLED_8X16);
		}
		MyMenu_PageRefresh = 0;
	}
	OLED_Update();
}

void MyMenu_SecondStopWatch(void)
{
	if(MyMenu_PageRefresh == 0)
	{
		return;
	}
	else
	{
		MyMenu_SecondStopWatch_Show();
		MyMenu_PageRefresh = 0;
	}
}

//void MyMenu_SecondPedometer(void)
//{
//	OLED_Clear();
//	//显示方式暂时选择拆分的形式，后面再来看有没有更好的 字符串数组直接显示的办法。
//	OLED_ShowString(0,0,"步数：",OLED_8X16);
//	OLED_ShowNum(48,0,0, 5,OLED_8X16);    	//时
//	OLED_Update();
//}

uint8_t MyMenu_SecondSetLight(uint8_t *value)
{
	uint8_t flag = 0;
	if(Keytemp1 == KEYEvent_Single)
	{
		if(*value >= 100)
		{
			*value = 100;
		}
		else
		{
			*value += 1;
		}
		flag= 1;
	}
	else if(Keytemp2 == KEYEvent_Single)
	{
		if(*value <= 1 || *value > 100)
		{
			*value = 1;
		}
		else
		{
			*value -= 1;
		}
		flag = 1;
	}
	return flag;
}

void MyMenu_SecondOLEDupdate(uint8_t value)
{
	static uint8_t temp = 0;
	if(MyMenu_PageRefresh ==1)
	{
		MyMenu_SecondGetValue(value);
		MyMenu_SecondOLEDshow();
		temp = value;
		MyMenu_PageRefresh = 0;
	}
	else
	{
		if(temp !=value)
		{
			MyMenu_SecondGetValue(value);
			MyMenu_SecondOLEDshow();
			temp = value;
		}
	}
}

void MyMenu_SecondSettings(void)
{
	static uint8_t StartLight = 81;
	MyMenu_SecondGetValue(StartLight);
	MyMenu_SecondSetLight(&StartLight);
	MyMenu_SecondOLEDupdate(StartLight);
}


//二级总菜单
void MyMenu_Second_Enter(void)
{
		switch(MyMenu_FirstIndex)
		{
			case MyMenu_Time:
				MyMenu_SecondTime();
				break;
			case MyMenu_AlarmClock:
				MyMenu_SecondAlarmClock();
				break;

			case MyMenu_StopWatch:
				MyMenu_SecondStopWatch();
				break;

			case MyMenu_Pedometer:
				MyMenu_SecondPedometer();
				break;

			case MyMenu_Settings:
				MyMenu_SecondSettings();
				break;
			default:
				break;
		}
}

//三级菜单光标逻辑
void MyMenu_ThirdTime_Index(void)
{
	if(Keytemp1 == KEYEvent_Single)
	{
		if(MyMenu_SeconedTimeIndex >= 5)
		{
			MyMenu_SeconedTimeIndex = MyMenu_Hour;
		}
		else
		{
			MyMenu_SeconedTimeIndex ++;  //时分秒年月日，一共6个项目，加到5了 就不再加了
		}
	}
	else if(Keytemp2 == KEYEvent_Single)
	{
		if(MyMenu_SeconedTimeIndex == 0 || MyMenu_SeconedTimeIndex > 5)
		{
			MyMenu_SeconedTimeIndex = MyMenu_Day;
		}
//		else if(MyMenu_SeconedTimeIndex)
		else
		{
			MyMenu_SeconedTimeIndex --;
		}
	}
}

//三级菜单Time进入界面，就是显示一下反显。
void MyMenu_ThirdTime_Enter(void)
{
	static MyMenu_SeconedTime_t MyMenu_ThirdTime_lastIndex = MyMenu_Hour;
	static char Buffer1[10];
	static char Buffer2[15];
	static uint8_t FirstDraw = 1;
	
	MyMenu_ThirdTime_Index();
	
	if(MyMenu_PageRefresh == 0 && MyMenu_ThirdTime_lastIndex == MyMenu_SeconedTimeIndex 
		&& FirstDraw == 0)
	{
		return;
	}
	else if (MyMenu_PageRefresh == 1)
	{
		OLED_Clear();
		struct tm SetTime;
		time_t TimeTemp = MyTime_Now();
		struct tm *TimePtr = localtime(&TimeTemp);
		if(TimePtr != NULL)
        {
			SetTime = *TimePtr;
			strftime(Buffer1, sizeof(Buffer1), "%H:%M:%S", &SetTime);
			strftime(Buffer2, sizeof(Buffer2), "%Y-%m-%d", &SetTime);
			OLED_ShowString(0,0,Buffer1,OLED_8X16);
			OLED_ShowString(0,16,Buffer2,OLED_8X16);
			OLED_Update();
			MyMenu_PageRefresh = 0;
			FirstDraw = 1;
		}
	}
	if(FirstDraw == 1 || MyMenu_ThirdTime_lastIndex != MyMenu_SeconedTimeIndex)
	{
		OLED_Clear();
		OLED_ShowString(0,0,Buffer1,OLED_8X16);
		OLED_ShowString(0,16,Buffer2,OLED_8X16);
		switch(MyMenu_SeconedTimeIndex)
		{
			case MyMenu_Hour:
				OLED_ReverseArea(0,0,16,16);
				break;
			case MyMenu_Minute:
				OLED_ReverseArea(24,0,16,16);
				break;
			case MyMenu_Second:
				OLED_ReverseArea(48,0,16,16);
				break;
			case MyMenu_Year:
				OLED_ReverseArea(0,16,32,16);
				break;
			case MyMenu_Month:		
				OLED_ReverseArea(40,16,16,16);
				break;
			case MyMenu_Day:	
				OLED_ReverseArea(64,16,16,16);
				break;
			default :
				break;
		}
		OLED_Update();
		MyMenu_ThirdTime_lastIndex = MyMenu_SeconedTimeIndex;
		FirstDraw = 0;
	}
}

//四级菜单Time设置时间的逻辑。也就是设置数组
void MyMenu_FourthTime_ArraySet(uint16_t *array)
{
	switch(MyMenu_SeconedTimeIndex)
	{
		case MyMenu_Hour:
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Hour] >= 23)
				{
					array[MyMenu_Hour] = 0;
				}
				else
				{
					array[MyMenu_Hour] ++;
				}
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Hour] <= 0)
				{
					array[MyMenu_Hour] = 23;
				}
				else
				{
					array[MyMenu_Hour] --;
				}
			}
			break;
		case MyMenu_Minute:
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Minute] >= 59)
				{
					array[MyMenu_Minute] = 0;
				}
				else
				{
					array[MyMenu_Minute] ++;
				}
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Minute] <= 0)
				{
					array[MyMenu_Minute] = 59;
				}
				else
				{
					array[MyMenu_Minute] --;
				}
			}
			break;
		case MyMenu_Second:
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Second] >= 59)
				{
					array[MyMenu_Second] = 0;
				}
				else
				{
					array[MyMenu_Second] ++;
				}
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Second] <= 0)
				{
					array[MyMenu_Second] = 59;
				}
				else
				{
					array[MyMenu_Second] --;
				}
			}
			break;
		case MyMenu_Year:
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Year] >= 2099)
				{
					array[MyMenu_Year] = 2000;
				}
				else
				{
					array[MyMenu_Year] ++;
				}
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Year] <= 2000)
				{
					array[MyMenu_Year] = 2099;
				}
				else
				{
					array[MyMenu_Year] --;
				}
			}
			break;
		case MyMenu_Month:
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Month] >= 12)
				{
					array[MyMenu_Month] = 1;
				}
				else
				{
					array[MyMenu_Month] ++;
				}
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Month] <= 1)
				{
					array[MyMenu_Month] = 12;
				}
				else
				{
					array[MyMenu_Month] --;
				}
			}
			break;			
		case MyMenu_Day:
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Day] >= 31)
				{
					array[MyMenu_Day] = 1;
				}
				else
				{
					array[MyMenu_Day] ++;
				}
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(array[MyMenu_Day] <= 1)
				{
					array[MyMenu_Day] = 31;
				}
				else
				{
					array[MyMenu_Day] --;
				}
			}
			break;
		default :
			break;
	}
}
//四级菜单Time的oled展示，也就是设置时 按键按一下 数字跟着在屏幕反馈
void MyMenu_FourthTime_Show(uint16_t *array)
{
		OLED_ShowNum(0,0,array[MyMenu_Hour],2,OLED_8X16);
		OLED_ShowString(16,0,":",OLED_8X16);
		OLED_ShowNum(24,0,array[MyMenu_Minute],2,OLED_8X16);
		OLED_ShowString(40,0,":",OLED_8X16);	
		OLED_ShowNum(48,0,array[MyMenu_Second],2,OLED_8X16);
		OLED_ShowNum(0,16,array[MyMenu_Year],4,OLED_8X16);
		OLED_ShowString(32,16,"-",OLED_8X16);
		OLED_ShowNum(40,16,array[MyMenu_Month],2,OLED_8X16);
		OLED_ShowString(56,16,"-",OLED_8X16);
		OLED_ShowNum(64,16,array[MyMenu_Day],2,OLED_8X16);
		OLED_Update();
}



void MyMenu_FourthTime_OledUpdate(uint16_t *array)
{
	uint32_t nowtime = GetTik();
	static uint32_t Lasttime = 0;
	static uint8_t Flag = 0;
	static uint8_t LastFlag = 0;
	//设置时，当按键单击，此时必然有数据更新，自然也要更新屏幕，这是第一个if
	if(Keytemp1 == KEYEvent_Single || Keytemp2 == KEYEvent_Single)
	{
		MyMenu_FourthTime_Show(array);
	}
	//每隔500ms 让flag跳变一次
	if(nowtime - Lasttime >= 500)
	{
		Lasttime = nowtime;
		Flag = !Flag;
	}
	//当flag和LastFlag不同的时候，也就是500ms一次
	if(Flag != LastFlag)
	{
		//如果flag =1时，反显设置的区域，也就是提示用户 你在设置这里。
		if(Flag)
		{
			switch(MyMenu_SeconedTimeIndex)
			{
				case MyMenu_Hour:
					OLED_ReverseArea(0,0,16,16);
					break;
				case MyMenu_Minute:
					OLED_ReverseArea(24,0,16,16);
					break;
				case MyMenu_Second:
					OLED_ReverseArea(48,0,16,16);
					break;
				case MyMenu_Year:
					OLED_ReverseArea(0,16,32,16);
					break;
				case MyMenu_Month:		
					OLED_ReverseArea(40,16,16,16);
					break;
				case MyMenu_Day:
					OLED_ReverseArea(64,16,16,16);
					break;
				default :
					break;
			}
		}
		//否则更新屏幕，也就是 Flag = 1反向，Flag=0时又刷新屏幕，这样就形成了光标闪烁的效果
		else 
		{
			MyMenu_FourthTime_Show(array);
		}
		//这个标志位是为了延迟
		LastFlag = Flag;
		OLED_Update();
	}
}

///四级菜单time设置主程序
uint16_t *MyMenu_FourthTime_Set(void)
{
	static uint16_t array[6];
	///当从别的界面进来时，直接获得当前的count的值，然后展示正好赋值给array数组
	if(MyMenu_PageRefresh == 1)
	{
		time_t timenow = MyTime_Now();
		struct tm timestruct;
		timestruct = *localtime(&timenow);
		array[MyMenu_Hour] = timestruct.tm_hour;
		array[MyMenu_Minute] = timestruct.tm_min;
		array[MyMenu_Second] = timestruct.tm_sec;
		array[MyMenu_Year] = timestruct.tm_year + 1900;	
		array[MyMenu_Month] = timestruct.tm_mon+1;
		array[MyMenu_Day] = timestruct.tm_mday;
		MyMenu_FourthTime_Show(array);
		MyMenu_PageRefresh = 0;
	}
	MyMenu_FourthTime_ArraySet(array);
	MyMenu_FourthTime_OledUpdate(array);
	return array;
}


void MyMenu_ThirdAlarmClock_Index(void)
{
	if(Keytemp1 == KEYEvent_Single )
	{
		if(MyMenu_SeconedAlarmClockIndex >= MyMenu_AlarmClockSecond)
		{
			MyMenu_SeconedAlarmClockIndex = MyMenu_AlarmClockHour;
		}
		else
		{
			MyMenu_SeconedAlarmClockIndex++;
		}
		
	}
	else if(Keytemp2 == KEYEvent_Single)
	{
		if(MyMenu_SeconedAlarmClockIndex <= MyMenu_AlarmClockHour || MyMenu_SeconedAlarmClockIndex > MyMenu_AlarmClockSecond)
		{
			MyMenu_SeconedAlarmClockIndex = MyMenu_AlarmClockSecond;
		}
		else
		{
			MyMenu_SeconedAlarmClockIndex --;
		}
	}
}

void MyMenu_ThirdAlarmClock_OLEDShow(void)
{
		time_t Alarm = MyAlarm_Get();
		struct tm AlarmClock = *localtime(&Alarm);
		OLED_Clear();
		OLED_ShowString(0,0,"闹钟：",OLED_8X16);
		OLED_ShowNum(48,0,AlarmClock.tm_hour, 2,OLED_8X16);    	//时
		OLED_ShowString(64,0,":",OLED_8X16);
		OLED_ShowNum(72,0,AlarmClock.tm_min, 2,OLED_8X16);		//分
		OLED_ShowString(88,0,":",OLED_8X16);
		OLED_ShowNum(96,0,AlarmClock.tm_sec, 2,OLED_8X16);		//秒
	OLED_Update();
}


void MyMenu_ThirdAlarmClock_Enter(void)
{
	static MyMenu_SeconedAlarmClock_t MyMenu_ThirdAlarm_lastIndex = MyMenu_AlarmClockHour;
	static uint8_t FirstDraw = 0;
	MyMenu_ThirdAlarmClock_Index();
	if(MyMenu_PageRefresh == 0 && MyMenu_ThirdAlarm_lastIndex == MyMenu_SeconedAlarmClockIndex && FirstDraw==0)
	{
		return;
	}
	else if(MyMenu_PageRefresh == 1)
	{
		MyMenu_ThirdAlarmClock_OLEDShow();
		OLED_Update();
		MyMenu_PageRefresh = 0;
		FirstDraw = 1;
		MyMenu_ThirdAlarm_lastIndex = MyMenu_SeconedAlarmClockIndex;
	}
	else if(FirstDraw == 1 || MyMenu_ThirdAlarm_lastIndex != MyMenu_SeconedAlarmClockIndex)
	{
		MyMenu_ThirdAlarmClock_OLEDShow();
		switch(MyMenu_SeconedAlarmClockIndex)
		{
			case MyMenu_AlarmClockHour:
				OLED_ReverseArea(48,0,16,16);
				break;
			case MyMenu_AlarmClockMinute:
				OLED_ReverseArea(72,0,16,16);
				break;
			case MyMenu_AlarmClockSecond:
				OLED_ReverseArea(96,0,16,16);
				break;
			default :
				break;
		}
		OLED_Update();
		MyMenu_ThirdAlarm_lastIndex = MyMenu_SeconedAlarmClockIndex;
		FirstDraw = 0;
	}
}

void MyMenu_FourthAlarmClock_ArraySetting(uint16_t* AlarmArray)
{
	switch (MyMenu_SeconedAlarmClockIndex)
	{
		case MyMenu_AlarmClockHour :
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(AlarmArray[MyMenu_AlarmClockHour] >= 23)
				{
					AlarmArray[MyMenu_AlarmClockHour] = 0;
				}
				else
				{
					AlarmArray[MyMenu_AlarmClockHour] ++;
				}
				
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(AlarmArray[MyMenu_AlarmClockHour] <= 0 || AlarmArray[MyMenu_AlarmClockHour] > 23)
				{
					AlarmArray[MyMenu_AlarmClockHour] = 23;
				}
				else
				{
					AlarmArray[MyMenu_AlarmClockHour] --;
				}
			}
			break;
			
		case MyMenu_AlarmClockMinute :
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(AlarmArray[MyMenu_AlarmClockMinute] >= 59)
				{
					AlarmArray[MyMenu_AlarmClockMinute] = 0;
				}
				else
				{
					AlarmArray[MyMenu_AlarmClockMinute] ++;
				}
				
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(AlarmArray[MyMenu_AlarmClockMinute] <= 0 || AlarmArray[MyMenu_AlarmClockMinute] > 59)
				{
					AlarmArray[MyMenu_AlarmClockMinute] = 59;
				}
				else
				{
					AlarmArray[MyMenu_AlarmClockMinute] --;
				}
			}
			break;
			
		case MyMenu_AlarmClockSecond :
			if(Keytemp1 == KEYEvent_Single || Keytemp1 ==KEYEvent_Longtype)
			{
				if(AlarmArray[MyMenu_AlarmClockSecond] >=59)
				{
					AlarmArray[MyMenu_AlarmClockSecond] = 0;
				}
				else
				{
					AlarmArray[MyMenu_AlarmClockSecond] ++;
				}
				
			}
			else if(Keytemp2 == KEYEvent_Single || Keytemp2 ==KEYEvent_Longtype)
			{
				if(AlarmArray[MyMenu_AlarmClockSecond] <= 0 || AlarmArray[MyMenu_AlarmClockSecond] > 59)
				{
					AlarmArray[MyMenu_AlarmClockSecond] = 59;
				}
				else
				{
					AlarmArray[MyMenu_AlarmClockSecond] --;
				}
			}
			break;
	}
}

void MyMenu_FourthAlarmClock_OLEDShow(uint16_t *Array)
{
	OLED_Clear();
	OLED_ShowString(0,0,"闹钟：",OLED_8X16);
	OLED_ShowNum(48,0,Array[MyMenu_AlarmClockHour], 2,OLED_8X16);    	//时
	OLED_ShowString(64,0,":",OLED_8X16);
	OLED_ShowNum(72,0,Array[MyMenu_AlarmClockMinute], 2,OLED_8X16);		//分
	OLED_ShowString(88,0,":",OLED_8X16);
	OLED_ShowNum(96,0,Array[MyMenu_AlarmClockSecond], 2,OLED_8X16);		//秒
	OLED_Update();
}

void MyMenu_FourthAlarmClock_OledUpdate(uint16_t *Array)
{
	uint32_t NowTime = GetTik();
	static uint32_t LastTime = 0;
	static uint8_t FirstFlag = 0;
	static uint8_t LastFlag = 0;
	if(NowTime - LastTime >= 500)
	{
		FirstFlag = !FirstFlag;
		LastTime = NowTime;
	}
	if(Keytemp1 == KEYEvent_Single || Keytemp2 == KEYEvent_Single || MyMenu_PageRefresh ==1)
	{
		MyMenu_FourthAlarmClock_OLEDShow(Array);
		MyMenu_PageRefresh = 0;
	}
	if(FirstFlag != LastFlag)
	{
		if(FirstFlag)
		{
			switch (MyMenu_SeconedAlarmClockIndex)
			{
				case MyMenu_AlarmClockHour:
					OLED_ReverseArea(48,0,16,16);
					break;
				
				case MyMenu_AlarmClockMinute:
					OLED_ReverseArea(72,0,16,16);
					break;
				
				case MyMenu_AlarmClockSecond:
					OLED_ReverseArea(96,0,16,16);
					break;
			}
		}
		else
		{
			MyMenu_FourthAlarmClock_OLEDShow(Array);
		}
		
		LastFlag = FirstFlag;
		OLED_Update();
	}
}

uint16_t *MyMenu_FourthAlarmClock_Set(void)
{
	static uint16_t AlarmArray[3]={0};
	if(MyMenu_PageRefresh==1 && (BKP_ReadBackupRegister(BKP_DR2) == 0xA2A2))
	{
		AlarmArray[MyMenu_AlarmClockHour] = 0;
		AlarmArray[MyMenu_AlarmClockMinute] = 0;
		AlarmArray[MyMenu_AlarmClockSecond] = 0;
	}
	else if(BKP_ReadBackupRegister(BKP_DR2) == 0xA1A1 && MyMenu_PageRefresh==1)
	{
		AlarmArray[MyMenu_AlarmClockHour] = BKP_ReadBackupRegister(BKP_DR3);
		AlarmArray[MyMenu_AlarmClockMinute] = BKP_ReadBackupRegister(BKP_DR4);
		AlarmArray[MyMenu_AlarmClockSecond] = BKP_ReadBackupRegister(BKP_DR5);
	}
	MyMenu_FourthAlarmClock_ArraySetting(AlarmArray);
	MyMenu_FourthAlarmClock_OledUpdate(AlarmArray);
	return AlarmArray;
}

//三级总菜单清除
void MyMenu_Third_LongType(void)
{
		switch(MyMenu_FirstIndex)
		{
			case MyMenu_AlarmClock :
				MyAlarm_Clear();
				break;
			
//			case MyMenu_StopWatch :
////				MyAlarm_Clear();
//				break;
			
			case MyMenu_Pedometer :
//				MyAlarm_Clear();
				break;
			
			case MyMenu_Settings :
//				MyAlarm_Clear();
				break;
			
			default :
				break;
		}
}

void MyMenu_ThirdStopWatch_Enter(void)
{
	static uint8_t SWRunning = 1;
	if(SWRunning == 1 || MyMenu_PageRefresh == 1)
	{
		MyMenu_ThirdStopWatch_Start();
		SWRunning = 1;
		MyMenu_PageRefresh = 0;
	}
	if(Keytemp1 == KEYEvent_Single)
	{
		if(SWRunning)
		{
			MyMenu_ThirdStopWatch_Stop();
			SWRunning = 0;
		}
		else
		{
			MyMenu_ThirdStopWatch_Start();
			SWRunning = 1;
		}
	}
	else if(Keytemp2 == KEYEvent_Single)
	{
		MyMenu_ThirdStopWatch_Clear();
		SWRunning = 0;
	}
}


//三级总菜单
void MyMenu_Third_Enter(void)
{
		switch(MyMenu_FirstIndex)
		{
			case MyMenu_Time :
				MyMenu_ThirdTime_Enter();
				break;
			
			case MyMenu_AlarmClock :
				MyMenu_ThirdAlarmClock_Enter();
				break;
			
			case MyMenu_StopWatch :
				MyMenu_ThirdStopWatch_Enter();
				break;
			
			case MyMenu_Pedometer :
				MyMenu_ThirdTime_Enter();
				break;
			
			case MyMenu_Settings :
				MyMenu_ThirdTime_Enter();
				break;
			
			default :
				break;
		}
}



//四级总菜单
void MyMenu_Fourth_Set(void)
{
		switch(MyMenu_FirstIndex)
		{
			case MyMenu_Time :
				MyMenu_FourthTime_Set();
				break;
			
			case MyMenu_AlarmClock :
				MyMenu_FourthAlarmClock_Set();
				break;
			
//			case MyMenu_StopWatch :
//				MyMenu_FourthStopWatch_Set();
//				break;
			
			case MyMenu_Pedometer :
				MyMenu_FourthTime_Set();
				break;
			
			case MyMenu_Settings :
				MyMenu_FourthTime_Set();
				break;
			
			default :
				break;
		}
}

void MyMenu_SetupComplete(void)
{
	uint16_t *TimeArray = NULL;
	uint16_t *AlarmArray = NULL;
	switch(MyMenu_FirstIndex)
		{
			case MyMenu_Time :
				TimeArray = MyMenu_FourthTime_Set();
				MyTime_Setting(TimeArray);
				break;
			
			case MyMenu_AlarmClock :
				AlarmArray = MyMenu_FourthAlarmClock_Set();
				MyAlarm_Setting(AlarmArray);
				
				break;
			
			case MyMenu_StopWatch :
				MyMenu_FourthTime_Set();
				break;
			
			case MyMenu_Pedometer :
				MyMenu_FourthTime_Set();
				break;
			
			case MyMenu_Settings :
				MyMenu_FourthTime_Set();
				break;
			
			default :
				break;
		}
}

void MyMenu_LevelAll(void)
{
	
	Keytemp1 = MyKey1_GetEvent();
	Keytemp2 = MyKey2_GetEvent();
	static uint8_t value = 0;
	switch(MyMenu_LevelState)
	{
		case MyMenu_Level1:
			MyMenu_First();
			if (Keytemp1 == KEYEvent_Double)
			{
				OLED_Clear();
				MyMenu_PageRefresh = 1;
				MyMenu_LevelState = MyMenu_Level2;
			}
			break;
		case MyMenu_Level2:  //注意flag
			
				
				MyMenu_Second_Enter();
		
//				if((Keytemp1 == KEYEvent_Single || Keytemp2 == KEYEvent_Single) &&
//					MyMenu_FirstIndex == MyMenu_Settings)
//				{
//					value = *MyMenu_SecondSettings();
//				}
				if(Keytemp2 == KEYEvent_Double)
				{
					OLED_Clear();
					MyMenu_PageRefresh = 1;
					MyMenu_LevelState = MyMenu_Level1;
				}
				else if(Keytemp1 == KEYEvent_Double)
				{
					OLED_Clear();
					MyMenu_PageRefresh = 1;
					 
					if(MyMenu_FirstIndex == MyMenu_Settings)
					{
						MyMenu_SecondSureSet(value);
						MyMenu_LevelState = MyMenu_Level1;
					}
					else if(MyMenu_FirstIndex == MyMenu_Pedometer)
					{
						MyMenu_LevelState = MyMenu_Level1;
					}
					else
					{
						MyMenu_LevelState = MyMenu_Level3;
					}
					
				}
				if(MyMenu_FirstIndex == MyMenu_Pedometer && Keytemp1 == KEYEvent_Single)
				{
					MyMenu_PedometerClear();
				}
				break;
		case MyMenu_Level3:
				MyMenu_Third_Enter();  //  这里没玩
				if(Keytemp1 == KEYEvent_Double)
				{
					OLED_Clear();
					MyMenu_PageRefresh = 1;
					if(MyMenu_FirstIndex == MyMenu_StopWatch)
					{
						MyMenu_LevelState = MyMenu_Level3;
					}
					else
					{
						MyMenu_LevelState = MyMenu_Level4;
					}
					
				}
				else if(Keytemp2 == KEYEvent_Double)
				{
					
					OLED_Clear();
					MyMenu_PageRefresh = 1;

					MyMenu_LevelState = MyMenu_Level2;
				}
				
				else if(Keytemp2 == KEYEvent_Longtype)
				{
					if(MyMenu_FirstIndex == MyMenu_AlarmClock)
					{
						MyMenu_Third_LongType();
						OLED_Clear();
						MyMenu_PageRefresh = 1;
						MyMenu_LevelState = MyMenu_Level2;
					}
				}
				break;
		case MyMenu_Level4:
				MyMenu_Fourth_Set();  //  具体参数设置，目前是单个
				if(Keytemp2 == KEYEvent_Double)			//这是返回
				{
					OLED_Clear();
					MyMenu_PageRefresh = 1;
					MyMenu_LevelState = MyMenu_Level3;
				}
				else if(Keytemp1 == KEYEvent_Double)
				{
					MyMenu_SetupComplete();
					MyMenu_PageRefresh = 1;
					MyMenu_LevelState = MyMenu_Level3;
				}
				break;
	}
}

void OLED_EnergyTask(void)
{
	uint32_t nowtime = GetTik();
	static uint32_t fristtime = 0;
	static uint8_t SleepFlag = 0;
	if(fristtime == 0)
	{
		fristtime = nowtime;
	}
	if(nowtime - fristtime >= 30000 && (Keytemp1 == KEYEvent_None || Keytemp1 == KEYEvent_LongTimeOut) 
		&& (Keytemp2 == KEYEvent_None || Keytemp2 == KEYEvent_LongTimeOut)
		&& SleepFlag == 0)
	{
		OLED_Sleep();
		SleepFlag = 1;
	}
	else if((Keytemp1 != KEYEvent_None && Keytemp1 != KEYEvent_LongTimeOut) ||
			(Keytemp2 != KEYEvent_None && Keytemp2 != KEYEvent_LongTimeOut) || 
			(MyMenu_LevelState == MyMenu_Level3 && MyMenu_FirstIndex == MyMenu_StopWatch))
	{
		fristtime = nowtime;
	}
	if(SleepFlag == 1 && 
		((Keytemp1 != KEYEvent_None && Keytemp1 != KEYEvent_LongTimeOut) ||
			(Keytemp2 != KEYEvent_None &&Keytemp2 != KEYEvent_LongTimeOut) ||
			RTC_AlarmITFlag_Get() == 1))
	{
		OLED_WakeUp();
		fristtime = nowtime;
		SleepFlag = 0;
	}
}
