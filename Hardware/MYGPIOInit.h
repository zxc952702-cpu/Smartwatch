#ifndef __MYGPIOINIT_H
#define __MYGPIOINIT_H

void MYGPIO_Init(GPIO_TypeDef *GPIO_portx,uint16_t GPIO_Pin_x,GPIOMode_TypeDef GPIO_Mode);
uint32_t SIR_GETData(void);
void MyNvic_Init(void);
uint8_t get_itflag(void);
uint16_t Infrared_Data(void);
uint8_t get_IRS_VELUE(void);

#endif
