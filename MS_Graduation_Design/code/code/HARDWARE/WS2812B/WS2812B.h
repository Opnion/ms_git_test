/*
 * @Author: ÂíË¬
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-21 06:50:21
 * @LastEditTime: 2023-12-22 10:35:48
 * @FilePath: \USERe:\MS_Graduation Design\code\code\HARDWARE\WS2812B\WS2812B.h
 * https://blog.csdn.net/weixin_53411486/article/details/127246513
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */

#ifndef WS2812B_H
#define WS2812B_H

#include "sys.h"




#define 	RGB_LED 	GPIO_Pin_14
#define		RGB_LED_HIGH	(GPIO_SetBits(GPIOE,RGB_LED))
#define 	RGB_LED_LOW		(GPIO_ResetBits(GPIOE,RGB_LED))
 
void RGB_LED_Init(void);
void RGB_LED_Write0(void);
void RGB_LED_Write1(void);
void RGB_LED_Reset(void);
void RGB_LED_Write_Byte(uint8_t byte);
void RGB_LED_Write_24Bits(uint8_t green,uint8_t red,uint8_t blue,uint8_t green1,uint8_t red1,uint8_t blue1);
void RGB_LED_Red(void);
void RGB_LED_Green(void);
void RGB_LED_Blue(void);


#endif






