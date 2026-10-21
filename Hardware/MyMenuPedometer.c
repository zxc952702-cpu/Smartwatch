#include "stm32f10x.h"                  // Device header
#include "MyMPU6050.h"
#include "OLED.h"

static uint32_t StepCount;
void MyMenuPedometer_Init(void)
{
	MyMPU6050_Init();
}

void MyMenu_SecondPedometer(void)
{
	
	float ACCin[3], GYROin[3], TEMP, EularAngle[3];
//	MyMPU6050_Proc();
	MyMPU6050_GetStepCount(&StepCount);
	
	MyMPU6050_GetResult(ACCin, &TEMP, GYROin, EularAngle);
	OLED_Clear();
	
	OLED_ShowString(0,0,"步数：" , OLED_8X16);
	OLED_ShowNum(48,0, StepCount, 5,OLED_8X16);
//	OLED_ShowFloatNum(0,16, ACCin[0], 2,3,OLED_8X16);
//	OLED_ShowFloatNum(0,32, ACCin[1], 2,3,OLED_8X16);
//	OLED_ShowFloatNum(48,48, ACCin[2],2,3,OLED_8X16);
	OLED_ShowString(0,48,"清除" , OLED_8X16);
	OLED_Update();
}

void MyMenu_PedometerClear(void)
{
	MyMPU6050_ClearStepCount();

	OLED_Clear();
	OLED_ShowString(0,0,"步数：" , OLED_8X16);
	OLED_ShowNum(48,0,StepCount, 5,OLED_8X16);
	OLED_ShowString(0,48,"清除" , OLED_8X16);
	OLED_Update();
}
