/**
  ******************************************************************************
  * @file    bsp_ws281x.c 
  * @author  Apex yuan
  * @version V1.0.0
  * @date    2019-8-16
  * @brief   Main program body
  ******************************************************************************
  * @attention
  * 1. ws281x_show()�����ڲ���ִ�����̷ǳ���Ҫ����Ҫ�����޸ģ����ܻᵼ�µ�����ɫ
  *    ���ҵ����
  * 2. Ϊ��ʹWS2812����任��������ִ�У��޸���bsp_systick.c�ļ���
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_ws281x_test.h"
#include "bsp_systick.h"

//uint16_t pixelBuffer[PIXEL_NUM][GRB];

uint16_t eyeBuffer[EYE_PIXEL_NUM][GRB] = {0};
uint16_t earBuffer[EAR_PIXEL_NUM][GRB] = {0};

uint8_t ws281x_status[2] = {0};

/**
  * @brief  initialize WS281x LED
  * @param  None
  * @retval None
  */
void ws281x_init(void)
{
  /* �����ʼ���ṹ�� */
  GPIO_InitTypeDef GPIO_InitStruct;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  DMA_InitTypeDef DMA_InitStruct;
  
  /* ����GPIO TIM DMA ����ʱ�� */
  WS281x_GPIO_CLK_FUNCTION(WS281x_GPIO_CLK, ENABLE);
  WS281x_TIM_CLK_FUNCTION(WS281x_TIM_CLK, ENABLE);
  WS281x_DMA_CLK_FUNCTION(WS281x_DMA_CLK, ENABLE);
  
  /* GPIO�˿����� */
  GPIO_InitStruct.GPIO_Pin = EYE_GPIO_PIN | EAR_GPIO_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WS281x_GPIOx, &GPIO_InitStruct);
  
  /* ��ʱ��ʱ������ */
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 90 - 1;  //72M / 90 = 800K
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(WS281x_TIMx, &TIM_TimeBaseInitStruct);
  
  /* ��ʱ������Ƚ����� */
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_Pulse = 0;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
  /* ��ʼ��ͨ��1 */
  TIM_OC1Init(WS281x_TIMx, &TIM_OCInitStruct);
  /* ��ʼ��ͨ��2 */
  TIM_OC2Init(WS281x_TIMx, &TIM_OCInitStruct);
  
  TIM_CtrlPWMOutputs(WS281x_TIMx, ENABLE);	//PWM���ʹ��	
  
  TIM_OC1PreloadConfig(WS281x_TIMx, TIM_OCPreload_Enable);   //CH1Ԥװ����ʹ��
  TIM_OC2PreloadConfig(WS281x_TIMx, TIM_OCPreload_Enable);   //CH2Ԥװ����ʹ��

  TIM_Cmd(WS281x_TIMx, DISABLE);
  TIM_DMACmd(WS281x_TIMx, EYE_TIM_DMA_SOURCE, ENABLE);
  TIM_DMACmd(WS281x_TIMx, EAR_TIM_DMA_SOURCE, ENABLE);
  
  /* EYE DMA���� */
  DMA_InitStruct.DMA_PeripheralBaseAddr = EYE_DMA_PERIPH_ADDR;
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)eyeBuffer;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStruct.DMA_BufferSize = EYE_PIXEL_NUM * GRB;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(EYE_DMAx_CHANNELx, &DMA_InitStruct);
  
  /* EAR DMA���� */
  DMA_InitStruct.DMA_PeripheralBaseAddr = EAR_DMA_PERIPH_ADDR;
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)earBuffer;
  DMA_InitStruct.DMA_BufferSize = EAR_PIXEL_NUM * GRB;
  DMA_Init(EAR_DMAx_CHANNELx, &DMA_InitStruct);
    
  DMA_Cmd(EYE_DMAx_CHANNELx, DISABLE);
  DMA_Cmd(EAR_DMAx_CHANNELx, DISABLE);
  
  /*�ر����е���*/
  ws281x_closeAll(EYE);
  ws281x_closeAll(EAR);
  delay_ms(100);
}

/*
 * bref ����������ôд��ʾ����������ʹ�á�
 *        
 */
/**
  * @brief  ��ʾ���������Դ�������ʾ�������ϡ�
  *         TIM_OC1PreloadConfig�������ֻ���ڳ�ʼ��������ʹ�ܣ�������λ��ʾ�����û�иú������ڶ�̬�任�����е������ɫ������������ʾ��
  * @param  None
  * @retval None
  */
void ws281x_show(uint8_t name)
{
  switch(name)
  {
    case EYE:
      DMA_SetCurrDataCounter(EYE_DMAx_CHANNELx, (uint16_t)((EYE_PIXEL_NUM) * GRB)); 
      DMA_Cmd(EYE_DMAx_CHANNELx, ENABLE);
      //TIM_OC1PreloadConfig(WS281x_TIMx, TIM_OCPreload_Enable);   //CH1Ԥװ����ʹ��
      TIM_Cmd(WS281x_TIMx, ENABLE);
      while(DMA_GetFlagStatus(EYE_DMA_FLAG) != SET);    //�ȴ��������
      DMA_Cmd(EYE_DMAx_CHANNELx, DISABLE);
      //TIM_OC1PreloadConfig(WS281x_TIMx, TIM_OCPreload_Disable); //�ر�CH1Ԥװ����
      TIM_Cmd(WS281x_TIMx, DISABLE);
      TIM_SetCompare1(WS281x_TIMx, 0);  //��ʹ�ö�·ͨ��ʱ���ռ�ձȷǳ���Ҫ������ᵼ��������λ�ô򿪶�ʱ��ʱ����ͨ�������ϴ�ռ�ձ������
      DMA_ClearFlag(EYE_DMA_FLAG);
      break;
    case EAR:
      DMA_SetCurrDataCounter(EAR_DMAx_CHANNELx, (uint16_t)((EAR_PIXEL_NUM) * GRB)); 
      DMA_Cmd(EAR_DMAx_CHANNELx, ENABLE);
      //TIM_OC2PreloadConfig(WS281x_TIMx, TIM_OCPreload_Enable);   //CH2Ԥװ����ʹ��
      TIM_Cmd(WS281x_TIMx, ENABLE);
      while(DMA_GetFlagStatus(EAR_DMA_FLAG) != SET);    //�ȴ��������
      DMA_Cmd(EAR_DMAx_CHANNELx, DISABLE);
      //TIM_OC2PreloadConfig(WS281x_TIMx, TIM_OCPreload_Disable); //�ر�CH2Ԥװ����
      TIM_Cmd(WS281x_TIMx, DISABLE);
      TIM_SetCompare2(WS281x_TIMx, 0); //��ʹ�ö�·ͨ��ʱ���ռ�ձȷǳ���Ҫ������ᵼ��������λ�ô򿪶�ʱ��ʱ����ͨ�������ϴ�ռ�ձ������
      DMA_ClearFlag(EAR_DMA_FLAG);
      break;
    default:
      break;
  }
}

/**
  * @brief  close all led
  * @param  None
  * @retval None
  */
void ws281x_closeAll(uint8_t name)
{
  uint16_t i;
  uint8_t j;
  
  switch(name)
  {
    case EYE:
      for(i = 0; i < EYE_PIXEL_NUM; ++i)
      {
        for(j = 0; j < GRB; ++j)
        {
          eyeBuffer[i][j] = WS_LOW;
        }
      }
      ws281x_show(EYE);
      break;
    case EAR:
      for(i = 0; i < EAR_PIXEL_NUM; ++i)
      {
        for(j = 0; j < GRB; ++j)
        {
          earBuffer[i][j] = WS_LOW;
        }
      }
      ws281x_show(EAR); 
      break;
    default:
      break;
  } 
}

/**
  * @brief  ��RGB��ɫ��ϵ�һ��32λ�����д洢
  * @param  red��  0-255
  *         green��0-255
  *         blue�� 0-255
  * @retval None
  */
uint32_t ws281x_color(uint8_t red, uint8_t green, uint8_t blue)
{
  return green << 16 | red << 8 | blue;
}

/**
  * @brief  ���ض�LED���趨��ɫ
  * @param  �LED�ƺ�
��*         GRBClor: 32λ����ɫֵ
  * @retval None
  */
void ws281x_setPixelColor(uint8_t name, uint16_t n, uint32_t GRBColor)
{
  uint8_t i;
  
  switch(name)
  {
    case EYE:
      if(n < EYE_PIXEL_NUM)
      {
        for(i = 0; i < GRB; i++)
        {
          eyeBuffer[n][i] = ((GRBColor << i) & 0x800000) ? WS_HIGH : WS_LOW;
        }
      }
      break;
    case EAR:
      if(n < EAR_PIXEL_NUM)
      {
        for(i = 0; i < GRB; i++)
        {
          earBuffer[n][i] = ((GRBColor << i) & 0x800000) ? WS_HIGH : WS_LOW;
        }
      }
      break;
    default:
      break;
  }
}

void ws281x_setPixelRGB(uint8_t name, uint16_t n ,uint8_t red, uint8_t green, uint8_t blue)
{
  uint8_t i;
  
  switch(name)
  {
    case EYE:
      if(n < EYE_PIXEL_NUM)
      {
        for(i = 0; i < GRB; ++i)
        {
          eyeBuffer[n][i] = (((ws281x_color(red,green,blue) << i) & 0X800000) ? WS_HIGH : WS_LOW);
        }
      }
      break;
    case EAR:
      if(n < EAR_PIXEL_NUM)
      {
        for(i = 0; i < GRB; ++i)
        {
          earBuffer[n][i] = (((ws281x_color(red,green,blue) << i) & 0X800000) ? WS_HIGH : WS_LOW);
        }
      }
      break;
    default:
      break;
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t ws281x_wheel(uint8_t wheelPos) {
  wheelPos = 255 - wheelPos;
  if(wheelPos < 85) {
    return ws281x_color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if(wheelPos < 170) {
    wheelPos -= 85;
    return ws281x_color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return ws281x_color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void ws281x_colorWipe(uint8_t name, uint16_t num, uint32_t c, uint8_t wait) {
  for(uint16_t i = 0; i < num; i++) {
    ws281x_setPixelColor(name, i , c);
    ws281x_show(name);
    delay_ms(wait);
  }
}

void ws281x_rainbow(uint8_t name, uint16_t num, uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<num; i++) {
      ws281x_setPixelColor(name, i, ws281x_wheel((i+j) & 255));
    }
    ws281x_show(name);
    delay_ms(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void ws281x_rainbowCycle(uint8_t name, uint16_t num, uint8_t wait) {
  uint16_t i, j;
  
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< num; i++) {
      ws281x_setPixelColor(name, i,ws281x_wheel(((i * 256 / num) + j) & 255));
    }
    ws281x_show(name);
    delay_ms(wait);
  }
}

//Theatre-style crawling lights.
void ws281x_theaterChase(uint8_t name, uint16_t num, uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < num; i=i+3) {
        ws281x_setPixelColor(name, i+q, c);    //turn every third pixel on
      }
      ws281x_show(name);

      delay_ms(wait);

      for (uint16_t i=0; i < num; i=i+3) {
        ws281x_setPixelColor(name, i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void ws281x_theaterChaseRainbow(uint8_t name, uint16_t num, uint8_t wait) {  
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < num; i=i+3) {
        ws281x_setPixelColor(name, i+q, ws281x_wheel( (i+j) % 255));    //turn every third pixel on
      }
      ws281x_show(name);

      delay_ms(wait);

      for (uint16_t i=0; i < num; i=i+3) {
        ws281x_setPixelColor(name, i+q, 0);        //turn every third pixel off
      }
    }
  }
}

