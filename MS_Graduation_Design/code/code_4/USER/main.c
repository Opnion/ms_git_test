/*
 * @Author: 马爽
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-11-29 15:13:24
 * @LastEditTime: 2024-02-12 10:16:59
 * @FilePath: \USER\main.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "sys.h"

u8 KEY_GSM_flag = 0;		// 短信按键KEY1发送标志位
u8 KEY_WS2812B_flag = 0;	// RGB灯标志位，灯珠PCB未布线待飞线
u8 KEY_Buzzer_flag = 0;		// 蜂鸣器按键KEY0标志位
u8 KEY_Camera_flag = 0;		// 摄像头按键KEY2使能标志位
u8 KEY_Microphone_flag = 0; // 麦克风按键KEY2使能位
u8 KEY_Camera_BMP = 0;		// BMP格式拍照使能标志位，在LCD实时显示显示摄像头KEY1控制拍照
u8 Camera_flag = 0;			// LCD屏幕切换摄像头实时界面还是菜单界面标志位，按键KEY2
u8 BMP_GSM = 0;				// 区分拍照和发短信标志位，按键KEY2
u8 Microphone_flag = 0;		// 麦克风界面防一直刷屏
u8 Record_Star = 0;			// 开始录音
u8 Record_End = 0;			// 结束录音
u8 Record_Play = 0;			// 试听上一次录音
u8 voice_flag = 0;			// 保持录音模块开启状态

u8 hsync_int = 0;	// 帧中断标志
u8 ov2640_mode = 0; // 工作模式:0,RGB565模式;1,JPEG模式

#define jpeg_dma_bufsize 5 * 1024 // 定义JPEG DMA接收时数据缓存jpeg_buf0/1的大小(*4字节)
volatile u32 jpeg_data_len = 0;	  // buf中的JPEG有效数据长度(*4字节)
volatile u8 jpeg_data_ok = 0;	  // JPEG数据采集完成标志
								  // 0,数据没有采集完;
								  // 1,数据采集完了,但是还没处理;
								  // 2,数据已经处理完成了,可以开始下一帧接收

u32 *jpeg_buf0;		// JPEG数据缓存buf,通过malloc申请内存
u32 *jpeg_buf1;		// JPEG数据缓存buf,通过malloc申请内存
u32 *jpeg_data_buf; // JPEG数据缓存buf,通过malloc申请内存

void jpeg_data_process(void);
void jpeg_dcmi_rx_callback(void);
void sw_ov2640_mode(void);
void sw_sdcard_mode(void);
void camera_new_pathname(u8 *pname, u8 mode);
u8 ov2640_jpg_photo(u8 *pname);

int main(void)
{
	u8 res;	   // 文件夹是否创建成功标志位
	u8 *pname; // 带路径的文件名
	// u8 i;
	u8 sd_ok = 1; // 0,sd卡不正常;1,SD卡正常.
	// u8 scale = 1;  // 默认是全尺寸缩放
	// u8 msgbuf[15]; // 消息缓存区

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置系统中断优先级分组2
	delay_init(168);
	uart_init(115200);					// usart1，用于LCD传输显示
	TIM2_Init(1000, 8400);				// 用于蜂鸣器的音调
	TIM3_Int_Init(10000 - 1, 8400 - 1); // 10Khz计数,1秒钟中断一次
	GSM_Init(9600);						// GSM 用usart3来发短信
	Buzzer_Init();						// 无源蜂鸣器初始化（4000Hz）
	KEY_EXIT_Init();					// 按键初始化外部中断
	LCD_Init();							// 初始化LCD FSMC接口
	////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////

	FSMC_SRAM_Init();		 // 初始化外部SRAM.
	W25QXX_Init();			 // 初始化W25Q128
	my_mem_init(SRAMIN);	 // 初始化内部内存池
	my_mem_init(SRAMEX);	 // 初始化内部内存池
	my_mem_init(SRAMCCM);	 // 初始化CCM内存池
	exfuns_init();			 // 为fatfs相关变量申请内存
	f_mount(fs[0], "0:", 1); // 挂载SD卡
	LCD_Static_Display();	 // LCD的静态显示
	LCD_Dynamic_Display();	 // LCD的动态显示
	////////////////////////////////////////////////////////////////
	res = f_mkdir("0:/Mas_Photo");		 // 创建Mas_Photo文件夹
	if (res != FR_EXIST && res != FR_OK) // 发生了错误
	{
		POINT_COLOR = RED; // 画笔颜色：黑色
		Show_Str(30, 280, 240, 16, "SD_ERROR_1!", 16, 0);
		sd_ok = 0;
	}
	////////////////////////////////////////////////////////////////
	jpeg_buf0 = mymalloc(SRAMIN, jpeg_dma_bufsize * 4);					// 为jpeg dma接收申请内存
	jpeg_buf1 = mymalloc(SRAMIN, jpeg_dma_bufsize * 4);					// 为jpeg dma接收申请内存
	jpeg_data_buf = mymalloc(SRAMEX, 300 * 1024);						// 为jpeg文件申请内存(最大300KB)
	pname = mymalloc(SRAMIN, 30);										// 为带路径的文件名分配30个字节的内存
	while (pname == NULL || !jpeg_buf0 || !jpeg_buf1 || !jpeg_data_buf) // 内存分配出错
	{
		Show_Str(30, 190, 240, 16, "Malloc_ERROR!", 16, 0);
		delay_ms(200);
		LCD_Fill(30, 190, 240, 146, WHITE); // 清除显示
		delay_ms(200);
	}
	while (OV2640_Init()) // 初始化OV2640
	{
		Show_Str(30, 190, 240, 16, "OV2640_ERROR!", 16, 0);
		delay_ms(200);
		LCD_Fill(30, 190, 239, 206, WHITE);
		delay_ms(200);
	}
	Show_Str(30, 190, 200, 16, "OV2640_RIGHT", 16, 0);

	OV2640_RGB565_Mode();																		 // JPEG模式
	My_DCMI_Init();																				 // DCMI配置
	DCMI_DMA_Init((u32)&LCD->LCD_RAM, 0, 1, DMA_MemoryDataSize_HalfWord, DMA_MemoryInc_Disable); // DCMI DMA配置
	OV2640_OutSize_Set(lcddev.width, lcddev.height);

	delay_ms(500); // 初始化完成延迟0.5秒

	////////////////////////////////////////////////////////////////
	while (1)
	{

		/**
		 * @Description: 按键KEY0控制无源蜂鸣器响
		 * @return {*}
		 * @Author: 马爽
		 * @Date: 2024-01-26 00:20:27
		 */
		if (BMP_GSM == 0)
		{
			if (KEY_Buzzer_flag == 1)
			{
				LCD_Dynamic_Display();
				TIM2_Init(33, 2500);
				GPIO_SetBits(GPIOE, GPIO_Pin_2); // 开启使能
				delay_ms(300);
				GPIO_ResetBits(GPIOE, GPIO_Pin_2); // 关闭使能
				delay_ms(100);

				TIM2_Init(47, 4500);
				GPIO_SetBits(GPIOE, GPIO_Pin_2); // 开启使能
				delay_ms(400);
				GPIO_ResetBits(GPIOE, GPIO_Pin_2); // 关闭启使能
				delay_ms(500);
				KEY_Buzzer_flag = 0;
				LCD_Dynamic_Display();
			}
		}

		/**
		 * @Description: 按键KEY1通过串口3来控制短信电话模块GSM发送短信
		 * @return {*}
		 * @Author: 马爽
		 * @Date: 2024-01-26 00:21:05
		 */
		if (BMP_GSM == 0)
		{
			KEY_Camera_BMP = 0;
			if (KEY_GSM_flag == 1)
			{
				LCD_Dynamic_Display();
				delay_ms(200);
				USART3_Send_data("AT+CSCS=\"GSM\"\r\n"); // 短信格式设置为文本模式
				delay_ms(2000);
				USART3_Send_data("AT+CMGF=1\r\n");				 // 短信格式设置为文本模式
				delay_ms(2000);									 // 这里的延时可以更改但不能低于500MS如果太快模块接收不了
				USART3_Send_data("AT+CMGS=\"18029745289\"\r\n"); // 短信接受号码
				delay_ms(2000);
				USART3_Send_data("Someone is lingering at your doorstep.\r\n"); // 发送短信内容
				delay_ms(5000);
				USART3_Send_data_value(0x1a); // 发送回车结束符
				KEY_GSM_flag = 0;
				KEY_Buzzer_flag = 0; // 发送短信时按门铃不出声
				LCD_Dynamic_Display();
			}
		}

		/**
		 * @Description:按键KEY1控制摄像头拍照BMP格式
		 * @return {*}
		 * @Author: 马爽
		 * @Date: 2024-01-29 22:46:36
		 */
		if (BMP_GSM == 1)
		{
			KEY_GSM_flag = 0;
			if (KEY_Camera_BMP == 1)
			{
				if (sd_ok == 1) // SD卡正常才能拍照
				{
					sw_sdcard_mode();			   // 切换为SD卡模式
					camera_new_pathname(pname, 0); // 得到文件名
					res = bmp_encode(pname, 0, 0, lcddev.width, lcddev.height, 0);
					sw_ov2640_mode(); // 切换为OV2640模式
					if (res)		  // 拍照有误
					{
						Show_Str(30, 130, 240, 16, "WRITE_ERROR!", 16, 0);
					}
					else
					{
						//  蜂鸣器响0.3秒提醒拍照成功
						TIM2_Init(33, 2500);
						GPIO_SetBits(GPIOE, GPIO_Pin_2); // 开启使能
						delay_ms(300);
						GPIO_ResetBits(GPIOE, GPIO_Pin_2); // 关闭使能
						delay_ms(100);
					}
				}
				KEY_Camera_BMP = 0; // 拍完一张照，关闭使能
			}
		}

		if (hsync_int) // 刚刚产生帧中断,可以延时
		{
			delay_ms(10);
			hsync_int = 0;
		}

		/**
		 * @Description: 按键KEY2控制LCD在菜单界面和摄像头实时界面的切换
		 * @return {*}
		 * @Author: 马爽
		 * @Date: 2024-01-29 22:27:50
		 */
		if (KEY_Camera_flag == 1)
		{
			KEY_Buzzer_flag = 0;
			if (Camera_flag == 1)
			{
				DCMI_Start();
				BMP_GSM = 1;		 // 可以准备BMP拍照
				Camera_flag = 0;	 // 拍照和菜单互换
				KEY_Camera_flag = 0; // 按键清0
			}
			else if (Camera_flag == 0)
			{
				DCMI_Stop();
				LCD_Static_Display();  // LCD的静态显示
				LCD_Dynamic_Display(); // LCD的动态显示
				BMP_GSM = 0;		   // 关闭BMP拍照
				Camera_flag = 1;	   // 拍照和菜单互换
				KEY_Camera_flag = 0;   // 按键清0
			}
		}

		//		/**
		//		 * @Description: Do not edit
		//		 * @return {*}
		//		 * @Author: 马爽
		//		 * @Date: 2024-01-31 12:36:36
		//		 */
		//		if (KEY_Microphone_flag == 1)
		//		{
		//			wav_recorder();
		//		}
	}
}

// 处理JPEG数据
// 当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen; // 剩余数据长度
	u32 *pbuf;
	if (ov2640_mode) // 只有在JPEG格式下,才需要做处理.
	{
		if (jpeg_data_ok == 0) // jpeg数据还未采集完?
		{
			DMA_Cmd(DMA2_Stream1, DISABLE); // 停止当前传输
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE)
				;															// 等待DMA2_Stream1可配置
			rlen = jpeg_dma_bufsize - DMA_GetCurrDataCounter(DMA2_Stream1); // 得到剩余数据长度
			pbuf = jpeg_data_buf + jpeg_data_len;							// 偏移到有效数据末尾,继续添加
			if (DMA2_Stream1->CR & (1 << 19))
				for (i = 0; i < rlen; i++)
					pbuf[i] = jpeg_buf1[i]; // 读取buf1里面的剩余数据
			else
				for (i = 0; i < rlen; i++)
					pbuf[i] = jpeg_buf0[i]; // 读取buf0里面的剩余数据
			jpeg_data_len += rlen;			// 加上剩余长度
			jpeg_data_ok = 1;				// 标记JPEG数据采集完按成,等待其他函数处理
		}
		if (jpeg_data_ok == 2) // 上一次的jpeg数据已经被处理了
		{
			DMA_SetCurrDataCounter(DMA2_Stream1, jpeg_dma_bufsize); // 传输长度为jpeg_buf_size*4字节
			DMA_Cmd(DMA2_Stream1, ENABLE);							// 重新传输
			jpeg_data_ok = 0;										// 标记数据未采集
			jpeg_data_len = 0;										// 数据重新开始
		}
	}
	else
	{
		LCD_SetCursor(0, 0);
		LCD_WriteRAM_Prepare(); // 开始写入GRAM
		hsync_int = 1;
	}
}

// jpeg数据接收回调函数
void jpeg_dcmi_rx_callback(void)
{
	u16 i;
	u32 *pbuf;
	pbuf = jpeg_data_buf + jpeg_data_len; // 偏移到有效数据末尾
	if (DMA2_Stream1->CR & (1 << 19))	  // buf0已满,正常处理buf1
	{
		for (i = 0; i < jpeg_dma_bufsize; i++)
			pbuf[i] = jpeg_buf0[i];		   // 读取buf0里面的数据
		jpeg_data_len += jpeg_dma_bufsize; // 偏移
	}
	else // buf1已满,正常处理buf0
	{
		for (i = 0; i < jpeg_dma_bufsize; i++)
			pbuf[i] = jpeg_buf1[i];		   // 读取buf1里面的数据
		jpeg_data_len += jpeg_dma_bufsize; // 偏移
	}
}

// 切换为OV2640模式（GPIOC8/9/11切换为 DCMI接口）
void sw_ov2640_mode(void)
{
	OV2640_PWDN = 0;										 // OV2640 Power Up
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);	 // PC8,AF13  DCMI_D2
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_DCMI);	 // PC9,AF13  DCMI_D3
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI); // PC11,AF13 DCMI_D4
}
// 切换为SD卡模式（GPIOC8/9/11切换为 SDIO接口）
void sw_sdcard_mode(void)
{
	OV2640_PWDN = 1;										// OV2640 Power Down
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO); // PC8,AF12
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO); // PC9,AF12
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
}

// 文件名自增（避免覆盖）
// mode:0,创建.bmp文件;1,创建.jpg文件.
// bmp组合成:形如"0:PHOTO/PIC13141.bmp"的文件名
// jpg组合成:形如"0:PHOTO/PIC13141.jpg"的文件名
void camera_new_pathname(u8 *pname, u8 mode)
{
	u8 res;
	u16 index = 0;
	while (index < 0XFFFF)
	{
		if (mode == 0)
			sprintf((char *)pname, "0:Mas_Photo/Shuang_%05d.bmp", index);
		else
			sprintf((char *)pname, "0:Mas_Photo/Shuang_%05d.jpg", index);
		res = f_open(ftemp, (const TCHAR *)pname, FA_READ); // 尝试打开这个文件
		if (res == FR_NO_FILE)
			break; // 该文件名不存在=正是我们需要的.
		index++;
	}
}
// OV2640拍照jpg图片
// 返回值:0,成功
//     其他,错误代码
u8 ov2640_jpg_photo(u8 *pname)
{
	FIL *f_jpg;
	u8 res = 0;
	u32 bwr;
	u16 i;
	u8 *pbuf;
	f_jpg = (FIL *)mymalloc(SRAMIN, sizeof(FIL)); // 开辟FIL字节的内存区域
	if (f_jpg == NULL)
		return 0XFF; // 内存申请失败.
	ov2640_mode = 1;
	sw_ov2640_mode();																								// 切换为OV2640模式
	dcmi_rx_callback = jpeg_dcmi_rx_callback;																		// 回调函数
	DCMI_DMA_Init((u32)jpeg_buf0, (u32)jpeg_buf1, jpeg_dma_bufsize, DMA_MemoryDataSize_Word, DMA_MemoryInc_Enable); // DCMI DMA配置(双缓冲模式)
	OV2640_JPEG_Mode();																								// 切换为JPEG模式
	OV2640_ImageWin_Set(0, 0, 1600, 1200);
	OV2640_OutSize_Set(1600, 1200); // 拍照尺寸为1600*1200
	DCMI_Start();					// 启动传输
	while (jpeg_data_ok != 1)
		;			  // 等待第一帧图片采集完
	jpeg_data_ok = 2; // 忽略本帧图片,启动下一帧采集
	while (jpeg_data_ok != 1)
		;			  // 等待第二帧图片采集完
	jpeg_data_ok = 2; // 忽略本帧图片,启动下一帧采集
	while (jpeg_data_ok != 1)
		;		 // 等待第三帧图片采集完,第三帧,才保存到SD卡去.
	DCMI_Stop(); // 停止DMA搬运
	ov2640_mode = 0;
	sw_sdcard_mode();													 // 切换为SD卡模式
	res = f_open(f_jpg, (const TCHAR *)pname, FA_WRITE | FA_CREATE_NEW); // 模式0,或者尝试打开失败,则创建新文件
	if (res == 0)
	{
		printf("jpeg data size:%d\r\n", jpeg_data_len * 4); // 串口打印JPEG文件大小
		pbuf = (u8 *)jpeg_data_buf;
		for (i = 0; i < jpeg_data_len * 4; i++) // 查找0XFF,0XD8
		{
			if ((pbuf[i] == 0XFF) && (pbuf[i + 1] == 0XD8))
				break;
		}
		if (i == jpeg_data_len * 4)
			res = 0XFD; // 没找到0XFF,0XD8
		else			// 找到了
		{
			pbuf += i; // 偏移到0XFF,0XD8处
			res = f_write(f_jpg, pbuf, jpeg_data_len * 4 - i, &bwr);
			if (bwr != (jpeg_data_len * 4 - i))
				res = 0XFE;
		}
	}
	jpeg_data_len = 0;
	f_close(f_jpg);
	sw_ov2640_mode();																			 // 切换为OV2640模式
	OV2640_RGB565_Mode();																		 // RGB565模式
	DCMI_DMA_Init((u32)&LCD->LCD_RAM, 0, 1, DMA_MemoryDataSize_HalfWord, DMA_MemoryInc_Disable); // DCMI DMA配置
	myfree(SRAMIN, f_jpg);
	return res;
}

///////////////////////////////////////////////
/*
		MENU

DOOR:DISABLE/ENABLE
GSM :DISABLE/ENABLE
BEEP:DISABLE/ENABLE
CAMERA:DISABLE/ENABLE
Microphone:DISABLE/ENABLE


KEY0:BEEP/RECORD
KEY1:GSM/BMP/REPLAY
KEY2:CAMERA/Microphone

SD card will be ready!!!
CAMERA  will be ready!!!

*/
///////////////////////////////////////////////
