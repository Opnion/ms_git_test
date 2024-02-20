/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-05 02:00:45
 * @LastEditTime: 2023-12-05 13:10:55
 * @FilePath: \USERe:\Graduation Design\code\code\HARDWARE\GSM\GSM.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __GSM_H
#define __GSM_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

void USART3_Send_data(char *str);//���ڷ����ַ�����GSMģ����Ҫ���ܵ����
void USART3_Send_data_value(u8 dat);//���ڷ���ʮ��������

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void GSM_Init(u32 bound);
#endif


