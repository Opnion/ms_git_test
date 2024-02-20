/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: 人体感应模块或红外模块，接口：5V、GND、OUT（PA11）
 * @Date: 2023-11-29 15:23:33
 * @LastEditTime: 2024-01-27 00:51:06
 * @FilePath: \USERe:\MS_Graduation Design\code\code\HARDWARE\HC-SR501\HC-SR501.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "HC-SR501.h"

extern u8 KEY_Buzzer_flag;

void Telemeter_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Telemeter_EXIT_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 初始化IO口
    Telemeter_Init();

    // 使能SYSCFG时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource11); // PA0 连接到中断线0

    // 中断设置
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 靠近：高到低电平（亮灯）；远离：低到高电平（灭灯）
    EXTI_Init(&EXTI_InitStructure);

    // 优先级设置
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_InitStructure);
}



void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line11) == 0) // 确保是否产生了EXTI Line中断
    {
       // KEY_Buzzer_flag = 1;
        EXTI_ClearITPendingBit(EXTI_Line11); // 清除中断标志位
    }
}


