#ifndef __MYMPU6050_H
#define __MYMPU6050_H

void MyMPU6050_Init(void);
uint8_t MyMPU6050_GetID(void);
uint8_t MyMPU6050_ReciveData(uint8_t RegAddress);
void MyMPU6050_Proc(void);
void MyMPU6050_GetResult(float *pAccelOut, float *pTemp,float *pGyroOut, float *pEularAngleOut);
void MyMPU6050_GetStepCount(uint32_t *pSteapCount);
void MyMPU6050_ClearStepCount(void);

#endif
