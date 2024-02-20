/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-12-13 05:52:05
 * @LastEditTime: 2024-01-18 01:42:03
 * @FilePath: \USERe:\chao_Graduation Design\code\HARDWARE\TIM1\timer.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "timer.h"
#include "led.h"

// �ƹ�ǿ�ȵȼ�
extern int LED_Level[11];
// �ȼ������־λ
extern int Levei_flag;

extern u8 BlueTooth_flag;
extern u8 GuangMin_flag;
extern u8 ChaoShengBo_flag;
extern u8 Zeng_flag;
extern u8 Jian_flag;

extern u8 BlueTooth_count;
extern u8 GuangMin_count;
extern u8 ChaoShengBo_count;
extern u8 Zeng_count;
extern u8 Jian_count;

extern u8 BlueTooth_key;
extern u8 GuangMin_key;
extern u8 ChaoShengBo_key;
extern u8 Zeng_key;
extern u8 Jian_key;

extern int pwmValue;

extern uint16_t timer;

// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
void Timer1_Init(u16 arr, u16 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // ʱ��ʹ��

  TIM_TimeBaseStructure.TIM_Period = arr;                     // �����Զ���װ�ؼĴ�������ֵ
  TIM_TimeBaseStructure.TIM_Prescaler = (psc - 1);            // ����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;                // ����ʱ�ӷָ�
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            // �ظ���������
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);             // ������ʼ��
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);                       // ���жϱ�־λ

  TIM_ITConfig(           // ʹ�ܻ���ʧ��ָ����TIM�ж�
      TIM1,               // TIM1
      TIM_IT_Update |     // TIM �����ж�Դ
          TIM_IT_Trigger, // TIM �����ж�Դ
      ENABLE              // ʹ��
  );

  // �������ȼ�
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // �����ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM1, ENABLE); // ʹ��TIMx����
}

void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) // ���ָ����TIM�жϷ������:TIM �ж�Դ
  {
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // ���TIMx���жϴ�����λ:TIM �ж�Դ

    if (BlueTooth_flag == 1)
    {
      ////////////////////////////////////////////////////////////////////////////////////
      // ɨ�������������(ɨ��˴�,����)��Ȼ���������ʹ��
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)
      {
        if (GuangMin_count == 20)
        {
          GuangMin_count = 0;
          // ����ʹ����ʧ�ܷ�ת
          if (GuangMin_key == 1)
          {
            GuangMin_key = 0;
            OLED_ShowCHinese(113, 0, 4); // ���޸�ֵ     ������
          }
          else if (GuangMin_key == 0)
          {
            GuangMin_key = 1;
            OLED_ShowCHinese(113, 0, 3); // ���޸�ֵ     ������
          }
        }
        else
        {
          GuangMin_count++;
        }
      }

      // ��а���ʹ�ܣ���ģ�������Ƶ�
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0 && GuangMin_key == 1)
      {
        GuangMin_flag = 1;
      }
      else
      {
        GuangMin_flag = 0;
      }

      ///////////////////////////////////////////////////////////////////////////////////
      // ɨ����������������(ɨ��30��,����)��Ȼ������������ʹ��
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0)
      {
        if (Zeng_count == 20)
        {
          Zeng_count = 0;
          Zeng_key = 1;
        }
        else
        {
          Zeng_count++;
        }
      }

      // ��������ʹ�ܣ���ģ�������Ƶ�
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0 && Zeng_key == 1)
      {
        // �ﵽ��ߵȼ���������
        if (Levei_flag < 10)
        {
          Levei_flag++;
        }
        else
        {
          Levei_flag = 0;
        }
        OLED_ShowNum(42, 6, Levei_flag, 2, 16); // ���޸�ֵ        ����
        Zeng_key = 0;
      }

      ///////////////////////////////////////////////////////////////////////////////////
      // ɨ�����ȼ���������(ɨ��30��,����)��Ȼ�����ȼ�����ʹ��
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0)
      {
        if (Jian_count == 20)
        {
          Jian_count = 0;
          Jian_key = 1;
        }
        else
        {
          Jian_count++;
        }
      }

      // ��������ʹ�ܣ���ģ�������Ƶ�
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0 && Jian_key == 1)
      {
        // �ﵽ��͵ȼ���������
        if (Levei_flag > 0)
        {
          Levei_flag--;
        }
        else
        {
          Levei_flag = 10;
        }
        OLED_ShowNum(42, 6, Levei_flag, 2, 16); // ���޸�ֵ        ����
        Jian_key = 0;
      }
    }else if(BlueTooth_flag==0)
    {
      TIM_SetCompare2(TIM3, 0); // PB.5 �����

    }

    ///////////////////////////////////////////////////////////////////////////////////
    // ������������30��һ��
    // ����������û��30����־λ��0����������1
    // if (ChaoShengBo_count != 30)
    // {
    //   ChaoShengBo_flag = 0;
    //   ChaoShengBo_count++;
    // }
    // else
    // {
    //   // ����������30��ʱ��ʱ����0����־λ��1
    //   ChaoShengBo_count = 0;
    //   ChaoShengBo_flag = 1;
    // }

    // if (ChaoShengBo_flag == 1)
    // {
    //   // ��������ಢ��ʾ
    //   SR_Distance = Hcsr04GetLength();
    //   if (SR_Distance >= 20)
    //   {

    //     OLED_ShowCHinese(59, 3, 3);	 //���޸�ֵ		 ������
    //   }
    //   else
    //   {
    //    OLED_ShowCHinese(59, 3, 4);	 //���޸�ֵ		 ������
    //   }
    // }

    ///////////////////////////////////////////////////////////////////////////////////

    // SR_Distance = Hcsr04GetLength();
    // OLED_Float(4, 70, SR_Distance, 3); // ��ʾ�������ľ���
  }
}

// void Time2_NVIC_Config(void)
// {
//   NVIC_InitTypeDef NVIC_InitStruct;

//   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

//   NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
//   NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // ֻ��һ���жϣ�������
//   NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

//   NVIC_Init(&NVIC_InitStruct);
// }
// void Time2_Config(void)
// {
//   TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

//   TIM_TimeBaseInitStruct.TIM_Period = (1000 - 1);
//   TIM_TimeBaseInitStruct.TIM_Prescaler = (36 - 1); // �ж�һ�� 1ms
//   //	������Ҳ��
//   //	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//   //	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  //Ĭ������
//   //	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x01;
//   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

//   // �����ж�
//   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//   TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//   // ʹ�ܼ���
//   TIM_Cmd(TIM2, ENABLE);
// }

// void Timer2_Init(void)
// {
//   Time2_NVIC_Config();
//   Time2_Config();
// }

// void TIM2_IRQHandler(void)
// {
// 	if(TIM_GetITStatus(TIM2,TIM_IT_Update))
// 	{
// 		// timer++;
//     // if (timer == 200)
// 		// {
// 		// 	OLED_Show();
// 		// 	timer = 0;

// 		// 	//����С��10���׷�������
// 		// 	if (SR_Distance <= 10)
// 		// 	{
// 		// 		GPIO_SetBits(GPIOB, GPIO_Pin_4);
// 		// 	}
// 		// 	else
// 		// 	{
// 		// 		GPIO_ResetBits(GPIOB, GPIO_Pin_4);
// 		// 	}
// 		// }
// 	}
// 	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
// }
