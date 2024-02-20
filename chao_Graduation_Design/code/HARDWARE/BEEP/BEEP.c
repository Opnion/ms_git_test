/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:56:54
 * @LastEditTime: 2024-01-17 15:30:06
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\BEEP\BEEP.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "sys.h"

void Beep_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	// PB4默认用作调试口，如果用作普通的IO，需要加上以下两句
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能B端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // 速度50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  // 初始化GPIOB8,9
	//GPIO_SetBits(GPIOB, GPIO_Pin_4);
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
}
