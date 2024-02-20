#include "bsp_systick.h"

volatile uint32_t _us_tick;
 uint32_t _ms_tick = 0;

void systick_init(void) 
{
  _us_tick = 0;
  _ms_tick = 0;  
  
	if(SysTick_Config(SystemCoreClock / 1000))
  {
    /*capture error*/
    while(1);
  }
  
  /* Configure the SysTick handler priority */
  //NVIC_SetPriority(SysTick_IRQn, 0x0);
}

void delay_ms(uint32_t millis) 
{ 
	uint32_t target;
	
	target = _ms_tick + millis;
	while(_ms_tick < target);
} 
void delay_us(uint32_t uillis)
{ 
	uint32_t target;
	target = _us_tick + uillis;
	while(_us_tick < target);
}

uint32_t millis(void) 
{
	return _ms_tick;
}

uint32_t micros(void)
{
  return _us_tick;
}

void reset(void) 
{
	_us_tick = 0;
  _ms_tick = 0;
}

void SysTick_Handler(void)
{
	_us_tick++;
  //_ms_tick = _us_tick / 1000;
  _ms_tick++;

}


//static uint8_t  fac_us=0;							//us��ʱ������			   
//static uint16_t fac_ms=0;							//ms��ʱ������

//void delay_init(void)
//{
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
//	fac_us = SystemCoreClock/8000000;				//Ϊϵͳʱ�ӵ�1/8  
//	fac_ms = (u16)fac_us*1000;					//ÿ��ms��Ҫ��systickʱ����   
//}	

////��ʱnus
////nusΪҪ��ʱ��us��.
//void delay_us(uint32_t nus)
//{		
//	uint32_t temp;	    	 
//	SysTick->LOAD=nus*fac_us; 					//ʱ�����	  		 
//	SysTick->VAL=0x00;        					//��ռ�����
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����	  
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
//	SysTick->VAL =0X00;      					 //��ռ�����	 
//}
////��ʱnms
////ע��nms�ķ�Χ
////SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
////nms<=0xffffff*8*1000/SYSCLK
////SYSCLK��λΪHz,nms��λΪms
////��72M������,nms<=1864 
//void delay_ms(uint16_t nms)
//{	 		  	  
//	uint32_t temp;		   
//	SysTick->LOAD=(uint32_t)nms*fac_ms;				//ʱ�����(SysTick->LOADΪ24bit)
//	SysTick->VAL =0x00;							//��ռ�����
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����  
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
//	SysTick->VAL =0X00;       					//��ռ�����	  	    
//} 

