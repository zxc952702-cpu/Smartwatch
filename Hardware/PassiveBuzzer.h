#ifndef __PASSIVEBUZZER
#define __PASSIVEBUZZER


void PassiveBuzzer_Init(void);
void PassiveBuzzer_AlarmTask(uint32_t StartTime);
uint32_t GetTik(void);

#endif
