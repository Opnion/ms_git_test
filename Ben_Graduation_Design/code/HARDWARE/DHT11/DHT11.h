/*
 * @Author: ÂíË¬
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-14 00:01:28
 * @LastEditTime: 2023-12-14 00:29:00
 * @FilePath: \code\HARDWARE\DHT11\DHT11.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef _DHT11_H
#define _DHT11_H


#define read0 GPIO_ResetBits(GPIOA, GPIO_Pin_0)
#define read1 GPIO_SetBits(GPIOA, GPIO_Pin_0)
#define Read_Data GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

void DHT11_GPIO_Init_IN(void);
void DHT11_GPIO_Init_OUT(void);
void DHT11_Start(void);
unsigned char DHT11_REC_Byte(void);
void DHT11_REC_Data(void);


#endif
