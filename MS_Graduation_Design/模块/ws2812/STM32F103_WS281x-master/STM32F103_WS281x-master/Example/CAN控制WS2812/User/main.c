/**
  ******************************************************************************
  * @file    main.c 
  * @author  Apex yuan
  * @version V1.0.0
  * @date    2019-8-16
  * @brief   Main program body
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "bsp.h"
#include "rgb_hsv.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t tTime[2] = {0}; //���ڼ�¼ʱ������

uint8_t msg_process_flag = 0; //���µĴ�����ָ��ñ�־λ��λ������ɸñ�־λ��λ
uint32_t colorArry[256] = {0xffffff, 0x00ff00, 0x61ff00, 0xffff00,0xff0000, 0xff00ff, 0x0000ff, 0x20a0f0}; //ɫ����׺�Ȼ���������
uint32_t color_num[2];

/* ���������߱� */
const uint16_t index_wave[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 
                               4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 12, 12, 
                               13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 30, 31, 32, 33, 
                               34, 36, 37, 38, 40, 41, 43, 45, 46, 48, 50, 52, 54, 56, 58, 60, 62, 65, 67, 70, 72, 75, 78, 81, 84, 87, 90, 
                               94, 97, 101, 105, 109, 113, 117, 122, 126, 131, 136, 141, 146, 152, 158, 164, 170, 176, 183, 190, 197, 205, 
                               213, 221, 229, 238, 247, 256, 256, 247, 238, 229, 221, 213, 205, 197, 190, 183, 176, 170, 164, 158, 152, 146, 
                               141, 136, 131, 126, 122, 117, 113, 109, 105, 101, 97, 94, 90, 87, 84, 81, 78, 75, 72, 70, 67, 65, 62, 60, 58, 
                               56, 54, 52, 50, 48, 46, 45, 43, 41, 40, 38, 37, 36, 34, 33, 32, 31, 30, 28, 27, 26, 25, 25, 24, 23, 22, 21, 20, 
                               20, 19, 18, 18, 17, 16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 
                               6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                               2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  bsp_init();
  
  printf("\r\nStart\r\n");
  /* ΪCAN���ͽṹ�帳��ֵ */
  can_tx_msg.StdId = 0x0;
  can_tx_msg.ExtId = 0xFFFF;     //��չID
  can_tx_msg.IDE = CAN_ID_EXT;   //��չ��ʽ
  can_tx_msg.RTR = CAN_RTR_DATA; //����֡
  can_tx_msg.DLC = 8;
  
  /* Infinite loop */
  while (1)
  {
    /*���ջ������������������㣬Ϊ�´ν���������׼��*/
    if(usart1_rx_cnt >= 8)
    {
      for(uint8_t i = 0; i < 8; ++i)
      {
        can_tx_msg.Data[i] = usart1_rx_buffer[i];
      }
      CAN_Transmit(CAN1, &can_tx_msg);
      /* Wait until one of the mailboxes is empty */
      while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || \
            (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || \
            (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));
      
      usart1_rx_cnt = 0; //��ռ���
    } 

    if(can_rx_flag == 1)
    {
      
      msg_process_flag = 1; //���յ���CAN��Ϣ��˵������Ϣ������
      
      /* ��ӡ���յ���CAN��Ϣ������ʹ�� */
      printf("\r\n%#x",can_rx_msg.ExtId);
      printf("\r\n%d",can_rx_msg.IDE);
      printf("\r\n%d",can_rx_msg.RTR);
      printf("\r\n%d",can_rx_msg.FMI);
      for(uint8_t i = 0; i < 8; ++i)
      {
        printf("\r\n%d",can_rx_msg.Data[i]);
      }
      can_rx_flag = 0;
    }

    /*�������ݴ���*/
    if(can_rx_msg.ExtId == 0xFFFF && can_rx_msg.DLC == 8) 
    {
      /* ��ȡɫ�� */
      color_num[EYE] = can_rx_msg.Data[0];
      color_num[EAR] = can_rx_msg.Data[2];
      
      /* EYE process */
      switch(can_rx_msg.Data[1])
      {
        /* �ر� */
        case 0x00: 
          if(ws281x_status[EYE] == WS281x_ON)
          {
            ws281x_closeAll(EYE);
          }
          break;
          
        /* ���� */
        case 0x01:
          for(uint16_t i = 0; i < EYE_PIXEL_NUM; ++i)
          {
            ws281x_setPixelColor(EYE, i, colorArry[color_num[EYE]]);
          }
          ws281x_show(EYE);
          ws281x_status[EYE] = WS281x_ON;
          break;
          
        /* ��˸ */  
        case 0x02:
          if(millis() - tTime[EYE] >= 500) //500ms��ʱ
          {
            if(ws281x_status[EYE] == WS281x_OFF) //��ǰ�Ƶ�״̬Ϊ��
            {
              for(uint16_t i = 0; i < EYE_PIXEL_NUM; ++i)
              {
                ws281x_setPixelColor(EYE, i, colorArry[color_num[EYE]]);
              }
              ws281x_show(EYE);
              ws281x_status[EYE] = WS281x_ON;
            }
            else
            {
              ws281x_closeAll(EYE);
              ws281x_status[EYE] = WS281x_OFF;
            }
            tTime[EYE] = millis();
          }
          break;
          
        /* ����˸���κ��� */
        case 0x03:
        {
          static uint8_t count = 0;
          
          /*ÿ���½��յ����ݾ�Ҫ��count���㣬����ֻ���ڽ��յ���һ��ָ��ʱ����˸*/
          if(msg_process_flag == 1) 
          {
            count = 0;
          }
          if(count < 4) //��˸���Σ��������ο����ιع�4�����ģ�
          {
            if(millis() - tTime[EYE] >= 500) //500ms��ʱ
            {
              if(ws281x_status[EYE] == WS281x_OFF) //��ǰ�Ƶ�״̬Ϊ��
              {
                for(uint16_t i = 0; i < EYE_PIXEL_NUM; ++i)
                {
                  ws281x_setPixelColor(EYE, i, colorArry[color_num[EYE]]);
                }
                ws281x_show(EYE);
                ws281x_status[EYE] = WS281x_ON;
              }
              else
              {
                ws281x_closeAll(EYE);
                ws281x_status[EYE] = WS281x_OFF;
              }
              tTime[EYE] = millis();
              count++;
            }
            
          }
          else //����
          {
            for(uint16_t i = 0; i < EYE_PIXEL_NUM; ++i)
            {
              ws281x_setPixelColor(EYE, i, colorArry[color_num[EYE]]);
            }
            ws281x_show(EYE);
            ws281x_status[EYE] = WS281x_ON;
          }
          break;
        }
      
        /* ������ */ 
        /*Ŀǰͨ��RGB��HSV��ɫģ��ת������һ��ɫ��Ϊ�˿���ʵ��ȫɫ����Ч������RGB��HSVģ��֮�����ɫת��������һЩ���⡣2019/8/26*/
        /* �޸���RGB��HSV֮��ת�������� 2019/8/28 */
        case 0x04:
        {
          uint8_t r,g,b = 0;
          static uint16_t count = 0;
          float h, s, v;
          
          if(millis() - tTime[EYE] >= 10)
          {
            rgb2hsv(colorArry[color_num[EYE]]>>8, colorArry[color_num[EYE]]>>16, colorArry[color_num[EYE]], &h, &s, &v);
            v = (float)index_wave[count] / 256;
            hsv2rgb(h, s, v, &r, &g, &b);
            
            for(uint16_t j = 0; j < EYE_PIXEL_NUM; ++j)
            {
              ws281x_setPixelRGB(EYE, j, r, g, b);              
            }
            ws281x_show(EYE);
            count++;
            if(count >= 300)
            {
              count = 0;
            }           
            tTime[EYE] = millis();
          }
          break;
        }  
        default:
          break;
      }
        
      /* EAR process */
      switch(can_rx_msg.Data[3])
      {
        /* �ر� */
        case 0x00: 
          if(ws281x_status[EAR] == WS281x_ON)
          {
            ws281x_closeAll(EAR);
          }
          break;
          
        /* ���� */
        case 0x01:
          for(uint16_t i = 0; i < EAR_PIXEL_NUM; ++i)
          {
            ws281x_setPixelColor(EAR, i, colorArry[color_num[EAR]]);
          }
          ws281x_show(EAR);
          ws281x_status[EAR] = WS281x_ON;
          break;
          
        /* ��˸ */  
        case 0x02:
          if(millis() - tTime[EAR] >= 500) //500ms��ʱ
          {
            if(ws281x_status[EAR] == WS281x_OFF) //��ǰ�Ƶ�״̬Ϊ��
            {
              for(uint16_t i = 0; i < EAR_PIXEL_NUM; ++i)
              {
                ws281x_setPixelColor(EAR, i, colorArry[color_num[EAR]]);
              }
              ws281x_show(EAR);
              ws281x_status[EAR] = WS281x_ON;
            }
            else
            {
              ws281x_closeAll(EAR);
              ws281x_status[EAR] = WS281x_OFF;
            }
            tTime[EAR] = millis();
          }
          break;
          
        /* ����˸���κ��� */
        case 0x03:
        {
          static uint8_t count = 0;
          
          /*ÿ���½��յ����ݾ�Ҫ��count���㣬����ֻ���ڽ��յ���һ��ָ��ʱ����˸*/
          if(msg_process_flag == 1) 
          {
            count = 0;
          }
          if(count < 4) //��˸���Σ��������ο����ιع�4�����ģ�
          {
            if(millis() - tTime[EAR] >= 500) //500ms��ʱ
            {
              if(ws281x_status[EAR] == WS281x_OFF) //��ǰ�Ƶ�״̬Ϊ��
              {
                for(uint16_t i = 0; i < EAR_PIXEL_NUM; ++i)
                {
                  ws281x_setPixelColor(EAR, i, colorArry[color_num[EAR]]);
                }
                ws281x_show(EAR);
                ws281x_status[EAR] = WS281x_ON;
              }
              else
              {
                ws281x_closeAll(EAR);
                ws281x_status[EAR] = WS281x_OFF;
              }
              tTime[EAR] = millis();
              count++;
            }
            
          }
          else //����
          {
            for(uint16_t i = 0; i < EAR_PIXEL_NUM; ++i)
            {
              ws281x_setPixelColor(EAR, i, colorArry[color_num[EAR]]);
            }
            ws281x_show(EAR);
            ws281x_status[EAR] = WS281x_ON;
          }
          break;
        }
      
        /* ������ */ 
        /*Ŀǰͨ��RGB��HSV��ɫģ��ת������һ��ɫ��Ϊ�˿���ʵ��ȫɫ����Ч������RGB��HSVģ��֮�����ɫת��������һЩ���⡣2019/8/26*/
        /* �޸���RGB��HSV֮��ת�������� 2019/8/28 */
        case 0x04:
        {
          uint8_t r,g,b = 0;
          static uint16_t count = 0;
          float h, s, v;
          
          if(millis() - tTime[EAR] >= 10)
          {
            rgb2hsv(colorArry[color_num[EAR]]>>8, colorArry[color_num[EAR]]>>16, colorArry[color_num[EAR]], &h, &s, &v);
            v = (float)index_wave[count] / 256;
            hsv2rgb(h, s, v, &r, &g, &b);
            
            for(uint16_t j = 0; j < EAR_PIXEL_NUM; ++j)
            {
              ws281x_setPixelRGB(EAR, j, r, g, b);              
            }
            ws281x_show(EAR);
            count++;
            if(count >= 300)
            {
              count = 0;
            }           
            tTime[EAR] = millis();
          }
          break;
        }  
        default:
          break;
      }
      
      msg_process_flag = 0; //��Ϣ������ɣ�����־λ����
    }
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
