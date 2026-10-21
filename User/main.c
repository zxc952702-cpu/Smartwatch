#include "stm32f10x.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h" 
#include "OLED.h"
#include "Delay.h"
#include "MyMenu.h"
#include "MyTime.h"
#include "MyI2C.h"
#include "PassiveBuzzer.h"
#include "MyMenuPedometer.h"
#include "MyMenuStopWatch.h"
#include "MyMPU6050.h"
#include "MyKey.h"


//void KeyTask(void *argument)
//{
//    while (1)
//    {
//        MyKeyEvent_t Key1event;

//        Key1event = MyKey_GetEvent(
//            GPIOA,
//            GPIO_Pin_0,
//            &key1struct
//        );

//        if (Key1event == KEY_Single)
//        {
//            OLED_Clear();
//            OLED_ShowString(0, 0, "1.", OLED_8X16);
//            OLED_Update();
//        }
//        else if (Key1event == KEY_Double)
//        {
//            OLED_Clear();
//            OLED_ShowString(0, 0, "2.", OLED_8X16);
//            OLED_Update();
//        }
//        else if (Key1event == KEY_Longtype)
//        {
//            OLED_Clear();
//            OLED_ShowString(0, 0, "3.", OLED_8X16);
//            OLED_Update();
//        }

//        vTaskDelay(pdMS_TO_TICKS(5));
//    }
//}

//SemaphoreHandle_t xI2CMutex;

//void My_MenuTask1(void *argument)
//{
//    while (1)
//    {
////		if(xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(5)) == 1)
////        {
//            MyMenu_LevelAll();
////            xSemaphoreGive(xI2CMutex);
////        }
//		RTCAlarm_Work();
//		vTaskDelay(pdMS_TO_TICKS(5));
//    }
//}

//void MyMPU6050Task1(void *argument)
//{
//	TickType_t xLastWakeTime = xTaskGetTickCount();
//	const TickType_t xPeriod = pdMS_TO_TICKS(5); 
//    while (1)
//    {
//		vTaskDelayUntil(&xLastWakeTime, xPeriod);
////		if(xSemaphoreTake(xI2CMutex,pdMS_TO_TICKS(2)) == 1)
////		{
//			MyMPU6050_Proc();
////			xSemaphoreGive(xI2CMutex);
////		}
////		
//    }
//}



int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//	MyI2C_Hardware_Init();
//	OLED_Init();
	MyI2C_Hardware_Init();
	MyMenu_Init();
	MyKey_Init();
	MyMenuPedometer_Init();
//	MyKey_Init();
//	MyTime_Init();
//	MyMenuStopWatch_Init();
//	PassiveBuzzer_Init();
	
//	xI2CMutex = xSemaphoreCreateMutex();
//	
//	TaskHandle_t My_MenuTask1_Handle;
//	TaskHandle_t MyMPU6050Task1_Handle;
	
//	xTaskCreate(My_MenuTask1,"My_MenuTask1",128, NULL,1,&My_MenuTask1_Handle);
//	xTaskCreate(MyMPU6050Task1,"MyMPU6050Task1",128, NULL,3,&MyMPU6050Task1_Handle);
//	vTaskStartScheduler();
    while (1)
    {
		MyMenu_LevelAll();
		MyMPU6050_Proc();
		OLED_EnergyTask();
		RTCAlarm_Work();
		Delay_ms(5);
    }
}

//typedef enum{
//	KEYEvent_None = 0,
//	KEYEvent_Single,
//    KEYEvent_Double,
//    KEYEvent_Longtype,
//	KEYEvent_LongTimeOut,
//} MyKeyEvent_t;

