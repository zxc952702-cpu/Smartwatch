#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"

typedef enum{
	KEYState_Idle = 0,
	KEYState_Press,
    KEYState_Wait_Double,
	KEYState_Double_Press,
    KEYState_Long,
	KEYState_LongTimeOut,
} MyKeyState_t;

typedef enum{
	KEYEvent_None = 0,
	KEYEvent_Single,
    KEYEvent_Double,
    KEYEvent_Longtype,
	KEYEvent_LongTimeOut,
} MyKeyEvent_t;

typedef struct {
	TickType_t start;
	MyKeyState_t state;
	TickType_t debounceStart;
	uint8_t StableState;
	uint8_t LastState;
} MyKey_struct;

MyKey_struct key1struct;
MyKey_struct key2struct;
MyKeyEvent_t MyKey1Event;
MyKeyEvent_t MyKey2Event;

void MyKey_GPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void MyKeyStruct_Init(MyKey_struct *key)
{
    key->start = 0;
    key->state = KEYState_Idle;
	key->debounceStart = 0;
	key->StableState = 0;
    key->LastState = 0;
}

void MyKey_Init(void)
{
	MyKey_GPIO_Init();
	MyKeyStruct_Init(&key1struct);
	MyKeyStruct_Init(&key2struct);
}

MyKeyEvent_t MyKey_GetEvent(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, MyKey_struct *key)
{	
	MyKeyEvent_t KeyEvent = KEYEvent_None;
	TickType_t now = xTaskGetTickCount(); 

	uint8_t KeyNum = 0;
	
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Bit_RESET)
	{
		KeyNum = 1;
	}
	else
	{
		KeyNum	= 0;
	}
	if(KeyNum != key -> LastState)
	{
		key -> LastState = KeyNum;
		key -> debounceStart = now;
	}
	else
	{
		if(now - key->debounceStart >= pdMS_TO_TICKS(20))
		{
			key -> StableState = KeyNum;
		}
	
	}
	switch (key -> state)
	{
		case KEYState_Idle :
			if(key -> StableState == 1)		//第一次按下
			{
				key -> start = now;			//按下开始计时
				key -> state = KEYState_Press;	//按键状态转到KEY_Press
			}
			break;
		case KEYState_Press :					
			if(key -> StableState == 0)   //按下后松手
			{
				key -> start = now;  // 重新计时
				key -> state = KEYState_Wait_Double;
			}
			else if(now - (key -> start) >= pdMS_TO_TICKS(400))  	//按下没松手且超过400ms
			{
				key -> state = KEYState_Long;
			}
			break;
		case KEYState_Wait_Double :
			if(now - (key -> start) >= pdMS_TO_TICKS(400))		//第一次按下松手后超过400ms没按
			{
				key -> state = KEYState_Idle;
				KeyEvent = KEYEvent_Single;
			}
			//按下后又按下，且间隔不超过400ms
			else if(key -> StableState ==1)		
			{
				key -> state = KEYState_Double_Press;
			}
			break;
			//第二次按下后再次松手，彻底完成双击
		case KEYState_Double_Press :
			if(key -> StableState == 0)
			{
				key -> state = KEYState_Idle;
				KeyEvent = KEYEvent_Double;
			}
			break;
			//长按一直是 按下状态 且时间大于400ms且小于1000ms 则判定长按，
		case KEYState_Long :
			if(now - (key -> start) < pdMS_TO_TICKS(1000))
			{
				KeyEvent = KEYEvent_Longtype;
				key -> state = KEYState_Idle;
			}			
			//否则如果，一直按着没松手，且大于1000ms，我算他一次长按，但是输出的状态是超时
			else if(now - (key -> start) >= pdMS_TO_TICKS(1000) && key -> StableState == 1)		
			{
				KeyEvent = KEYEvent_Longtype;
				key -> state = KEYState_LongTimeOut;
			}
			break;
		case KEYState_LongTimeOut :
			if(key -> StableState == 0)
			{
				key -> state = KEYState_Idle;
			}
		default :
			break;
	}
	return KeyEvent;
}

//uint8_t MyKey1_GetEvent(void)
//{	
//	static TickType_t Key1Start = 0;
//	static MyKeyState_t Key1State = KEY_Idle;
//	MyKeyEvent_t Key1Event = KEY_None;
//	
//	static TickType_t Key2Start = 0;
//	static MyKeyState_t Key2State = KEY_Idle;	
//	MyKeyEvent_t Key2Event = KEY_None;
//	
//	uint8_t KeyNum1;
//	uint8_t KeyNum2;
//	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == Bit_RESET)
//	{
//		KeyNum1 = 1;
//	}
//	else
//	{
//		KeyNum1	= 0;
//		
//	}

//	switch (Key1State)
//	{
//		case KEY_Idle :
//			if(KeyNum1 == 1)		//第一次按下
//			{
//				Key1Start = xTaskGetTickCount();
//				Key1State = KEY_Press;
//			}
//			break;
//		case KEY_Press :
//			if(KeyNum1 == 0)   //按下后松手
//			{
//				Key1Start = xTaskGetTickCount();  // 重新计时
//				Key1State = KEY_Wait_Double;
//			}
//			else if(xTaskGetTickCount() - Key1Start >= pdMS_TO_TICKS(400))  	//按下没松手且超过400ms
//			{
//				Key1State = KEY_Long;
//				Key1Event = KEY_Longtype;
//			}
//			break;
//		case KEY_Wait_Double :
//			if(xTaskGetTickCount() - Key1Start >= pdMS_TO_TICKS(400))		//第一次按下松手后超过400ms没按
//			{
//				Key1State = KEY_Idle;
//				Key1Event = KEY_Single;
//			}
//			
//			//按下后又按下，且间隔不超过400ms
//			
//			else if(KeyNum1 ==1 && xTaskGetTickCount() - Key1Start < pdMS_TO_TICKS(400))		
//			{
//				Key1State = KEY_Double_Press;
//			}
//			break;
//			//第二次按下后再次松手，彻底完成双击
//		case KEY_Double_Press :
//			if(KeyNum1 == 0)
//			{
//				Key1State = KEY_Idle;
//				Key1Event = KEY_Double;
//			}
//			break;
//			
//			//长按一直是 按下状态 且时间大于400ms 则判定长按
//		case KEY_Long :
//			if(KeyNum1 == 0)
//			{
//				Key1State = KEY_Idle;
//			}
//			break;
//		default :
//			break;
//	
//	}
//	return Key1Event;
//}

//uint8_t MyKey2_GetEvent(void)
//{
//	static MyKeyState_t MyKey2_State;
//	static TickType_t Key2Start = 0;
//	MyKeyEvent_t Key2Event = KEY_None;
//	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == Bit_RESET)
//	{
//		KeyNum2 = 1;
//	}
//	else
//	{
//		KeyNum2 = 0;
//	}
//	
//	switch (Key2State)
//	{
//		case KEY_Idle :
//			if(KeyNum2 == 1)		//第一次按下
//			{
//				Key2Start = xTaskGetTickCount();
//				Key2State = KEY_Press;
//			}
//			break;
//		case KEY_Press :
//			if(KeyNum2 == 0)   //按下后松手
//			{
//				Key2Start = xTaskGetTickCount();  // 重新计时
//				Key2State = KEY_Wait_Double;
//			}
//			else if(xTaskGetTickCount() - Key1Start >= pdMS_TO_TICKS(400))  	//按下没松手且超过400ms
//			{
//				Key2State = KEY_Long;
//				Key2Event = KEY_Longtype;
//			}
//			break;
//		case KEY_Wait_Double :
//			if(xTaskGetTickCount() - Key1Start >= pdMS_TO_TICKS(400))		//第一次按下松手后超过400ms没按
//			{
//				Key2State = KEY_Idle;
//				Key2Event = KEY_Single;
//			}
//			
//			//按下后又按下，且间隔不超过400ms
//			
//			else if(KeyNum2 ==1 && xTaskGetTickCount() - Key1Start < pdMS_TO_TICKS(400))		
//			{
//				Key2State = KEY_Double_Press;
//			}
//			break;
//			//第二次按下后再次松手，彻底完成双击
//		case KEY_Double_Press :
//			if(KeyNum2 == 0)
//			{
//				Key2State = KEY_Idle;
//				Key2Event = KEY_Double;
//			}
//			break;
//			
//			//长按一直是 按下状态 且时间大于400ms 则判定长按
//		case KEY_Long :
//			if(KeyNum2 == 0)
//			{
//				Key2State = KEY_Idle;
//			}
//			break;
//		default :
//			break;
//	
//	}
//	return Key2Event;
//}
