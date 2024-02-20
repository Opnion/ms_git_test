#include "sys.h"
#include "math.h"
/*******************************************************
len�ĳ��ȴ���������ݰ����ֽ�13=1�ֽ�(��ͷ)+2�ֽ�(����)+8�ֽ�(2��int)+2(У��λ�Ͱ�β);
����Ҫ��������Ӧ���ݣ�Ȼ���ڵ��������������ݰ�
*******************************************************/
char usart_value; // ����һ���ֽ����ݵı���

// �����ȱ�־λ
extern int Levei_flag;
// �����ȱ�
extern int LED_Level[11];
// �������Ʊ�־λ
extern u8 BlueTooth_flag;

uint8_t len = 0;  // �������ݵ����鵱ǰ�±�
uint8_t num[20];  // ��Ž���һ�����ݰ�������
uint8_t Flag = 0; // ���յ�����֮��Flag=1
// static uint8_t f = 0; // ��0xA5��ʼ����0x5A����
// �жϷ�����
void DEBUG_USART_IRQHandler(void)
{
	if (USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE))
	{												   // �����жϱ�־λ�仯
		usart_value = USART_ReceiveData(DEBUG_USARTx); // ����һ���ֽڵ�����
		if (usart_value == 'k')						   // ��0xA5��ʼ
		{
			// ��Сϵͳ��ĵƵ�ָʾ�ƣ�����kʱ������
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			// TIM_SetCompare2(TIM3, LED_Level[Levei_flag]);
			BlueTooth_flag = 1;
		}
		else if (usart_value == 'g')
		{
			// ��Сϵͳ��ĵƵ�ָʾ�ƣ�����kʱ��Ϩ��
			GPIO_SetBits(GPIOC, GPIO_Pin_13);
			// TIM_SetCompare2(TIM3, 0);
			BlueTooth_flag = 0;
		}
	}

	USART_ClearFlag(DEBUG_USARTx, USART_IT_RXNE); // ����жϱ�־λ
}

static void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}
//
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);

	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(DEBUG_USARTx, &USART_InitStructure);

	NVIC_Config();
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);
	USART_Cmd(DEBUG_USARTx, ENABLE);
}
//
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t data)
{
	USART_SendData(pUSARTx, data);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
		;
}
