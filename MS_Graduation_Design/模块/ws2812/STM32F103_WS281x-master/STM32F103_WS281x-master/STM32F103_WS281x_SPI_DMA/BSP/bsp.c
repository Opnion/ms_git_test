#include "bsp.h"

void bsp_init(void)
{
  /*Ϊ�����SPI_DMA��ʽ����WS2812���飬ͨ���޸�system_stm32f10x.c�ļ�����ϵͳ��Ƶ����Ϊ56M*/
  
  /* �����ж����ȼ����� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  systick_init();
  //delay_init();
  ws281x_init();
}


