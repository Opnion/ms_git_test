#include "stm32f10x.h"

#include "bsp.h"

int main(void)
{
  bsp_init();
    
  while(1)
  {
    /*���ջ������������������㣬Ϊ�´ν���������׼��*/
    if(usart1_rx_cnt >= 8)
    {
      usart1_rx_cnt = 0;
    } 
    
    /*�������ݴ���*/
    if(usart1_rx_buffer[0] == 0xff && usart1_rx_buffer[7] == 0xff) //��ͷ��β����ȷ
    {
      uint8_t time = usart1_rx_buffer[3];
      
      switch(usart1_rx_buffer[1])  //ģʽ
      {
        case 0x00:   //��ɫģʽ
          switch(usart1_rx_buffer[2]) //��ʾ��ʽ
          {
            case 0x00:  //�ر�
              WS281x_CloseAll();
              delay_ms(100);
              break;
            case 0x01:  //����
              for(uint16_t i = 0; i < PIXEL_NUM; ++i)
              {
                WS281x_SetPixelRGB(i, usart1_rx_buffer[4], usart1_rx_buffer[5], usart1_rx_buffer[6]);
              }
              WS281x_Show();
              delay_ms(100);
              break;
            case 0x02:  //��˸
              for(uint16_t i = 0; i < PIXEL_NUM; ++i)
              {
                WS281x_SetPixelRGB(i, usart1_rx_buffer[4], usart1_rx_buffer[5], usart1_rx_buffer[6]);
              }
              WS281x_Show();
              delay_ms(time * 10);
              WS281x_CloseAll();
              delay_ms(time * 10);
              break;
            case 0x03:  //�����ˮ��
              for(uint16_t i = 0; i < PIXEL_NUM; ++i)
              {
                WS281x_SetPixelRGB(i, usart1_rx_buffer[4], usart1_rx_buffer[5], usart1_rx_buffer[6]);
                WS281x_Show();
                delay_ms(time * 10);
                WS281x_SetPixelColor(i, 0x00);
              }
              break;
            case 0x04:
              WS281x_ColorWipe(WS281x_Color(usart1_rx_buffer[4], usart1_rx_buffer[5], usart1_rx_buffer[6]), time);
              break;
            case 0x05: 
              WS281x_TheaterChase(WS281x_Color(usart1_rx_buffer[4], usart1_rx_buffer[5], usart1_rx_buffer[6]), time);
            default:
              break;              
          }
          break;
        case 0x01:
          switch(usart1_rx_buffer[2])
          {
            case 0x01:
              WS281x_Rainbow(time);
              break;
            case 0x02:
              WS281x_RainbowCycle(time);
              break;
            case 0x03:
              WS281x_TheaterChaseRainbow(time);
              break;
            default:
              break;
          }
          break;
        default :
          break;
      }
    } 
  }
}
