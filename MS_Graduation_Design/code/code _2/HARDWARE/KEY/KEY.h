/*
 * @Author: ��ˬ
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

//������ʼ��
void KEY_Init(void);
//����(�ⲿ)�жϣ�����0->���Դ��루�����š����գ�;����1->�����죻����2->����
void KEY_EXIT_Init(void);

#endif

