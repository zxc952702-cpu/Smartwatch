#ifndef __MYI2C_H
#define __MYI2C_H

//void I2C_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
void MyI2C_Hardware_Init(void);
void MyI2C_Software_Init(void);
void MyI2C_DMASend_Init(uint8_t *Buff,uint16_t Size);
void MyI2C_Hardware_Sendbyte(uint8_t Address, uint8_t Data);
uint8_t MyI2C_Hardware_Read(uint8_t Address,uint8_t RegAddress);

#endif
