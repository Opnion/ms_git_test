#include "bsp.h"

void bsp_init(void)
{ 
  /* �����ж����ȼ����� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  systick_init();
  ws281x_init();
}


