/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: �������ⲿ�жϷ�ʽ���������죨KEY1�������ԣ�KEY2������KEY0���������Թ���
 * @Date: 2023-11-29 15:19:50
 * @LastEditTime: 2024-01-26 00:23:44
 * @FilePath: \USERe:\MS_Graduation Design\code\code\HARDWARE\KEY\KEY.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "KEY.h"

extern u8 KEY_GSM_flag;
extern u8 KEY_WS2812B_flag;
extern u8 KEY_Buzzer_flag;

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructyre;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructyre.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructyre.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructyre.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructyre.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_InitStructyre.GPIO_Pin = GPIO_Pin_0;      // WK_UP��Ӧ����PA0
    GPIO_InitStructyre.GPIO_PuPd = GPIO_PuPd_DOWN; // ����

    GPIO_Init(GPIOA, &GPIO_InitStructyre);
}

void KEY_EXIT_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    // �ⲿ�жϣ����������ų�ʼ��
    KEY_Init();

    // ʹ��SYSCFGʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0); // PA0 ���ӵ��ж���0
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1); // PA1 ���ӵ��ж���1
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2); // PA2 ���ӵ��ж���2

    /* ����EXTI_Line0 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;             // LINE0
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // �ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // �����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              // ʹ��LINE0
    EXTI_Init(&EXTI_InitStructure);                        // ����

    /* ����EXTI_Line2,3,4 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     // �ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // �½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;               // �ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);                         // ����

    // ���ȼ�
    /**
     * @Description: ���ȼ�����ԽС�����ȼ�Խ�ߣ��˴����ȼ�ΪPA0>PA1>PA2
     * @return {*}
     * @Author: ��ˬ
     * @Date: 2023-11-30 06:07:39
     */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @Description: �ⲿ�ж�0������,��������
 * @return {*}
 * @Author: ��ˬ
 * @Date: 2023-11-30 06:14:27
 */
void EXTI0_IRQHandler(void)
{
    // ��Ӧ�������
    delay_ms(10);
    // �������Ե�
    if (KEY0 == 1)
    {
        KEY_Buzzer_flag = 1;
    }

    EXTI_ClearITPendingBit(EXTI_Line0); // ����жϱ�־
}

/**
 * @Description: �ⲿ�ж�1��������������������
 * @return {*}
 * @Author: ��ˬ
 * @Date: 2023-11-30 06:38:29
 */
void EXTI1_IRQHandler()
{

    // ��Ӧ�������
    delay_ms(10);
    // �������Ե�
    if (KEY1 == 0)
    {
        KEY_GSM_flag=1;
    }

    EXTI_ClearITPendingBit(EXTI_Line1); // ����жϱ�־
}

/**
 * @Description: �ⲿ�ж�2������������¼������
 * @return {*}
 * @Author: ��ˬ
 * @Date: 2023-11-30 06:39:13
 */
void EXTI2_IRQHandler()
{

    // ��Ӧ�������
    delay_ms(10);

    EXTI_ClearITPendingBit(EXTI_Line2); // ����жϱ�־
}
