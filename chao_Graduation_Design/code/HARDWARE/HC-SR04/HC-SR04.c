#include "sys.h"
// 超声波硬件接口定义
#define HCSR04_PORT GPIOA
#define HCSR04_CLK RCC_APB2Periph_GPIOA
#define HCSR04_TRIG GPIO_Pin_3
#define HCSR04_ECHO GPIO_Pin_2
#define ECHO_Reci PAin(2)
#define TRIG_Send PAout(3)

// 超声波计数
u16 msHcCount = 0;

void hcsr04_NVIC()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// IO口初始化 及其他初始化
void Hcsr04Init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);

    GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT, HCSR04_ECHO);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = (1000 - 1);
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    hcsr04_NVIC();
    TIM_Cmd(TIM2, DISABLE);
}

// 打开定时器3
static void OpenTimerForHc()
{
    TIM_SetCounter(TIM2, 0);
    msHcCount = 0;
    TIM_Cmd(TIM2, ENABLE);
}

// 关闭定时器3
static void CloseTimerForHc()
{
    TIM_Cmd(TIM2, DISABLE);
}

// 定时器3终中断
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        msHcCount++;
    }
}

// 获取定时器3计数器值
u32 GetEchoTimer(void)
{
    u32 t = 0;
    t = msHcCount * 1000;
    t += TIM_GetCounter(TIM2);
    TIM2->CNT = 0;
    delay_ms(50);
    return t;
}

// 通过定时器3计数器值推算距离
float Hcsr04GetLength(void)
{
    u32 t = 0;
    int i = 0;
    float lengthTemp = 0;
    float sum = 0;
    while (i != 5)
    {
        TRIG_Send = 1;
        delay_us(20);
        TRIG_Send = 0;
        while (ECHO_Reci == 0)
            ;
        OpenTimerForHc();
        i = i + 1;
        while (ECHO_Reci == 1)
            ;
        CloseTimerForHc();
        t = GetEchoTimer();
        lengthTemp = ((float)t / 58.0); // cm
        sum = lengthTemp + sum;
    }
    lengthTemp = sum / 5.0;
    
    return lengthTemp;
}
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}
