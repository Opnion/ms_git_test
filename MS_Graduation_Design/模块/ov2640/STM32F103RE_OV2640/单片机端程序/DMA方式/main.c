#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>
#include "dcmi_ov2640.h"

#define STATE_START 0x01
#define STATE_STOP 0x02

uint8_t image_buffer[64480]; // 如果存储空间不够了, 把这个数组改小就行了
uint8_t image_state = 0; // 图像捕获状态
uint32_t image_size; // 图像的大小

// 精确延时n毫秒
void delay(uint16_t nms)
{
  TIM6->ARR = 10 * nms - 1;
  TIM6->PSC = 7199;
  TIM6->EGR = TIM_EGR_UG;
  TIM6->SR &= ~TIM_SR_UIF;
  TIM6->CR1 = TIM_CR1_OPM | TIM_CR1_CEN;
  while ((TIM6->SR & TIM_SR_UIF) == 0);
}

// 向串口发送图像数据, 并在末尾附上CRC校验码
void dump(const void *data, uint32_t size)
{
  uint8_t value;
  uint32_t i;
  uint32_t temp;
  
  CRC->CR = CRC_CR_RESET;
  for (i = 0; i < size; i++)
  {
    // 输出图像数据
    value = *((uint8_t *)data + i);
    printf("%02X", value);
    
    // 每4字节计算一次CRC
    if ((i & 3) == 0)
    {
      if (i + 4 <= size)
        CRC->DR = *(uint32_t *)((uint8_t *)data + i);
      else
      {
        temp = 0;
        memcpy(&temp, (uint8_t *)data + i, size - i);
        CRC->DR = temp;
      }
    }
  }
  
  // 输出CRC
  temp = CRC->DR;
  temp = (temp >> 24) | ((temp >> 8) & 0xff00) | ((temp & 0xff00) << 8) | ((temp & 0x00ff) << 24);
  printf("%08X\n", temp);
}

int fputc(int ch, FILE *fp)
{
  if (fp == stdout)
  {
    if (ch == '\n')
    {
      while ((USART1->SR & USART_SR_TXE) == 0);
      USART1->DR = '\r';
    }
    while ((USART1->SR & USART_SR_TXE) == 0);
    USART1->DR = ch;
  }
  return ch;
}

int main(void)
{
  RCC->AHBENR |= RCC_AHBENR_CRCEN | RCC_AHBENR_DMA1EN;
  RCC->APB1ENR = RCC_APB1ENR_I2C1EN | RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM6EN;
  RCC->APB2ENR = RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_USART1EN;
  
  GPIOA->CRH = (GPIOA->CRH & 0xfffff00f) | 0x4b0; // 串口发送引脚PA9设为复用推挽输出
  GPIOB->CRL = (GPIOB->CRL & 0x00ffffff) | 0xff000000; // PB6~7连接摄像头的I2C接口, 设为复用开漏输出
  // PB0(VSYNC)和PB9(=~(HREF & PCLK))为浮空输入
  // PC0~7是摄像头的8位数据引脚, 为浮空输入
  
  USART1->BRR = SystemCoreClock / 115200;
  USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
  
  I2C1->CR2 = 36; // APB1总线频率: 36MHz
  I2C1->CCR = 1800; // 速率: 10kHz (不可太高, 否则会导致Ack Failure; 计算公式为PCLK1/nf, 标准模式下n=2)
  I2C1->TRISE = 37; // 标准模式下为PCLK+1
  I2C1->CR1 = I2C_CR1_PE; // 打开I2C总线
  
  // 每当PB9上出现下降沿时, 就发送一次DMA请求, 采集GPIOC低8位的数据
  TIM4->CCMR2 = TIM_CCMR2_CC3S_1; // PB9(TIM4_CH4)映射到TIM4_CH3上
  TIM4->CCER = TIM_CCER_CC3P; // 下降沿触发
  TIM4->CCER |= TIM_CCER_CC3E; // 打开TIM4_CH3的输入捕获
  // 无需让定时器4开始计时, 这里只使用该定时器的一个输入捕获通道
  
  // 配置TIM4_CH3对应的DMA通道
  DMA1_Channel5->CMAR = (uint32_t)image_buffer;
  DMA1_Channel5->CPAR = (uint32_t)&GPIOC->IDR;
  DMA1_Channel5->CNDTR = sizeof(image_buffer); // 超出部分会被自动丢弃!! 所以图像不能大于64480字节
  DMA1_Channel5->CCR = DMA_CCR5_PL | DMA_CCR5_MINC | DMA_CCR5_EN;
  
  OV2640_Init(JPEG_800x600);
  
  // 打开PB0外部中断
  AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PB;
  EXTI->IMR = EXTI_IMR_MR0;
  EXTI->RTSR = EXTI_RTSR_TR0; // PB0上的上升沿能触发中断
  EXTI->FTSR = EXTI_FTSR_TR0; // PB0上的下降沿也能触发中断
  NVIC_EnableIRQ(EXTI0_IRQn); // 允许执行中断服务函数
  
  while (1)
  {
    if (image_state == (STATE_START | STATE_STOP))
    {
      printf("size=%d\n", image_size);
      dump(image_buffer, image_size); // 通过串口发送图像, 然后附上CRC校验值
      
      // 让DMA内部指针回到数组的开头
      DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
      DMA1_Channel5->CNDTR = sizeof(image_buffer);
      DMA1_Channel5->CCR |= DMA_CCR5_EN;
      
      image_state = 0; // 允许采集新图像 (这条语句一次性把START和STOP都清0了)
    }
  }
}

void EXTI0_IRQHandler(void)
{
  EXTI->PR = EXTI_PR_PR0; // 清除中断标志位
  if (GPIOB->IDR & GPIO_IDR_IDR0)
  {
    // PB0上升沿表示图像数据传输开始
    if (image_state != 0)
      return; // 如果图像已经开始采集了, 就忽略这个开始信号
    image_state = STATE_START;
    
    // 打开TIM4_CH3对应的DMA通道, 开始采集数据
    TIM4->DIER = TIM_DIER_CC3DE; // 允许PB9上的下降沿触发DMA请求
  }
  else
  {
    // PB0下降沿表示图像数据传输结束
    if ((image_state & STATE_START) == 0 || (image_state & STATE_STOP))
      return; // 忽略没有开始信号的结束信号, 以及重复的结束信号
    image_state |= STATE_STOP;
    
    TIM4->DIER &= ~TIM_DIER_CC3DE; // 停止采集
    image_size = sizeof(image_buffer) - DMA1_Channel5->CNDTR; // 总量-剩余数据量=图像大小
  }
}
