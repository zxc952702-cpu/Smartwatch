#ifndef __MYKEY_H
#define __MYKEY_H


typedef enum{
	KEYEvent_None = 0,
	KEYEvent_Single,
    KEYEvent_Double,
    KEYEvent_Longtype,
	KEYEvent_LongTimeOut,
} MyKeyEvent_t;

void MyKey_Init(void);
MyKeyEvent_t MyKey1_GetEvent(void);
MyKeyEvent_t MyKey2_GetEvent(void);

#endif
