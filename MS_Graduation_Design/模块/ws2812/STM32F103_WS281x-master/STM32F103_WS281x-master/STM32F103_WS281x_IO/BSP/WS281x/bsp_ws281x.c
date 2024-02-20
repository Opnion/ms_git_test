/**
  ******************************************************************************
  * @file    bsp_ws281x.c 
  * @author  Apex yuan
  * @version V1.0.0
  * @date    2019-8-16
  * @brief   Main program body
  ******************************************************************************
  * @attention
  * 1. ����IO��ƽ��ת��ģ��WS281xʱ�򣬶�ʱ��Ҫ��ܸߣ�������������Ƭ������������
  *    ���顣
  * 2. ��ʱ������ʵ�֣�����ʹ��SYSTICK��ѯ�ķ�ʽʵ�֣��жϷ�ʽ�����ͨ��ʱ��
  * 3. ����0��1�����ͨ�������Ĵ����ķ�ʽ���ſ��Դﵽ����
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_ws281x.h"
#include "bsp_systick.h"

uint8_t pixelBuffer[PIXEL_NUM][GRB];

/**
  * @brief  initialize WS281x LED
  * @param  None
  * @retval None
  */
void ws281x_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  WS281x_GPIO_CLK_FUNCTION(WS281x_GPIO_CLK, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = WS281x_GPIO_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WS281x_GPIOx, &GPIO_InitStruct);
  GPIO_ResetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  
  ws281x_closeAll();
  delay_ms(100);
}

/**
  * @brief  ר����WS281x������������ʱ����
  * @param  count����ʱ�� ��ʾ����������ʱʱ�� = delay_num * 440ns ��
  * @retval None
  */
static void ws281x_delay(uint32_t count)
{
  while(count--);
}

/**
  * @brief  ����0�룬�˴���ʱ��Ҫ��ܸߣ�����IO�˿�ֻ��ͨ���Ĵ�����ʽ������
  * @param  None
  * @retval None
  */
void ws281x_sendTero(void)
{
  //GPIO_SetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  GPIOA->BSRR = WS281x_GPIO_PIN; 
  ws281x_delay(1);
  //GPIO_ResetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  GPIOA->BRR = WS281x_GPIO_PIN;
  ws281x_delay(2);
}

/**
  * @brief  ����1�룬�˴���ʱ��Ҫ��ܸߣ�����IO�˿�ֻ��ͨ���Ĵ�����ʽ������
  * @param  None
  * @retval None
  */
void ws281x_sendOne(void)
{
  //GPIO_SetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  GPIOA->BSRR = WS281x_GPIO_PIN;
  ws281x_delay(2);
  //GPIO_ResetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  GPIOA->BRR = WS281x_GPIO_PIN;
  ws281x_delay(1);
}

/**
  * @brief  ���͸�λ
  * @param  None
  * @retval None
  */
void ws281x_sendReset(void)
{
  GPIO_ResetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  ws281x_delay(114);  //��ʱ50us
  //GPIO_SetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
}

/**
  * @brief  ��ʾ���������Դ�������ʾ�������ϡ�
  * @param  None
  * @retval None
  */
void ws281x_show(void)
{
  for(uint16_t i = 0; i < PIXEL_NUM; ++i)
  {
    for(uint8_t j = 0; j < GRB; ++j)
    {
      if(pixelBuffer[i][j] == WS_HIGH)
      {
        ws281x_sendOne();
      }
      else
      {
        ws281x_sendTero();
      }
    }
  }
}

/**
  * @brief  close all led
  * @param  None
  * @retval None
  */
void ws281x_closeAll(void)
{
  uint16_t i;
  uint8_t j;
  
  for(i = 0; i < PIXEL_NUM; ++i)
  {
    for(j = 0; j < 24; ++j)
    {
      pixelBuffer[i][j] = WS_LOW;
    }
  }
  ws281x_show(); 
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
void ws281x_setPixelColor(uint16_t n, uint32_t GRBColor)
{
  uint8_t i;
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < GRB; i++)
    {
      pixelBuffer[n][i] = ((GRBColor << i) & 0x800000) ? WS_HIGH : WS_LOW;
    }
  }
}

void ws281x_setPixelRGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue)
{
  uint8_t i;
  
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < GRB; ++i)
    {
      pixelBuffer[n][i] = (((ws281x_color(red,green,blue) << i) & 0X800000) ? WS_HIGH : WS_LOW);
    }
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
void ws281x_colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<PIXEL_NUM; i++) {
    ws281x_setPixelColor(i, c);
    ws281x_show();
    delay_ms(wait);
  }
}

void ws281x_rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<PIXEL_NUM; i++) {
      ws281x_setPixelColor(i, ws281x_wheel((i+j) & 255));
    }
    ws281x_show();
    delay_ms(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void ws281x_rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< PIXEL_NUM; i++) {
      ws281x_setPixelColor(i,ws281x_wheel(((i * 256 / PIXEL_NUM) + j) & 255));
    }
    ws281x_show();
    delay_ms(wait);
  }
}

//Theatre-style crawling lights.
void ws281x_theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        ws281x_setPixelColor(i+q, c);    //turn every third pixel on
      }
      ws281x_show();

      delay_ms(wait);

      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        ws281x_setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void ws281x_theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        ws281x_setPixelColor(i+q, ws281x_wheel( (i+j) % 255));    //turn every third pixel on
      }
      ws281x_show();

      delay_ms(wait);

      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        ws281x_setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

