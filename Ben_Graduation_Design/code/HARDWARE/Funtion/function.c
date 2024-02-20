#include "function.h"
#include "HC05.h"
#include "math.h"
#include "stm32f10x_it.h" 

extern uint8_t Flag;//数据包是否发送
extern uint8_t num[20];//存储上位机发出的数据包


/**************************************************************************
函数名：DATARecv

作用：  将中断接收的数据包导出到我们的INPUT结构体，结构体和DATARccv可根据实际情况进行更改

返回值：INPUT类型的结构体

使用：BL_Send(DEBUG_USARTx,mode,quan)
***************************************************************************/
INPUT DATARecv(){
	int a=0;
	uint8_t i;
	INPUT structure;
  
	//接收一个char类型的数据
	structure.c=num[1];
	//接收一个int整形数据
	for(i=2;i<=6;i++){ 
		a+=num[i]<<((i-2)*8);
	}
	structure.num=a;
	a = 0;
	Flag=0;    //接收完成
	return structure;
}
/**************************************************************************
函数名：BL_Send

作用：  上位机数据显示，板子发送上位机，根据要发送的数据字节，在调试器上设置接收数据包
				参数1代表串口，后面代表发送的数据，可根据实际情况进行更改

参数：(串口类型，要发送的参数1，参数2，参数3)可修改个数，同时也要修改发送的字节就是下面注释掉的部分

使用：BL_Send(DEBUG_USARTx,mode,quan)
***************************************************************************/
void BL_Send(USART_TypeDef*pUSARTx,u8 send_ok){
	
	u8 t;
	u8 sum=0;//校验位--数据字节之和的低八位
	u8 i;
	Usart_SendByte(pUSARTx,0xA5);//头
	///////////////////////////////////////发送模式
  Usart_SendByte(pUSARTx,send_ok);
  sum+=send_ok;
	Usart_SendByte(pUSARTx,sum);//校验位
	Usart_SendByte(pUSARTx,0x5A);//尾
}


