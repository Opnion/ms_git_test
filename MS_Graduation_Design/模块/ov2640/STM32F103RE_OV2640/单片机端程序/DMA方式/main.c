#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>
#include "dcmi_ov2640.h"

#define STATE_START 0x01
#define STATE_STOP 0x02

uint8_t image_buffer[64480]; // ����洢�ռ䲻����, ����������С������
uint8_t image_state = 0; // ͼ�񲶻�״̬
uint32_t image_size; // ͼ��Ĵ�С

// ��ȷ��ʱn����
void delay(uint16_t nms)
{
  TIM6->ARR = 10 * nms - 1;
  TIM6->PSC = 7199;
  TIM6->EGR = TIM_EGR_UG;
  TIM6->SR &= ~TIM_SR_UIF;
  TIM6->CR1 = TIM_CR1_OPM | TIM_CR1_CEN;
  while ((TIM6->SR & TIM_SR_UIF) == 0);
}

// �򴮿ڷ���ͼ������, ����ĩβ����CRCУ����
void dump(const void *data, uint32_t size)
{
  uint8_t value;
  uint32_t i;
  uint32_t temp;
  
  CRC->CR = CRC_CR_RESET;
  for (i = 0; i < size; i++)
  {
    // ���ͼ������
    value = *((uint8_t *)data + i);
    printf("%02X", value);
    
    // ÿ4�ֽڼ���һ��CRC
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
  
  // ���CRC
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
  
  GPIOA->CRH = (GPIOA->CRH & 0xfffff00f) | 0x4b0; // ���ڷ�������PA9��Ϊ�����������
  GPIOB->CRL = (GPIOB->CRL & 0x00ffffff) | 0xff000000; // PB6~7��������ͷ��I2C�ӿ�, ��Ϊ���ÿ�©���
  // PB0(VSYNC)��PB9(=~(HREF & PCLK))Ϊ��������
  // PC0~7������ͷ��8λ��������, Ϊ��������
  
  USART1->BRR = SystemCoreClock / 115200;
  USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
  
  I2C1->CR2 = 36; // APB1����Ƶ��: 36MHz
  I2C1->CCR = 1800; // ����: 10kHz (����̫��, ����ᵼ��Ack Failure; ���㹫ʽΪPCLK1/nf, ��׼ģʽ��n=2)
  I2C1->TRISE = 37; // ��׼ģʽ��ΪPCLK+1
  I2C1->CR1 = I2C_CR1_PE; // ��I2C����
  
  // ÿ��PB9�ϳ����½���ʱ, �ͷ���һ��DMA����, �ɼ�GPIOC��8λ������
  TIM4->CCMR2 = TIM_CCMR2_CC3S_1; // PB9(TIM4_CH4)ӳ�䵽TIM4_CH3��
  TIM4->CCER = TIM_CCER_CC3P; // �½��ش���
  TIM4->CCER |= TIM_CCER_CC3E; // ��TIM4_CH3�����벶��
  // �����ö�ʱ��4��ʼ��ʱ, ����ֻʹ�øö�ʱ����һ�����벶��ͨ��
  
  // ����TIM4_CH3��Ӧ��DMAͨ��
  DMA1_Channel5->CMAR = (uint32_t)image_buffer;
  DMA1_Channel5->CPAR = (uint32_t)&GPIOC->IDR;
  DMA1_Channel5->CNDTR = sizeof(image_buffer); // �������ֻᱻ�Զ�����!! ����ͼ���ܴ���64480�ֽ�
  DMA1_Channel5->CCR = DMA_CCR5_PL | DMA_CCR5_MINC | DMA_CCR5_EN;
  
  OV2640_Init(JPEG_800x600);
  
  // ��PB0�ⲿ�ж�
  AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PB;
  EXTI->IMR = EXTI_IMR_MR0;
  EXTI->RTSR = EXTI_RTSR_TR0; // PB0�ϵ��������ܴ����ж�
  EXTI->FTSR = EXTI_FTSR_TR0; // PB0�ϵ��½���Ҳ�ܴ����ж�
  NVIC_EnableIRQ(EXTI0_IRQn); // ����ִ���жϷ�����
  
  while (1)
  {
    if (image_state == (STATE_START | STATE_STOP))
    {
      printf("size=%d\n", image_size);
      dump(image_buffer, image_size); // ͨ�����ڷ���ͼ��, Ȼ����CRCУ��ֵ
      
      // ��DMA�ڲ�ָ��ص�����Ŀ�ͷ
      DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
      DMA1_Channel5->CNDTR = sizeof(image_buffer);
      DMA1_Channel5->CCR |= DMA_CCR5_EN;
      
      image_state = 0; // ����ɼ���ͼ�� (�������һ���԰�START��STOP����0��)
    }
  }
}

void EXTI0_IRQHandler(void)
{
  EXTI->PR = EXTI_PR_PR0; // ����жϱ�־λ
  if (GPIOB->IDR & GPIO_IDR_IDR0)
  {
    // PB0�����ر�ʾͼ�����ݴ��俪ʼ
    if (image_state != 0)
      return; // ���ͼ���Ѿ���ʼ�ɼ���, �ͺ��������ʼ�ź�
    image_state = STATE_START;
    
    // ��TIM4_CH3��Ӧ��DMAͨ��, ��ʼ�ɼ�����
    TIM4->DIER = TIM_DIER_CC3DE; // ����PB9�ϵ��½��ش���DMA����
  }
  else
  {
    // PB0�½��ر�ʾͼ�����ݴ������
    if ((image_state & STATE_START) == 0 || (image_state & STATE_STOP))
      return; // ����û�п�ʼ�źŵĽ����ź�, �Լ��ظ��Ľ����ź�
    image_state |= STATE_STOP;
    
    TIM4->DIER &= ~TIM_DIER_CC3DE; // ֹͣ�ɼ�
    image_size = sizeof(image_buffer) - DMA1_Channel5->CNDTR; // ����-ʣ��������=ͼ���С
  }
}
