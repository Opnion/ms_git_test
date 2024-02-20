/*
 * @Author: 马爽
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
    //**结构体声明**//
	GPIO_InitTypeDef GPIO_InitStructure;            //声明GPIO
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; //声明定时器
	TIM_OCInitTypeDef  TIM_OCInitStructure;         //声明PWM通道
	
	//**时钟使能**//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器TIM4时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
	//****//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PB.6 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.6
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	//****//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //LED0-->PB.7 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.7
	GPIO_SetBits(GPIOB,GPIO_Pin_7);

  //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//初始化TIM3 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4_CH1预装载寄存器
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4_CH2预装载寄存器
   
   TIM_CtrlPWMOutputs(TIM4,ENABLE);  //MOE 主输出使能 
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1预装载使能 
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1预装载使能     
  
  TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIM3

//     //舵机旋转90度
//       TIM_SetCompare1(TIM4,1950);		//TIM4->CCR1=400					 
//		TIM_SetCompare2(TIM4,1900);	    //TIM4->CCR2=800
//        delay_ms(500);
//        TIM_SetCompare1(TIM4,1850);		//TIM4->CCR1=400					 
//		TIM_SetCompare2(TIM4,1800);	    //TIM4->CCR2=800
//        delay_ms(500);
}













