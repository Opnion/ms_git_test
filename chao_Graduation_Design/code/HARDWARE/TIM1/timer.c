/*
 * @Author: 马爽
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

// 灯光强度等级
extern int LED_Level[11];
// 等级数组标志位
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

// arr：自动重装值。
// psc：时钟预分频数
void Timer1_Init(u16 arr, u16 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // 时钟使能

  TIM_TimeBaseStructure.TIM_Period = arr;                     // 设置自动重装载寄存器周期值
  TIM_TimeBaseStructure.TIM_Prescaler = (psc - 1);            // 设置预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;                // 设置时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            // 重复计数设置
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);             // 参数初始化
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);                       // 清中断标志位

  TIM_ITConfig(           // 使能或者失能指定的TIM中断
      TIM1,               // TIM1
      TIM_IT_Update |     // TIM 更新中断源
          TIM_IT_Trigger, // TIM 触发中断源
      ENABLE              // 使能
  );

  // 设置优先级
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 从优先级0级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM1, ENABLE); // 使能TIMx外设
}

void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) // 检查指定的TIM中断发生与否:TIM 中断源
  {
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // 清除TIMx的中断待处理位:TIM 中断源

    if (BlueTooth_flag == 1)
    {
      ////////////////////////////////////////////////////////////////////////////////////
      // 扫描光敏按键开关(扫描八次,消抖)，然后光敏按键使能
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)
      {
        if (GuangMin_count == 20)
        {
          GuangMin_count = 0;
          // 按键使能与失能反转
          if (GuangMin_key == 1)
          {
            GuangMin_key = 0;
            OLED_ShowCHinese(113, 0, 4); // 待修改值     开、关
          }
          else if (GuangMin_key == 0)
          {
            GuangMin_key = 1;
            OLED_ShowCHinese(113, 0, 3); // 待修改值     开、关
          }
        }
        else
        {
          GuangMin_count++;
        }
      }

      // 光感按键使能，且模块检测后控制灯
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0 && GuangMin_key == 1)
      {
        GuangMin_flag = 1;
      }
      else
      {
        GuangMin_flag = 0;
      }

      ///////////////////////////////////////////////////////////////////////////////////
      // 扫描亮度增按键开关(扫描30次,消抖)，然后亮度增按键使能
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

      // 增亮按键使能，且模块检测后控制灯
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0 && Zeng_key == 1)
      {
        // 达到最高等级则不再增加
        if (Levei_flag < 10)
        {
          Levei_flag++;
        }
        else
        {
          Levei_flag = 0;
        }
        OLED_ShowNum(42, 6, Levei_flag, 2, 16); // 待修改值        数字
        Zeng_key = 0;
      }

      ///////////////////////////////////////////////////////////////////////////////////
      // 扫描亮度减按键开关(扫描30次,消抖)，然后亮度减按键使能
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

      // 减亮按键使能，且模块检测后控制灯
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0 && Jian_key == 1)
      {
        // 达到最低等级则不再增减
        if (Levei_flag > 0)
        {
          Levei_flag--;
        }
        else
        {
          Levei_flag = 10;
        }
        OLED_ShowNum(42, 6, Levei_flag, 2, 16); // 待修改值        数字
        Jian_key = 0;
      }
    }else if(BlueTooth_flag==0)
    {
      TIM_SetCompare2(TIM3, 0); // PB.5 输出高

    }

    ///////////////////////////////////////////////////////////////////////////////////
    // 超声波计数，30次一次
    // 超声波计数没到30，标志位置0，计数器加1
    // if (ChaoShengBo_count != 30)
    // {
    //   ChaoShengBo_flag = 0;
    //   ChaoShengBo_count++;
    // }
    // else
    // {
    //   // 计数器计数30次时计时器清0，标志位置1
    //   ChaoShengBo_count = 0;
    //   ChaoShengBo_flag = 1;
    // }

    // if (ChaoShengBo_flag == 1)
    // {
    //   // 超声波测距并显示
    //   SR_Distance = Hcsr04GetLength();
    //   if (SR_Distance >= 20)
    //   {

    //     OLED_ShowCHinese(59, 3, 3);	 //待修改值		 开、关
    //   }
    //   else
    //   {
    //    OLED_ShowCHinese(59, 3, 4);	 //待修改值		 开、关
    //   }
    // }

    ///////////////////////////////////////////////////////////////////////////////////

    // SR_Distance = Hcsr04GetLength();
    // OLED_Float(4, 70, SR_Distance, 3); // 显示超声波的距离
  }
}

// void Time2_NVIC_Config(void)
// {
//   NVIC_InitTypeDef NVIC_InitStruct;

//   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

//   NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
//   NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 只有一个中断，随意配
//   NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

//   NVIC_Init(&NVIC_InitStruct);
// }
// void Time2_Config(void)
// {
//   TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

//   TIM_TimeBaseInitStruct.TIM_Period = (1000 - 1);
//   TIM_TimeBaseInitStruct.TIM_Prescaler = (36 - 1); // 中断一次 1ms
//   //	不配置也可
//   //	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//   //	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  //默认向上
//   //	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x01;
//   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

//   // 配置中断
//   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//   TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//   // 使能计数
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

// 		// 	//距离小于10厘米蜂鸣器响
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
