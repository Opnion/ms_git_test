#ifndef __HC05_H
#define __HC05_H
#include "stm32f10x.h"
#include <stdio.h>

#define REC_BUF_SIZE 8    //接收数据包的大小

#define DEBUG_USARTx                     USART2   //蓝牙所用串口2
#define DEBUG_USART_CLK                  RCC_APB1Periph_USART2  //串口时钟
#define DEBUG_USART_APBxClkCmd           RCC_APB1PeriphClockCmd //串口时钟使能
#define DEBUG_USART_BAUDRATE             9600 //波特率设置·

#define DEBUG_USART_GPIO_CLK             RCC_APB2Periph_GPIOA   

#define DEBUG_USART_GPIO_APBxClkCmd       RCC_APB2PeriphClockCmd  //端口时钟

#define DEBUG_USART_TX_GPIO_PORT         GPIOA              //端口宏定义
#define DEBUG_USART_TX_GPIO_PIN          GPIO_Pin_2
#define DEBUG_USART_RX_GPIO_PORT         GPIOA
#define DEBUG_USART_RX_GPIO_PIN          GPIO_Pin_3

#define DEBUG_USART_IRQ                  USART2_IRQn
#define DEBUG_USART_IRQHandler           USART2_IRQHandler   //中断服务函数

static void NVIC_Config(void);
void USART_Config(void);
void Usart_SendByte(USART_TypeDef*pUSARTx,uint8_t data);

#endif 

/* 串口一配置
#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "stm32f10x.h"
#include <stdio.h>


#define DEBUG_USARTx                     USART1   //蓝牙所用串口2
#define DEBUG_USART_CLK                  RCC_APB2Periph_USART1  //串口时钟
#define DEBUG_USART_APBxClkCmd           RCC_APB2PeriphClockCmd //串口时钟使能
#define DEBUG_USART_BAUDRATE             115200 //波特率设置·

#define DEBUG_USART_GPIO_CLK             RCC_APB2Periph_GPIOA   

#define DEBUG_USART_GPIO_APBxClkCmd       RCC_APB2PeriphClockCmd  //端口时钟

#define DEBUG_USART_TX_GPIO_PORT         GPIOA              //端口宏定义
#define DEBUG_USART_TX_GPIO_PIN          GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PORT         GPIOA
#define DEBUG_USART_RX_GPIO_PIN          GPIO_Pin_10

#define DEBUG_USART_IRQ                  USART1_IRQn
#define DEBUG_USART_IRQHandler           USART1_IRQHandler   //中断服务函数

static void NVIC_Config(void);
void USART_Config(void);
void Usart_SendByte(USART_TypeDef*pUSARTx,uint8_t data);

#endif 
*/