/*
 * @Author: ÂíË¬
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-14 00:03:47
 * @LastEditTime: 2023-12-18 00:20:22
 * @FilePath: \USERe:\Ben_Graduation Design\code\HARDWARE\HX711\HX711.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __HX711_H
#define __HX711_H
#include "sys.h"

#define HX711_SCK PBout(1)             // PB1
#define HX711_DOUT PBin(0)             // PB2

extern void Init_HX711pin(void);
extern u32 HX711_Read(void);
extern void Get_Maopi(void);
extern void Get_Weight(void);

extern u32 HX711_Buffer;
extern u32 Weight_Maopi;
extern s32 Weight_Shiwu;
extern u8 Flag_Error;

#endif

