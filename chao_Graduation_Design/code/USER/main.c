/*
 * @Author: ��ˬ
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

// �ƹ�ǿ�ȵȼ�
int LED_Level[11] = {0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90};

// �ȼ������־λ
int Levei_flag = 5;

float SR_Distance = 0; // ���������

//
u8 BlueTooth_flag = 1;
u8 GuangMin_flag = 0;
u8 ChaoShengBo_flag = 0;
u8 Zeng_flag = 0;
u8 Jian_flag = 0;

// ������ʱ������������50��
u8 BlueTooth_count = 0;
u8 GuangMin_count = 0;
u8 ChaoShengBo_count = 0;
u8 Zeng_count = 0;
u8 Jian_count = 0;

// �����Ƿ�ʹ������
u8 BlueTooth_key = 0;
u8 GuangMin_key = 0;
u8 ChaoShengBo_key = 0;
u8 Zeng_key = 0;
u8 Jian_key = 0;

int pwmValue = 0;

// ��ʱ��2�ļ�����
uint16_t timer = 0;

int main(void)
{
	delay_init();									// ��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	Hcsr04Init();									// ��������ʼ��
	OLED_Init();									// ��ʼ��OLED
	LED_Init(999, 719);								// �Ƴ�ʼ��
	USART_Config();									// ������ʼ��
	OLED_MENU();
	LM393_Init();
	Timer1_Init(999, 719); // tim1��72MHz,72/1000/720=1000Hz  1/1000=1ms
	KEY_Init();
	Beep_Init(); // ��������ʼ��
	while (1)
	{
		SR_Distance = Hcsr04GetLength();
		OLED_Show();
		if (BlueTooth_flag == 1)
		{
			OLED_ShowCHinese(43, 0, 4);	 // ���޸�ֵ    ������
			if (GuangMin_flag == 1)
			{
				TIM_SetCompare2(TIM3, 0); // PB.5 �����
			}
			else
			{
				TIM_SetCompare2(TIM3, LED_Level[Levei_flag]);
			}

			// ����С��10���׷�������
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
			OLED_ShowCHinese(43, 0, 3);	 // ���޸�ֵ    ������
			TIM_SetCompare2(TIM3, 0); // PB.5 �����
			GPIO_ResetBits(GPIOB, GPIO_Pin_4);
			
		}
	}
}
