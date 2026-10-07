#ifndef __MYKEY_H
#define __MYKEY_H
#include "FreeRTOS.h"

typedef enum{
	KEY_Idle = 0,
	KEY_Press,
    KEY_Wait_Double,
	KEY_Double_Press,
    KEY_Long,
	KEYState_LongTimeOut,
} MyKeyState_t;

typedef enum{
	KEY_None = 0,
	KEY_Single,
    KEY_Double,
    KEY_Longtype,
	KEYEvent_LongTimeOut,
} MyKeyEvent_t;

typedef struct {
	TickType_t start;
	MyKeyState_t state;
	uint8_t StableState;
	uint8_t LastState;
	uint8_t StableCount;
} MyKey_struct;

extern MyKey_struct key1struct;
extern MyKey_struct key2struct;


void MyKey_Init(void);
void MyKeyStruct_Init(MyKey_struct *key);
void MyKey_Init(void);
MyKeyEvent_t MyKey_GetEvent(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, MyKey_struct *key);

#endif
