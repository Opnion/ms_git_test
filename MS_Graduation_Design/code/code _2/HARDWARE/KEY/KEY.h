/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-11-29 15:20:16
 * @LastEditTime: 2023-11-30 06:14:16
 * @FilePath: \USERe:\Graduation Design\code\code\HARDWARE\KEY\KEY.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

#define KEY0 PAin(0)
#define KEY1 PAin(1)
#define KEY2 PAin(2)

//按键初始化
void KEY_Init(void);
//按键(外部)中断；按键0->测试代码（发短信、拍照）;按键1->门铃响；按键2->留言
void KEY_EXIT_Init(void);

#endif

