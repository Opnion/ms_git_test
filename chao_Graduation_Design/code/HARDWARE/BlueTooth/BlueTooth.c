#include "sys.h"
#include "math.h"
/*******************************************************
len的长度代表接收数据包的字节13=1字节(包头)+2字节(按键)+8字节(2个int)+2(校验位和包尾);
根据要求设置相应数据，然后在调试器上设置数据包
*******************************************************/
char usart_value; // 接收一个字节数据的变量

// 灯亮度标志位
extern int Levei_flag;
// 灯亮度表
extern int LED_Level[11];
// 蓝牙控制标志位
extern u8 BlueTooth_flag;

uint8_t len = 0;  // 接收数据的数组当前下标
uint8_t num[20];  // 存放接收一次数据包的数组
uint8_t Flag = 0; // 接收到数据之后Flag=1
// static uint8_t f = 0; // 从0xA5开始接收0x5A结束
// 中断服务函数
void DEBUG_USART_IRQHandler(void)
{
	if (USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE))
	{												   // 接收中断标志位变化
		usart_value = USART_ReceiveData(DEBUG_USARTx); // 接收一个字节的数据
		if (usart_value == 'k')						   // 从0xA5开始
		{
			// 最小系统板的灯当指示灯，输入k时灯亮起
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			// TIM_SetCompare2(TIM3, LED_Level[Levei_flag]);
			BlueTooth_flag = 1;
		}
		else if (usart_value == 'g')
		{
			// 最小系统板的灯当指示灯，输入k时灯熄灭
			GPIO_SetBits(GPIOC, GPIO_Pin_13);
			// TIM_SetCompare2(TIM3, 0);
			BlueTooth_flag = 0;
		}
	}

	USART_ClearFlag(DEBUG_USARTx, USART_IT_RXNE); // 清除中断标志位
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
