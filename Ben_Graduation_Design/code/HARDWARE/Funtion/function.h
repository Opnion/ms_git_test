/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-21 04:48:19
 * @LastEditTime: 2023-12-30 14:03:59
 * @FilePath: \USERe:\Ben_Graduation Design\code\HARDWARE\Funtion\function.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __FUNCTION_H
#define __FUNCTION_H
	
#include "stm32f10x.h"
#include "HC05.h"
#include "function.h"

typedef struct {
	int  num;
	char c;
}INPUT;

INPUT DATARecv();//������λ������
void BL_Send(USART_TypeDef*pUSARTx,u8 send_ok);//�������ݸ���λ��


#endif 

