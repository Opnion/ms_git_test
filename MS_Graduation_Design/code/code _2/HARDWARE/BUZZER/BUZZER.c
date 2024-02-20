/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-11-29 15:26:26
 * @LastEditTime: 2023-12-22 23:29:12
 * @FilePath: \USERe:\MS_Graduation Design\code\code\HARDWARE\BUZZER\BUZZER.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "BUZZER.h"

void Buzzer_Init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;            
   
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;

    GPIO_Init(GPIOC,&GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_3);// ����ʹ��
    GPIO_ResetBits(GPIOC,GPIO_Pin_2);// ����ʹ��
}

// Tout = ((arr+1)*(psc+1))/Tclk;psc���Ƿ�Ƶϵ����
//arr���Ǽ���ֵ���ﵽ��������ͻᷢ������жϣ�Tclk��������������ʱ��ԴƵ�ʡ�
void TIM2_Init(u16 arr,u16 psc)
{
     TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
      NVIC_InitTypeDef NVIC_InitStructure;
   //ʹ�ܶ�ʱ��2Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//���ö�ʱ��2��Ƶֵ������ֵ
	TIM_TimeBaseStructure.TIM_Period = arr-1;//35-1;//40-1;//����ֵ100ms;arr
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;//4200-1;//1050-1;//����8400��Ԥ��Ƶֵ 42*2Mhz/8400=10000hz 10000�μ�������1��;psc
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//��f407��֧�֣�û��ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//���ö�ʱ��2�жϵĴ�����ʽ
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);

	//���ö�ʱ��2���ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//ʹ�ܶ�ʱ��2����
	TIM_Cmd(TIM2, ENABLE);
}



void TIM2_IRQHandler(void)
{
	//�жϱ�־λ
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		GPIO_ToggleBits(GPIOC,GPIO_Pin_3);
		//��ձ�־λ
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
