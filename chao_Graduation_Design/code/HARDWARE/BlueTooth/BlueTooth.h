/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:57:31
 * @LastEditTime: 2023-12-30 14:45:16
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\BlueTooth\BlueTooth.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __BlueTooth_H
#define __BlueTooth_H

#include "stm32f10x.h"
#include <stdio.h>
#include "sys.h"
#define REC_BUF_SIZE 8    //�������ݰ��Ĵ�С

#define DEBUG_USARTx                     USART3   //�������ô���3
#define DEBUG_USART_CLK                  RCC_APB1Periph_USART3  //����ʱ��
#define DEBUG_USART_APBxClkCmd           RCC_APB1PeriphClockCmd //����ʱ��ʹ��
#define DEBUG_USART_BAUDRATE             9600 //����������

#define DEBUG_USART_GPIO_CLK             RCC_APB2Periph_GPIOB   

#define DEBUG_USART_GPIO_APBxClkCmd       RCC_APB2PeriphClockCmd  //�˿�ʱ��

#define DEBUG_USART_TX_GPIO_PORT         GPIOB              //�˿ں궨��
#define DEBUG_USART_TX_GPIO_PIN          GPIO_Pin_10
#define DEBUG_USART_RX_GPIO_PORT         GPIOB
#define DEBUG_USART_RX_GPIO_PIN          GPIO_Pin_11

#define DEBUG_USART_IRQ                  USART3_IRQn
#define DEBUG_USART_IRQHandler           USART3_IRQHandler   //�жϷ�����

static void NVIC_Config(void);
void USART_Config(void);
void Usart_SendByte(USART_TypeDef*pUSARTx,uint8_t data);


#endif

