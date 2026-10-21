#include "stm32f10x.h"                  // Device header
#include "PassiveBuzzer.h"

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
	uint32_t start;
	MyKeyState_t state;
	uint32_t debounceStart;
	uint8_t StableState;
	uint8_t LastState;
} MyKey_struct;

//定义要使用的结构体

static MyKey_struct key1struct;			
static MyKey_struct key2struct;

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
//先定义结构体，再定义初始化
void MyKey_Init(void)
{
	MyKey_GPIO_Init();
	MyKeyStruct_Init(&key1struct);
	MyKeyStruct_Init(&key2struct);
	
}

MyKeyEvent_t MyKey_GetEvent(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, MyKey_struct *key)
{	
	MyKeyEvent_t KeyEvent = KEYEvent_None;
	uint32_t now = GetTik(); 

	uint8_t KeyNum = 0;
	
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Bit_RESET)  //按下是1 ，闲置是0
	{
		KeyNum = 1;
	}
	else
	{
		KeyNum	= 0;
	}
	if(KeyNum != key -> LastState)			//需要结合按键抖动思考 光思考语句本身是思考不出来什么的		
		//因为抖动，所以会有毛刺，也就是0...1不断跳变，然后获得稳定，所以要不断地负值给最后一次按键的状态，最终
	///按键稳定，然后再开始计时。把最后的这个按键值给记录下来，此时的时间也记录下来，然后来到else
	{
		key -> LastState = KeyNum;
		key -> debounceStart = now;
	}
	//上面判断的是一开始抖动部分，现在稳定了，else就是KeyNum == key -> LastState的时候
	//时间差超过20ms就记录键值。说明按下了
	else 
	{
		if(now - key->debounceStart >= 20)
		{
			key -> StableState = KeyNum;
		}
	
	}
	
	//后面的都是基于，按下没按下的键值处理的。
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
			else if(now - (key -> start) >= 500)  	//按下没松手且超过500ms
			{
				key -> state = KEYState_Long;
				KeyEvent = KEYEvent_Longtype;						//产生长按事件
			}
			break;
		case KEYState_Wait_Double :
			if(now - (key -> start) >= 400)		//第一次按下松手后超过400ms没按
			{
				key -> state = KEYState_Idle;
				KeyEvent = KEYEvent_Single;
			}
			//按下后又按下，且间隔不超过450ms
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
			//长按一直是 按下状态 且时间大于500ms且小于50000ms 则判定长按，
		case KEYState_Long :
			if(key -> StableState == 0)           //如果长按后松手
			{
				key -> state = KEYState_Idle;		//则回归空状态
			}			
			//如果长按，不松手且不超过50000ms，则一直去KEYState_Press 这里拿长按,利用时间差，每隔1000ms就那一次长按
			else if(now - (key -> start) < 50000)
			{
				static uint32_t temp = 0;
				if(temp == 0)
				{
					temp = now;
				}
				if(now - temp >= 1000)
				{
					temp = now;
					key -> state = KEYState_Press;
				}
			}
			//否则如果，一直按着没松手，且大于50000ms，但是输出的状态是超时，也输出 超时事件
			else if(now - (key -> start) >= 50000)
			{
				key -> state = KEYState_LongTimeOut;
				KeyEvent = KEYEvent_LongTimeOut;
			}
			break;
		case KEYState_LongTimeOut :        //如果超时松手，，则返回空状态， 如果一直不松手就一直超时
			if(key -> StableState == 0)   
			{
				key -> state = KEYState_Idle;
			}
			break;
		default :
			break;
	}
	return KeyEvent;
}

//返回按键 Key1事件的函数
MyKeyEvent_t MyKey1_GetEvent(void)
{
	return MyKey_GetEvent(GPIOA, GPIO_Pin_0, &key1struct);
}
//返回按键 Key2事件的函数
MyKeyEvent_t MyKey2_GetEvent(void)
{
	return MyKey_GetEvent(GPIOA, GPIO_Pin_1, &key2struct);
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
