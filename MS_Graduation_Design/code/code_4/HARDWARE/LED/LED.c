/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: 门前照亮灯，通过人体感应模块控制，引脚PF9，PF10
 * @Date: 2023-11-29 15:28:51
 * @LastEditTime: 2023-12-01 02:56:16
 * @FilePath: \USERe:\Graduation Design\code\code\HARDWARE\LED\LED.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "LED.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;

    GPIO_Init(GPIOF,&GPIO_InitStructure);

    //初始化灯不亮
    LED0=1;
    LED1=1;
   
}

