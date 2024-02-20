/*
 * @Author: 马爽
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
#define REC_BUF_SIZE 8    //接收数据包的大小

#define DEBUG_USARTx                     USART3   //蓝牙所用串口3
#define DEBUG_USART_CLK                  RCC_APB1Periph_USART3  //串口时钟
#define DEBUG_USART_APBxClkCmd           RCC_APB1PeriphClockCmd //串口时钟使能
#define DEBUG_USART_BAUDRATE             9600 //波特率设置

#define DEBUG_USART_GPIO_CLK             RCC_APB2Periph_GPIOB   

#define DEBUG_USART_GPIO_APBxClkCmd       RCC_APB2PeriphClockCmd  //端口时钟

#define DEBUG_USART_TX_GPIO_PORT         GPIOB              //端口宏定义
#define DEBUG_USART_TX_GPIO_PIN          GPIO_Pin_10
#define DEBUG_USART_RX_GPIO_PORT         GPIOB
#define DEBUG_USART_RX_GPIO_PIN          GPIO_Pin_11

#define DEBUG_USART_IRQ                  USART3_IRQn
#define DEBUG_USART_IRQHandler           USART3_IRQHandler   //中断服务函数

static void NVIC_Config(void);
void USART_Config(void);
void Usart_SendByte(USART_TypeDef*pUSARTx,uint8_t data);


#endif

