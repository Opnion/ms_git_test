/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-14 00:03:47
 * @LastEditTime: 2023-12-18 03:05:32
 * @FilePath: \USERe:\Ben_Graduation Design\code\HARDWARE\HX711\HX711.c
 * 
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "sys.h"


u32 HX711_Buffer;
u32 Weight_Maopi;
s32 Weight_Shiwu = 0;
u8 Flag_Error = 0;
float GapValue = 0.45;//25.65418;               //该值根据测量值进行调整，

void Init_HX711pin(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //HX711_SCK
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);             //根据设定参数初始化GPIOC

    //HX711_DOUT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //输入上拉
    GPIO_Init(GPIOB, & GPIO_InitStructure);

    //GPIO_SetBits(GPIOB, GPIO_Pin_1);   //初始化设置为0
}

u32 HX711_Read(void)                   //增益128
{
    u32 count;
    u8 i;
    HX711_DOUT = 1;
    delay_us(1);
    HX711_SCK = 0;
    count = 0;
    while (HX711_DOUT);
    for (i = 0; i < 24; i++)
    {
        HX711_SCK = 1;
        count = count << 1;
        delay_us(1);
        HX711_SCK = 0;
        if (HX711_DOUT)
            count++;
        delay_us(1);
    }
    HX711_SCK = 1;
    count=count^0x800000;//第25个脉冲下降沿来时，转换数据
    delay_us(1);
    HX711_SCK = 0;
    return(count);
}


void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();	
} 


//****************************************************
//称重
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = HX711_Read();
	if(HX711_Buffer > Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//获取实物的AD采样数值。
		//printf("实物ad值是：%d\r\n",Weight_Shiwu);
        
		Weight_Shiwu = (s32)((float)Weight_Shiwu/GapValue-0.0383); 	//计算实物的实际重量
																		//因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。
																		//当发现测试出来的重量偏大时，增加该数值。
																		//如果测试出来的重量偏小时，减小改数值。
	}
}


