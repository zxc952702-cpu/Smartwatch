#ifndef __MYMENU_H
#define __MYMENU_H

#include "MyKey.h"
typedef enum {
	MyMenu_Time = 0,
	MyMenu_AlarmClock = 1,
	MyMenu_StopWatch = 2,
	MyMenu_Game = 3,
} MyMenu_t;


void MyMenu_Init(void);
void MyMenu_MainMenu(void);

#endif
