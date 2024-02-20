/*
 * @Author: ÂíË¬
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:57:38
 * @LastEditTime: 2023-12-29 04:02:36
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\LM393\LM393.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "LM393.h"

void LM393_Init(void)
{
    GPIO_InitTypeDef   GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
}


