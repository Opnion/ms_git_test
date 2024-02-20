/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-11-29 15:29:02
 * @LastEditTime: 2023-11-30 04:34:47
 * @FilePath: \USERe:\Graduation Design\code\code\HARDWARE\LED\LED.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __LED_H
#define __LED_H

#include "sys.h"

//设置F9、F10口为输出
#define LED0 PFout(9)
#define LED1 PFout(10)

//LED初始化
void LED_Init(void);

#endif

