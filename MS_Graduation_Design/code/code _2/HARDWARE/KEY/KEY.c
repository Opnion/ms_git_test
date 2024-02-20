/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: 按键用外部中断方式控制门铃响（KEY1）、留言（KEY2）；（KEY0）用来测试功能
 * @Date: 2023-11-29 15:19:50
 * @LastEditTime: 2023-12-28 01:05:42
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
     GPIO_InitTypeDef  GPIO_InitStructyre;
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

     GPIO_InitStructyre.GPIO_Mode=GPIO_Mode_IN;
     GPIO_InitStructyre.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2;
     GPIO_InitStructyre.GPIO_PuPd=GPIO_PuPd_DOWN;
     GPIO_InitStructyre.GPIO_Speed=GPIO_Speed_100MHz;
    
    GPIO_InitStructyre.GPIO_Pin = GPIO_Pin_0;//WK_UP对应引脚PA0
    GPIO_InitStructyre.GPIO_PuPd = GPIO_PuPd_DOWN ;//下拉

     GPIO_Init(GPIOA,&GPIO_InitStructyre);


}





void KEY_EXIT_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    //外部中断（按键）引脚初始化
    KEY_Init();

    //使能SYSCFG时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 连接到中断线0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);//PA1 连接到中断线1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);//PA2 连接到中断线2

   

     /* 配置EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
  EXTI_Init(&EXTI_InitStructure);//配置
	
	/* 配置EXTI_Line2,3,4 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
  EXTI_Init(&EXTI_InitStructure);//配置
  
    //优先级
    /**
     * @Description: 优先级数字越小，优先级越高，此处优先级为PA0>PA1>PA2
     * @return {*}
     * @Author: 马爽
     * @Date: 2023-11-30 06:07:39
     */    
    NVIC_InitStructure.NVIC_IRQChannel=EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel=EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel=EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
    NVIC_Init(&NVIC_InitStructure);

    
}



/**
 * @Description: 外部中断0服务函数,用于测试GSM和拍照代码
 * @return {*}
 * @Author: 马爽
 * @Date: 2023-11-30 06:14:27
 */
void EXTI0_IRQHandler(void)
{
		//响应处理代码
        delay_ms(10);
        //按键测试灯
        if(KEY0==1)
        {
            if(KEY_WS2812B_flag==0)
            {
                RGB_LED_Write_24Bits(255,0,0,0,255,0);
                KEY_WS2812B_flag++;
            }else if(KEY_WS2812B_flag==1)
            {
                RGB_LED_Write_24Bits(0,255,0,0,0,255);
                KEY_WS2812B_flag++;
            }else if(KEY_WS2812B_flag==2)
            {
                RGB_LED_Write_24Bits(0,0,255,255,0,0);
                KEY_WS2812B_flag=0;
            }

        }
        
    EXTI_ClearITPendingBit(EXTI_Line0);//清除中断标志
}



/**
 * @Description: 外部中断1服务函数，用于门铃提醒
 * @return {*}
 * @Author: 马爽
 * @Date: 2023-11-30 06:38:29
 */
void EXTI1_IRQHandler()
{
   
		//响应处理代码
         delay_ms(10);
        //按键测试灯
        if(KEY1==0)
        {
            LED1=!LED1;
            USART3_Send_data("AT+CSCS=\"GSM\"\r\n");	//短信格式设置为文本模式
            delay_ms(2000);
           USART3_Send_data("AT+CMGF=1\r\n");	//短信格式设置为文本模式
            delay_ms(2000);//这里的延时可以更改但不能低于500MS如果太快模块接收不了
            USART3_Send_data("AT+CMGS=\"18029745289\"\r\n");//短信接受号码
            delay_ms(2000);
            USART3_Send_data("hello_word\r\n");//发送短信内容
            delay_ms(5000); 
            USART3_Send_data_value(0x1a); //发送回车结束符


            
        }
        
		EXTI_ClearITPendingBit(EXTI_Line1);//清除中断标志
	

}


/**
 * @Description: 外部中断2服务函数，用于录音留言
 * @return {*}
 * @Author: 马爽
 * @Date: 2023-11-30 06:39:13
 */
void EXTI2_IRQHandler()
{

		//响应处理代码
        delay_ms(10);

            TIM2_Init(60,1300);
            RGB_LED_Write_24Bits(100,0,255,100,50,0);//两个绿
			GPIO_SetBits(GPIOC,GPIO_Pin_2);// 开启使能
			delay_ms(500);
			GPIO_ResetBits(GPIOC,GPIO_Pin_2);// 开启使能
            delay_ms(100);

            TIM2_Init(25,4500);
            RGB_LED_Write_24Bits(0,255,50,50,100,0);//两个绿
			GPIO_SetBits(GPIOC,GPIO_Pin_2);// 开启使能
			delay_ms(500);
			GPIO_ResetBits(GPIOC,GPIO_Pin_2);// 开启使能
            delay_ms(100);
            RGB_LED_Write_24Bits(0,0,255,0,0,255);//两个绿

		EXTI_ClearITPendingBit(EXTI_Line2);//清除中断标志


}





