/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:52:05
 * @LastEditTime: 2023-12-29 10:27:25
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\KEY\key.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:52:05
 * @LastEditTime: 2023-12-28 00:07:30
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\KEY\key.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "stm32f10x.h"
#include "sys.h" 
#include "delay.h"



/**
 * @Description: 
 * PA12=>KEY_DOWN(���ȼ�);PA11=>KEY_UP(������);PA10=>KEY2_L(����);PA9=>KEY1_C(������);PA8=>KEY0_G(����)
 * @return {*}
 * @Author: ��ˬ
 * @Date: 2023-12-28 00:07:36
 */							    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
}


