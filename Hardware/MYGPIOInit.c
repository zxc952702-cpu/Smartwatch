#include "stm32f10x.h"                  // Device header

uint32_t SIR_CODE;
uint8_t itflag = 0;
uint16_t Infrared_Data(void);
typedef struct{
	uint16_t key_code;
	uint8_t  final_value;
} Infrared_Key_Map_Struct;

Infrared_Key_Map_Struct Infrared_Key_Map[]={
	{0xE916,0},
	{0xE619,100},
	{0xF20D,200},
	{0xF30C,1},
	{0xE718,2},
	{0xA15E,3},
	{0xF708,4},
	{0xE31C,5},
	{0xA55A,6},
	{0xBD42,7},
	{0xAD52,8},	
	{0xB54A,9},
	{0xBA45,101},
	{0xB946,102},
	{0xB847,103},
	{0xBB44,104},
	{0xBF40,105},
	{0xBC43,106},
	{0xF807,107},
	{0xEA15,108},
	{0xF609,109},
};

uint8_t get_IRS_VELUE(void)
{
	uint16_t temp = Infrared_Data();
	if(temp != 0xFFFF)
	{
		for(uint8_t i = 0;i<((sizeof(Infrared_Key_Map))/(sizeof(Infrared_Key_Map[0])));i++)
		{
			if(temp == (Infrared_Key_Map[i].key_code))
			{
				return Infrared_Key_Map[i].final_value;
			}
		}
	}
	return 0xFF;
}

// ==========================================
// 1. 纯软件粗略延时，不依赖任何系统中断
// 彻底解决在 EXTI 中断内部调用传统 Delay_us 导致的卡死
// ==========================================
void Delay_Software_us(uint32_t us)
{
    // 针对 STM32F103 主频 72MHz 优化，大约循环 12 次为 1微秒
    uint32_t count = us * 12;
    while(count--);
}

// ==========================================
// 2. GPIO 初始化函数
// ==========================================
void MYGPIO_Init(GPIO_TypeDef *GPIO_portx, uint16_t GPIO_Pin_x, GPIOMode_TypeDef GPIO_Mode)
{
    GPIO_InitTypeDef GPIO_InitSrtucture;
    GPIO_InitSrtucture.GPIO_Mode = GPIO_Mode;
    GPIO_InitSrtucture.GPIO_Pin = GPIO_Pin_x;
    GPIO_InitSrtucture.GPIO_Speed = GPIO_Speed_50MHz;
    
    if(GPIO_portx == GPIOA)       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if(GPIO_portx == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if(GPIO_portx == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_Init(GPIO_portx, &GPIO_InitSrtucture);
}

// ==========================================
// 3. NVIC 与 EXTI 外部中断初始化
// ==========================================
void MyNvic_Init(void)
{
    // 必须开启 AFIO 复用时钟，否则无法将 GPIO 映射到 EXTI
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); // 映射 GPIOA_Pin_1 到 EXTI 线1
    
    // 配置 EXTI 线1
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_Init(&EXTI_InitStructure);
    
    // 配置 NVIC 中断优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
}

// ==========================================
// 4. 测量高电平时间（带超时保护，防死循环）
// ==========================================
uint8_t SIR_ReadHighTime(void)
{
    uint8_t t = 0;
    // 当引脚保持高电平时计数。每 20us 检查一次。
    // 如果持续高电平超过 5ms (250 * 20us)，说明波形异常，强制退出
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1)
    {
        Delay_Software_us(20); 
        t++;
        if(t > 250) return 251; // 返回错误标示
    }
    return t;
}

// 获取数据接口
uint32_t SIR_GETData(void)
{
    return SIR_CODE;
}

// 获取标志位接口
uint8_t get_itflag(void)
{
    return itflag;
}

// ==========================================
// 5. 绝对不卡死的外部中断服务函数
// ==========================================
void EXTI1_IRQHandler(void)
{
    uint8_t time;
    uint8_t data;
    uint32_t raw_code = 0;
    uint32_t timeout_cnt = 0; // 超时安全计数器
    
    if(EXTI_GetITStatus(EXTI_Line1) == SET)
    {
        // 【防卡死点 1】：等待引导码前段的低电平（约9ms）结束
        // 如果该引脚由于噪声或断开持续维持低电平超过 12ms (2400 * 5us)，强行退出
        timeout_cnt = 0;
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
        {
            Delay_Software_us(5);
            timeout_cnt++;
            if(timeout_cnt > 2400) goto ERROR_EXIT; 
        }
        
        // 测量引导码的高电平时间
        time = SIR_ReadHighTime();
        if(time > 180 && time < 240) // 判定为合法的引导码 (约 3.6ms ~ 4.8ms)
        {
            for(uint8_t i = 0; i < 32; i++)
            {
                // 【防卡死点 2】：等待每位数据开始前的 560us 低电平
                // 如果超过 2ms (400 * 5us) 未变高电平，视为信号中断，退出
                timeout_cnt = 0;
                while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
                {
                    Delay_Software_us(5);
                    timeout_cnt++;
                    if(timeout_cnt > 400) goto ERROR_EXIT;
                }
                
                // 测量此位数据的高电平持续时间
                time = SIR_ReadHighTime();
                
                if(time > 15 && time < 45)       data = 0; // '0' 的高电平约 560us (实际约28次计数)
                else if(time > 55 && time < 100) data = 1; // '1' 的高电平约 1680us (实际约84次计数)
                else goto ERROR_EXIT;                      // 无效的高电平时间，直接退出
                
                // 写入数据（标准的 NEC 协议采用低位先行 LSB 格式）
                raw_code |= ((uint32_t)data << i); 
            }
            
            // 如果 32 位都成功且正确地接收完毕，更新数据并置位标志位
            SIR_CODE = raw_code;
            itflag = 1; 
        }
        
    ERROR_EXIT:
        // 【最核心】：无论中途哪个环节出错跳出，都必须在退出中断前清除挂起标志，否则会死循环
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

uint16_t Infrared_Data(void)
{
	uint32_t temp = SIR_CODE; 				//11000000 & 00000011
	if(((temp & 0xFF) == 0x00) && (((temp>>8) & 0xFF) == 0xFF))
	{
		temp >>= 16;
		return (uint16_t)temp;
	}
	else
	{
		return 0xFFFF;
	}

}
