/*
 * @Author: ÂíË¬
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-14 00:03:50
 * @LastEditTime: 2023-12-14 00:22:40
 * @FilePath: \code\HARDWARE\WaterSensor\WaterSensor.h
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#ifndef __WaterSensor_H
#define __WaterSensor_H
void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 

#endif


