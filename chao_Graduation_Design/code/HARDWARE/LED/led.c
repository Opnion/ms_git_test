/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:52:05
 * @LastEditTime: 2023-12-29 05:45:30
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\LED\led.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "sys.h"


extern u8 BlueTooth_flag ;
extern u8 GuangMin_flag ;
extern u8 ChaoShengBo_flag ;
extern u8 Key_flag ;

extern u16 pwmValue ;
void LED_Init(u16 arr, u16 psc)
{

    GPIO_InitTypeDef GPIO_InitStructure;           // 定义GPIO结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // 定义TIMx定时器结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;         // 定义定时器脉宽调制结构体

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                        // 使能TIM3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 使能GPIOB时钟和AFIO复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // TIM3部分重映射 TIM3_CH2->PB5

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;         // TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // 配置输出速率
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // 初始化GPIOB

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;         // TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // 配置输出速率
    GPIO_Init(GPIOC, &GPIO_InitStructure);            // 初始化GPIOB

    TIM_TimeBaseStructure.TIM_Period = arr;                     // 设置自动重装载寄存器周期的值 arr=value-1
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 设置预分频值 psc=value-1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             // 初始化TIMx时间基数
    


    // 初始化TIM3 Channel2 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // 选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能比较输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 输出极性:TIM输出比较极性高
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);                      // 根据T指定的参数初始化外设TIM3 OC2

    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // 使能TIM3在CCR2上的预装载寄存器
    TIM_Cmd(TIM3, ENABLE);                            // 使能TIM3
}

