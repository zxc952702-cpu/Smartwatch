#ifndef __MYTIME_H
#define __MYTIME_H

#include <time.h>

void MyTime_Init(void);
time_t MyTime_Now(void);
void MyTime_Setting(uint16_t *Array);
void MyAlarm_Setting(uint16_t *Array);
time_t MyAlarm_Get(void);
void MyAlarm_SaveFlag(void);
void MyAlarm_Clear(void);
uint8_t RTC_AlarmSetFlag_Get(void);
void RTCAlarm_Work(void);

#endif
