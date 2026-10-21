#include "stm32f10x.h"                  // Device header
#include "MyI2C.h" 
#include "OLED.h" 

static	uint8_t valueRec = 0;
static	uint8_t valueLight = 0;
static	uint8_t	SetValue = 0;

void MyMenu_SecondGetValue(uint8_t value)
{
	valueLight = value;
	valueRec = valueLight/2;
	SetValue = (uint8_t)(valueLight * 2.55f);
//	OLED_WriteCommand(0x81);
//	OLED_WriteCommand(SetValue);
}

void MyMenu_SecondOLEDshow(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"亮度：",OLED_8X16);
	OLED_DrawRectangle(48,4, 50,10,OLED_UNFILLED);
	OLED_DrawRectangle(48,4, valueRec,10,OLED_FILLED);
	OLED_ShowNum(98,0,valueLight,3 ,OLED_8X16);
	OLED_Update();
}

void MyMenu_SecondSureSet(uint8_t value)
{
	OLED_WriteCommand(0x81);
	OLED_WriteCommand(SetValue);
}
