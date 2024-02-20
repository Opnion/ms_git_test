/*
 * @Author: ÂíË¬
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:52:05
 * @LastEditTime: 2023-12-29 05:04:28
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\LED\led.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define LED PBout(5) // PB5

void LED_Init(u16 arr, u16 psc); 

#endif
