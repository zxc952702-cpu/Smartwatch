#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "OLED.h"
#include "MyMenu.h"

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

void KeyTask2(void *argument)
{
    while (1)
    {
		MyMenu_MainMenu();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

int main(void)
{
    // 初始化硬件 I2C 及 OLED
    OLED_Init();
//	MyKey_Init();
	MyMenu_Init();
    // 写入显存 Buffer
	MyMenu_MainMenu();
	
//    OLED_ShowString(0, 0, "1.时间", OLED_8X16);
//	OLED_ShowString(0, 15, "2.秒表", OLED_8X16);
//	OLED_ShowString(0, 30, "3.小游戏", OLED_8X16);
//	OLED_ShowString(0, 45, "4.小游戏", OLED_8X16);

    // 刷显存到 OLED 屏幕

    while (1)
    {
//		MyKeyEvent_t Key1event;
//		Key1event = MyKey_GetEvent(GPIOA,GPIO_Pin_0, &key1struct);
//		if(Key1event == KEY_Single)
//		{
//			OLED_Clear();
//			OLED_ShowString(0, 0, "1.", OLED_8X16);
//			OLED_Update();
//		}
//		else if(Key1event == KEY_Double)
//		{
//			OLED_Clear();
//			OLED_ShowString(0, 0, "2.", OLED_8X16);
//			OLED_Update();
//		}
//		else if(Key1event == KEY_Longtype)
//		{
//			OLED_Clear();
//			OLED_ShowString(0, 0, "3.", OLED_8X16);
//			OLED_Update();
//		}
        // 软件主循环
    }
}
