#ifndef __MYTIM_H
#define __MYTIM_H

void MyTIM_Init(void);
void MyTIM_Clear(void);
void MyTIM_Start(void);
void MyTIM_Stop(void);
void MyTIM_TimeGet(uint32_t *sec, uint16_t *ms);

#endif
