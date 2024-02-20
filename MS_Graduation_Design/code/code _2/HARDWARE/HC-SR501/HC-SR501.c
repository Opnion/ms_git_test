/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: �����Ӧģ������ģ�飬�ӿڣ�5V��GND��OUT��PA11��
 * @Date: 2023-11-29 15:23:33
 * @LastEditTime: 2023-12-01 02:42:03
 * @FilePath: \USERe:\Graduation Design\code\code\HARDWARE\HC-SR501\HC-SR501.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "HC-SR501.h"

void Telemeter_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;//GPIO_Pin_11
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

}


void Telemeter_EXIT_Init(void)
{
    EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    //��ʼ��IO��
    Telemeter_Init();

    //ʹ��SYSCFGʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);//PA0 ���ӵ��ж���0

    //�ж�����
    EXTI_InitStructure.EXTI_Line=EXTI_Line4;//EXTI_Line11
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;//�������ߵ��͵�ƽ�����ƣ���Զ�룺�͵��ߵ�ƽ����ƣ�
    EXTI_Init(&EXTI_InitStructure);

    //���ȼ�����
    NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;//EXTI15_10_IRQn()
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
    NVIC_Init(&NVIC_InitStructure);
}



void EXTI4_IRQHandler(void)
{
        //��Ӧ����
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)!=1)
        {
            LED0=0;
        }else
        {
            LED0=1;
        }
        
		EXTI_ClearITPendingBit(EXTI_Line4);//����жϱ�־
}

