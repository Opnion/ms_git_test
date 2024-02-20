/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-11-29 15:26:26
 * @LastEditTime: 2023-12-22 23:29:12
 * @FilePath: \USERe:\MS_Graduation Design\code\code\HARDWARE\BUZZER\BUZZER.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "BUZZER.h"

void Buzzer_Init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;            
   
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;

    GPIO_Init(GPIOC,&GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_3);// 开启使能
    GPIO_ResetBits(GPIOC,GPIO_Pin_2);// 开启使能
}

// Tout = ((arr+1)*(psc+1))/Tclk;psc就是分频系数，
//arr就是计数值，达到这个计数就会发生溢出中断，Tclk就是上述分析的时钟源频率。
void TIM2_Init(u16 arr,u16 psc)
{
     TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
      NVIC_InitTypeDef NVIC_InitStructure;
   //使能定时器2硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//配置定时器2分频值、计数值
	TIM_TimeBaseStructure.TIM_Period = arr-1;//35-1;//40-1;//计数值100ms;arr
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;//4200-1;//1050-1;//进行8400的预分频值 42*2Mhz/8400=10000hz 10000次计数就是1秒;psc
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//在f407不支持，没有时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//配置定时器2中断的触发方式
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);

	//配置定时器2的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//使能定时器2工作
	TIM_Cmd(TIM2, ENABLE);
}



void TIM2_IRQHandler(void)
{
	//判断标志位
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		GPIO_ToggleBits(GPIOC,GPIO_Pin_3);
		//清空标志位
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
