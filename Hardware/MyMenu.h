#ifndef __MYMENU_H
#define __MYMENU_H

#include "MyKey.h"

typedef enum {
	MyMenu_Time = 0,
	MyMenu_AlarmClock = 1,
	MyMenu_StopWatch = 2,
	MyMenu_Pedometer = 3,
	MyMenu_Settings =4,
	MyMenu_None,
} MyMenu_First_t;

typedef enum {
	MyMenu_Level1 = 0,
	MyMenu_Level2 = 1,
	MyMenu_Level3 = 2,
} MyMenu_Level_t;

void MyMenu_Init(void);
void MyMenu_LevelAll(void);
void OLED_EnergyTask(void);

#endif
