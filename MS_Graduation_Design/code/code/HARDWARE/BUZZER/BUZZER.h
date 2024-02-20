/*
 * @Author: ÂíË¬
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-11-29 15:26:11
 * @LastEditTime: 2023-12-22 11:08:02
 * @FilePath: \USERe:\MS_Graduation Design\code\code\HARDWARE\BUZZER\BUZZER.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __BUZZER_H
#define __BUZZER_H

#include "sys.h"
#include "stdint.h"

void Buzzer_Init(void);
void TIM2_Init(u16 arr,u16 psc);

#endif
