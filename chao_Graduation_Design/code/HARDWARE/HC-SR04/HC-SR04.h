/*
 * @Author: ÂíË¬
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:57:33
 * @LastEditTime: 2024-01-16 00:29:36
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\HC-SR04\HC-SR04.h
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __HC_SR04_H
#define __HC_SR04_H

#include "sys.h"

void hcsr04_NVIC(void);
void Hcsr04Init(void);
static void OpenTimerForHc(void);
static void CloseTimerForHc(void);
u32 GetEchoTimer(void);
float Hcsr04GetLength(void);
void NVIC_Configuration(void);

#endif
