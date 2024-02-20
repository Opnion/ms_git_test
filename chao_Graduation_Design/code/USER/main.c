/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:52:06
 * @LastEditTime: 2024-01-18 02:01:25
 * @FilePath: \USER\main.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "timer.h"

// 灯光强度等级
int LED_Level[11] = {0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90};

// 等级数组标志位
int Levei_flag = 5;

float SR_Distance = 0; // 超声波测距

//
u8 BlueTooth_flag = 1;
u8 GuangMin_flag = 0;
u8 ChaoShengBo_flag = 0;
u8 Zeng_flag = 0;
u8 Jian_flag = 0;

// 按键定时器消抖计数，50次
u8 BlueTooth_count = 0;
u8 GuangMin_count = 0;
u8 ChaoShengBo_count = 0;
u8 Zeng_count = 0;
u8 Jian_count = 0;

// 按键是否使能启用
u8 BlueTooth_key = 0;
u8 GuangMin_key = 0;
u8 ChaoShengBo_key = 0;
u8 Zeng_key = 0;
u8 Jian_key = 0;

int pwmValue = 0;

// 定时器2的计数器
uint16_t timer = 0;

int main(void)
{
	delay_init();									// 延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Hcsr04Init();									// 超声波初始化
	OLED_Init();									// 初始化OLED
	LED_Init(999, 719);								// 灯初始化
	USART_Config();									// 蓝牙初始化
	OLED_MENU();
	LM393_Init();
	Timer1_Init(999, 719); // tim1是72MHz,72/1000/720=1000Hz  1/1000=1ms
	KEY_Init();
	Beep_Init(); // 蜂鸣器初始化
	while (1)
	{
		SR_Distance = Hcsr04GetLength();
		OLED_Show();
		if (BlueTooth_flag == 1)
		{
			OLED_ShowCHinese(43, 0, 4);	 // 待修改值    开、关
			if (GuangMin_flag == 1)
			{
				TIM_SetCompare2(TIM3, 0); // PB.5 输出高
			}
			else
			{
				TIM_SetCompare2(TIM3, LED_Level[Levei_flag]);
			}

			// 距离小于10厘米蜂鸣器响
			if (SR_Distance <= 10)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_4);
				// GPIO_ResetBits(GPIOB, GPIO_Pin_4);
			}
			else
			{
				GPIO_ResetBits(GPIOB, GPIO_Pin_4);
			}
		}
		else if (BlueTooth_flag == 0)
		{
			OLED_ShowCHinese(43, 0, 3);	 // 待修改值    开、关
			TIM_SetCompare2(TIM3, 0); // PB.5 输出高
			GPIO_ResetBits(GPIOB, GPIO_Pin_4);
			
		}
	}
}
