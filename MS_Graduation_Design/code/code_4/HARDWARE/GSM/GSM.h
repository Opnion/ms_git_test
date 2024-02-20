/*
 * @Author: 马爽
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


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收

void USART3_Send_data(char *str);//用于发送字符串（GSM模块需要接受的命令）
void USART3_Send_data_value(u8 dat);//用于发送十六进制数

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void GSM_Init(u32 bound);
#endif


