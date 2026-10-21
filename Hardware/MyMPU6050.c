#include "stm32f10x.h"                  // Device header
#include <math.h>

#include "MyI2c.h" 

#include "MyMPU6050RegData.h"
#include "Delay.h"
#include "PassiveBuzzer.h"


static float ax ,ay ,az ,gx , gy, gz, temperature, yaw, roll,pitch;
const float alpha = 0.95238f;   //0.95238f
int16_t ACCX,ACCY,ACCZ,GYROX,GYROY,GYROZ,TEMP;
uint32_t Count;
static void MPU6050_I2C_WaitEvent(uint32_t Event)
{
    uint32_t Timeout = 100000;

    while (I2C_CheckEvent(I2C2, Event) != SUCCESS)
    {
        if (Timeout-- == 0)
        {
            break;
        }
    }

    return;
}

//void MyMPU6050_SendCommend(uint8_t RegAddress, uint8_t Commend)
//{
//	uint32_t timeout = 100000;
//	I2C_GenerateSTART(I2C1, ENABLE);
//	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
//	{
//		if(timeout -- ==0)
//		{
//			return;
//		}
//	}
//	
//	I2C_Send7bitAddress(I2C1, 0xD0, I2C_Direction_Transmitter);
//	if(MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == 0)
//	{
//		goto Error;
//	}
//	
//	
//	I2C_SendData(I2C1, RegAddress);
//	if(MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING) == 0)
//	{
//		goto Error;
//	}
//	
//	I2C_SendData(I2C1, Commend);
//	if(MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED) == 0)
//	{
//		goto Error;
//	}
//	
//	I2C_GenerateSTOP(I2C1, ENABLE);
//	
//	return;
//	
//	Error:

//    /* 尝试结束当前 I2C 通信 */
//    I2C_GenerateSTOP(I2C1, ENABLE);

//    /* 记录错误断点，调试时可以在此处设置断点 */
//    return;
//}


void MyMPU6050_SendCommend(uint8_t RegAddress, uint8_t Commend)
{
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT);
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	I2C_SendData(I2C2, RegAddress);
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	I2C_SendData(I2C2, Commend);
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	I2C_GenerateSTOP(I2C2, ENABLE);
}

uint8_t MyMPU6050_ReciveByte(uint8_t RegAddress,uint8_t *Byte)
{
	I2C_GenerateSTART(I2C2, ENABLE);
	
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT);
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	I2C_SendData(I2C2, RegAddress);  //要读的寄存器地址
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT);
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Receiver);
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	I2C_GenerateSTOP(I2C2, ENABLE);
	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED);
	*Byte = I2C_ReceiveData(I2C2);
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	return 1;
}

//uint8_t MyMPU6050_ReciveByte(uint8_t RegAddress,uint8_t *Byte)
//{
//	I2C_GenerateSTART(I2C1, ENABLE);
//	
//	if(MPU6050_I2C_WaitEvent( I2C_EVENT_MASTER_MODE_SELECT) ==0)
//	{
//		goto Error;
//	}
//	
//	I2C_Send7bitAddress(I2C1, 0xD0, I2C_Direction_Transmitter);
//	if(MPU6050_I2C_WaitEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) ==0)
//	{
//		goto Error;
//	}
//	
//	I2C_SendData(I2C1, RegAddress);  //要读的寄存器地址
//	if(MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)  ==0)
//	{
//		goto Error;
//	}
//	
//	I2C_GenerateSTART(I2C1, ENABLE);
//	if(MPU6050_I2C_WaitEvent( I2C_EVENT_MASTER_MODE_SELECT) ==0)
//	{
//		goto Error;
//	}
//	I2C_Send7bitAddress(I2C1, 0xD0, I2C_Direction_Receiver);
//	if(MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) ==0)
//	{
//		goto Error;
//	}
//	I2C_AcknowledgeConfig(I2C1, DISABLE);
//	I2C_GenerateSTOP(I2C1, ENABLE);
//	if(MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) ==0)
//	{
//		goto Error;
//	}
//	*Byte = I2C_ReceiveData(I2C1);
//	I2C_AcknowledgeConfig(I2C1, ENABLE);
//	return 1;
//	
//	Error:
//	I2C_GenerateSTOP(I2C1, ENABLE);
//	I2C_AcknowledgeConfig(I2C1, ENABLE);
//	return 0;
//}

//uint8_t MyMPU6050_ReciveDoubelData(uint8_t RegAddress, uint8_t *DataH ,uint8_t *DataL)
//{
//	I2C_AcknowledgeConfig(I2C1, ENABLE);
//	I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
//	
//	I2C_GenerateSTART(I2C1, ENABLE);
//	
//	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT);
//	I2C_Send7bitAddress(I2C1, 0xD0, I2C_Direction_Transmitter);
//	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
//	I2C_SendData(I2C1, RegAddress);  //要读的寄存器地址
//	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
//	I2C_GenerateSTART(I2C1, ENABLE);
//	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT);
//	I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next);
//	I2C_Send7bitAddress(I2C1, 0xD0, I2C_Direction_Receiver);
//	MPU6050_I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
//	
//	I2C_AcknowledgeConfig(I2C1, DISABLE);
//	I2C_GenerateSTOP(I2C1, ENABLE);
//	*DataH = I2C_ReceiveData(I2C1);
//    *DataL = I2C_ReceiveData(I2C1);
//	I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
//	I2C_AcknowledgeConfig(I2C1, ENABLE);

//	return 1;
//}

void MyMPU6050_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;  //选择引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_DeInit(I2C2);
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 200000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C2,&I2C_InitStructure);
	I2C_Cmd(I2C2,ENABLE);
	
	MyMPU6050_SendCommend(MPU6050_PWR_MGMT_1, 0x80);  	//先复位
	Delay_ms(100);										//延时100毫秒，等待复位
	MyMPU6050_SendCommend(MPU6050_PWR_MGMT_1, 0x01);  	//以陀螺仪x轴为时钟源 复位完成，开始工作
	MyMPU6050_SendCommend(MPU6050_PWR_MGMT_2, 0x00);	//管理待机唤醒的，不用
	MyMPU6050_SendCommend(MPU6050_SMPLRT_DIV, 0x04);  	//采样速率 1khz /(4+1) =200hz, 5ms 采样一次
	MyMPU6050_SendCommend(MPU6050_CONFIG, 0x02);		//设置，SYNC关闭，滤波选择的是第2档
	MyMPU6050_SendCommend(MPU6050_GYRO_CONFIG, 0x18); 	//陀螺仪配置 ，自检关闭且量程为± 2000 °/s
	MyMPU6050_SendCommend(MPU6050_ACCEL_CONFIG, 0x00); 	//加速度计 配置 ，自检关闭且量程为± 2g
	yaw = 0.0f;
	roll = 0.0f;
	pitch = 0.0f;
}

uint8_t MyMPU6050_GetID(void)
{
	uint8_t id;
	if(MyMPU6050_ReciveByte(MPU6050_WHO_AM_I, &id) == 1)
	{
		return id;
	}
	return 0;
}

void MyMPU6050_GetData(void)
{	
	uint8_t DataH, DataL;
	// 读取加速度 X
	MyMPU6050_ReciveByte(MPU6050_ACCEL_XOUT_H, &DataH);
	MyMPU6050_ReciveByte(MPU6050_ACCEL_XOUT_H + 1, &DataL);
	ACCX = (DataH << 8) | DataL;
	
	// 读取加速度 Y
	MyMPU6050_ReciveByte(MPU6050_ACCEL_YOUT_H, &DataH);
	MyMPU6050_ReciveByte(MPU6050_ACCEL_YOUT_H + 1, &DataL);
	ACCY = (DataH << 8) | DataL;
	
	// 读取加速度 Z
	MyMPU6050_ReciveByte(MPU6050_ACCEL_ZOUT_H, &DataH);
	MyMPU6050_ReciveByte(MPU6050_ACCEL_ZOUT_H + 1, &DataL);
	ACCZ = (DataH << 8) | DataL;
	
	// 读取陀螺仪 X
	MyMPU6050_ReciveByte(MPU6050_GYRO_XOUT_H, &DataH);
	MyMPU6050_ReciveByte(MPU6050_GYRO_XOUT_H + 1, &DataL);
	GYROX = (DataH << 8) | DataL;
	
	// 读取陀螺仪 Y
	MyMPU6050_ReciveByte(MPU6050_GYRO_YOUT_H, &DataH);
	MyMPU6050_ReciveByte(MPU6050_GYRO_YOUT_H + 1, &DataL);
	GYROY = (DataH << 8) | DataL;
	
	// 读取陀螺仪 Z
	MyMPU6050_ReciveByte(MPU6050_GYRO_ZOUT_H, &DataH);
	MyMPU6050_ReciveByte(MPU6050_GYRO_ZOUT_H + 1, &DataL);
	GYROZ = (DataH << 8) | DataL;
	
	// 读取温度
	MyMPU6050_ReciveByte(MPU6050_TEMP_OUT_H, &DataH);
	MyMPU6050_ReciveByte(MPU6050_TEMP_OUT_H + 1, &DataL);
	TEMP = (DataH << 8) | DataL;
	
	ax = ACCX /16384.0f;    //参数手册有值 每位的灵敏度是16384.0   2g
	ay = ACCY /16384.0f;
	az = ACCZ /16384.0f;
	gx = GYROX /16.4f;  //参数手册有值 每位的灵敏度是 16.4     ±2000º/s
	gy = GYROY /16.4f;
	gz = GYROZ /16.4f;
	temperature = TEMP/340.0f + 35.53f;	//参数手册有值 每位的灵敏度是340 LSB/ºC, -521 LSB 是35ºC的参考值  TEMP -(-521)/340 + 35.53f
	
	float roll_a = atan2(ay,az) / 3.14593f * 180.0f;
	float pitch_a = - atan2(ax,az) / 3.14593f * 180.0f;
	
	float yaw_g = yaw + gz * 0.005f;
	float roll_g = roll + gx * 0.005f;
	float pitch_g = pitch + gy * 0.005f;
	
	yaw = yaw_g;
	roll = alpha * roll_g + (1-alpha) * roll_a;
	pitch = alpha * pitch_g + (1-alpha) * pitch_a;
}

void MyMPU6050_Proc(void)
{
	MyMPU6050_GetData();
	
	static float ValueBuffer[5] = {0};
    static uint8_t Index = 0;
    static uint8_t SampleCount = 0;
    static float Sum = 0.0f;
	
	static float LastValue1 = 0.0f;
    static float LastValue2 = 0.0f;
    static uint8_t FirstFlag = 1;
    static uint32_t LastStepTime = 0;
	
	uint32_t NowTime = GetTik();
	
	float Magnitude = sqrtf(ax * ax + ay * ay + az * az);

	float Value = fabsf(Magnitude - 1.0f);
	
	Sum -= ValueBuffer[Index];
	
	ValueBuffer[Index] = Value;
    Sum += ValueBuffer[Index];
	Index++;
	if (Index >= 5)
    {
        Index = 0;
    }
	if (SampleCount < 5)
    {
        SampleCount++;
    }
	
	if (SampleCount < 5)
    {
        return;
    }
	Value = Sum / 5.0f;
	
	if (FirstFlag)
    {
        LastValue1 = Value;
        LastValue2 = Value;
        FirstFlag = 0;
        return;
    }
	// 检测中间采样点是否为局部峰值
    if (LastValue1 > LastValue2 &&
        LastValue1 >= Value &&
        LastValue1 > 0.40f &&
        (NowTime - LastStepTime >= 1000))
    {
        Count++;
        LastStepTime = NowTime;
    }
    LastValue2 = LastValue1;
    LastValue1 = Value;
//	if(nowtime - starttime >= pdMS_TO_TICKS(5))
//	{
//		MyMPU6050_GetData();
//		starttime += pdMS_TO_TICKS(5);
//	}
}

void MyMPU6050_GetResult(float *pAccelOut, float *pTempOut,float *pGyroOut, float *pEularAngleOut)
{
	pAccelOut[0] = ax;
	pAccelOut[1] = ay;
	pAccelOut[2] = az;
	*pTempOut = temperature;
	pGyroOut[0] = gx;
	pGyroOut[1] = gy;
	pGyroOut[2] = gz;
	pEularAngleOut[0] = yaw;
	pEularAngleOut[1] = roll;
	pEularAngleOut[2] = pitch;
}

void MyMPU6050_GetStepCount(uint32_t *pSteapCount)
{
	*pSteapCount = Count;
}

void MyMPU6050_ClearStepCount(void)
{
    Count = 0;
}
