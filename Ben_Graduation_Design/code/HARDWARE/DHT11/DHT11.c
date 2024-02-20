/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-14 00:01:18
 * @LastEditTime: 2023-12-14 01:17:28
 * @FilePath: \code\HARDWARE\DHT11\DHT11.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "sys.h"


unsigned int rec_data[4];

//对于stm32来说，是输入
void DHT11_GPIO_Init_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

//对于stm32来说，是输出
void DHT11_GPIO_Init_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void DHT11_Start(void)
{
	DHT11_GPIO_Init_OUT();
	
	read1;
	delay_us(30);
	
	read0;
	delay_ms(20);
	
	read1;
	delay_us(30);
	
	DHT11_GPIO_Init_IN();
	
}

unsigned char DHT11_REC_Byte(void)
{
	unsigned char i = 0;
	unsigned char data;
	
	for(i=0;i<8;i++)
	{
		while(Read_Data == 0);
		delay_us(30);
		
		data <<= 1;
		
		if(Read_Data == 1)
		{
			data += 1;
		}
		
		while(Read_Data == 1);
	}
	
	return data;
}

void DHT11_REC_Data(void)
{
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH,RL,TH,TL,CHECK;
	
	DHT11_Start();
	read1;
	
	if(Read_Data == 0)
	{
		while(Read_Data == 0);
		while(Read_Data == 1);
		
		R_H = DHT11_REC_Byte();
		R_L = DHT11_REC_Byte();
		T_H = DHT11_REC_Byte();
		T_L = DHT11_REC_Byte();
		CHECK = DHT11_REC_Byte();
		
		read0;
		delay_us(55);
		read1;
		
		if(R_H + R_L + T_H + T_L == CHECK)
		{
			RH = R_H;
			RL = R_L;
			TH = T_H;
			TL = T_L;
		}
	}
	
	rec_data[0] = RH;
	rec_data[1] = RL;
	rec_data[2] = TH;
	rec_data[3] = TL;
}

