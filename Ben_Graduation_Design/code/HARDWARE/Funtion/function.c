#include "function.h"
#include "HC05.h"
#include "math.h"
#include "stm32f10x_it.h" 

extern uint8_t Flag;//���ݰ��Ƿ���
extern uint8_t num[20];//�洢��λ�����������ݰ�


/**************************************************************************
��������DATARecv

���ã�  ���жϽ��յ����ݰ����������ǵ�INPUT�ṹ�壬�ṹ���DATARccv�ɸ���ʵ��������и���

����ֵ��INPUT���͵Ľṹ��

ʹ�ã�BL_Send(DEBUG_USARTx,mode,quan)
***************************************************************************/
INPUT DATARecv(){
	int a=0;
	uint8_t i;
	INPUT structure;
  
	//����һ��char���͵�����
	structure.c=num[1];
	//����һ��int��������
	for(i=2;i<=6;i++){ 
		a+=num[i]<<((i-2)*8);
	}
	structure.num=a;
	a = 0;
	Flag=0;    //�������
	return structure;
}
/**************************************************************************
��������BL_Send

���ã�  ��λ��������ʾ�����ӷ�����λ��������Ҫ���͵������ֽڣ��ڵ����������ý������ݰ�
				����1�����ڣ���������͵����ݣ��ɸ���ʵ��������и���

������(�������ͣ�Ҫ���͵Ĳ���1������2������3)���޸ĸ�����ͬʱҲҪ�޸ķ��͵��ֽھ�������ע�͵��Ĳ���

ʹ�ã�BL_Send(DEBUG_USARTx,mode,quan)
***************************************************************************/
void BL_Send(USART_TypeDef*pUSARTx,u8 send_ok){
	
	u8 t;
	u8 sum=0;//У��λ--�����ֽ�֮�͵ĵͰ�λ
	u8 i;
	Usart_SendByte(pUSARTx,0xA5);//ͷ
	///////////////////////////////////////����ģʽ
  Usart_SendByte(pUSARTx,send_ok);
  sum+=send_ok;
	Usart_SendByte(pUSARTx,sum);//У��λ
	Usart_SendByte(pUSARTx,0x5A);//β
}


