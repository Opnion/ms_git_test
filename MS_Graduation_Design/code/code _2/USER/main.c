/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-11-29 15:13:24
 * @LastEditTime: 2023-12-22 23:31:54
 * @FilePath: \USER\main.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "sys.h"

//
u8 KEY_GSM_flag=0;
 u8   KEY_WS2812B_flag=0;
 u8 KEY_Buzzer_flag=0;

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);
	LED_Init();
	KEY_EXIT_Init();       				//按键初始化外部中断
	Telemeter_EXIT_Init();				//测距初始化外部中断
	GSM_Init(9600);
	RGB_LED_Init();
	Buzzer_Init();
	TIM2_Init(1000,8400);
	while(1)
	{
		
	}
}
