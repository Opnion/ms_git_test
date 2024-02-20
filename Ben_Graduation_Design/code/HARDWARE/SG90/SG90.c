/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: B7    B14
 * @Date: 2023-12-14 00:03:45
 * @LastEditTime: 2023-12-14 01:21:18
 * @FilePath: \code\HARDWARE\SG90\SG90.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */


#include "sys.h" 



void SG90_Init(u16 arr,u16 psc)
{
    //**�ṹ������**//
	GPIO_InitTypeDef GPIO_InitStructure;            //����GPIO
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; //������ʱ��
	TIM_OCInitTypeDef  TIM_OCInitStructure;         //����PWMͨ��
	
	//**ʱ��ʹ��**//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��TIM4ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
	//****//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PB.6 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.6
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	//****//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //LED0-->PB.7 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.7
	GPIO_SetBits(GPIOB,GPIO_Pin_7);

  //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM3 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM4_CH1Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM4_CH2Ԥװ�ؼĴ���
   
   TIM_CtrlPWMOutputs(TIM4,ENABLE);  //MOE �����ʹ�� 
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ�� 
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��     
  
  TIM_ARRPreloadConfig(TIM4, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM3

//     //�����ת90��
//       TIM_SetCompare1(TIM4,1950);		//TIM4->CCR1=400					 
//		TIM_SetCompare2(TIM4,1900);	    //TIM4->CCR2=800
//        delay_ms(500);
//        TIM_SetCompare1(TIM4,1850);		//TIM4->CCR1=400					 
//		TIM_SetCompare2(TIM4,1800);	    //TIM4->CCR2=800
//        delay_ms(500);
}













