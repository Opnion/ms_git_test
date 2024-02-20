/*
 * @Author: ��ˬ
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

    GPIO_InitTypeDef GPIO_InitStructure;           // ����GPIO�ṹ��
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // ����TIMx��ʱ���ṹ��
    TIM_OCInitTypeDef TIM_OCInitStructure;         // ���嶨ʱ��������ƽṹ��

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                        // ʹ��TIM3ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // ʹ��GPIOBʱ�Ӻ�AFIO����ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // TIM3������ӳ�� TIM3_CH2->PB5

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;         // TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // �����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // ��ʼ��GPIOB

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;         // TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // �����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);            // ��ʼ��GPIOB

    TIM_TimeBaseStructure.TIM_Period = arr;                     // �����Զ���װ�ؼĴ������ڵ�ֵ arr=value-1
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // ����Ԥ��Ƶֵ psc=value-1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             // ��ʼ��TIMxʱ�����
    


    // ��ʼ��TIM3 Channel2 PWMģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ʹ�ܱȽ����
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // �������:TIM����Ƚϼ��Ը�
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);                      // ����Tָ���Ĳ�����ʼ������TIM3 OC2

    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
    TIM_Cmd(TIM3, ENABLE);                            // ʹ��TIM3
}

