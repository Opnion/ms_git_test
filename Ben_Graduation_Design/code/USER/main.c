/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:52:06
 * @LastEditTime: 2023-12-20 20:23:40
 * @FilePath: \code\USER\main.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "sys.h"
 #include "stm32f10x_rtc.h" 
 #include "stm32f10x_pwr.h" 
 #include "stm32f10x_bkp.h" 
 #include "led.h"
extern unsigned int rec_data[4];
float weight = 0;
u16 adcx;
float temp;


int main(void)
{
	
	delay_init();									// 延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    LED_Init();
	
	USART_Config();    //蓝牙初始化
	OLED_Init();		 // 初始化OLED

	SG90_Init(1999, 719);
	
	OLED_Clear();

	OLED_Mune();

	Adc_Init();		 // ADC初始化
	Init_HX711pin(); // HX711初始化
	Get_Maopi();
	delay_ms(1000);
	Get_Maopi();
	
	while (1)
	{
       
		DHT11_REC_Data(); // 接收温度和湿度的数据
		OLED_ShowNum(54, 0, rec_data[2], 2, 16);

		OLED_ShowNum(78, 0, rec_data[3], 1, 16);

		OLED_ShowNum(54, 2, rec_data[0], 2, 16);

		OLED_ShowNum(78, 2, rec_data[1], 1, 16);

		Get_Weight();
		delay_ms(1000);
		// weight=((float)Weight_Shiwu  -735.92)/21.8+68;///(float)106.5;
		weight = ((float)Weight_Shiwu) / 1000; ///(float)106.5;
											   // OLED_ShowNum(60,4,weight,1,16);
		OLED_ShowFloatNumber(60, 4, weight, 16);
		//     //舵机旋转90度
		//    TIM_SetCompare1(TIM4,1950);		//TIM4->CCR1=400
		// 	TIM_SetCompare2(TIM4,1900);	    //TIM4->CCR2=800
		//     delay_ms(500);
		//     TIM_SetCompare1(TIM4,1850);		//TIM4->CCR1=400
		// 	TIM_SetCompare2(TIM4,1800);	    //TIM4->CCR2=800
		//     delay_ms(500);
		adcx = Get_Adc_Average(ADC_Channel_1, 10);
		temp = (float)adcx * (3.3 / 4096);
		if (temp <= 0.7)
		{
			TIM_SetCompare1(TIM4, 1950); // TIM4->CCR1=400
			OLED_ShowCHinese(90, 6, 11); // 子
		}
		else if (temp > 1.5)
		{
			TIM_SetCompare1(TIM4, 1850); // TIM4->CCR1=400
			OLED_ShowCHinese(90, 6, 12); // 子
		}

		
	}
}
