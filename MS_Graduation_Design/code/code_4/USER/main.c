/*
 * @Author: ��ˬ
 * @version: 1.0.0
 * @Description: file content
 * @Date: 2023-11-29 15:13:24
 * @LastEditTime: 2024-02-12 10:16:59
 * @FilePath: \USER\main.c
 *
 * @shortcut key: ctrl+win+I/ctrl+win+T
 */
#include "sys.h"

u8 KEY_GSM_flag = 0;		// ���Ű���KEY1���ͱ�־λ
u8 KEY_WS2812B_flag = 0;	// RGB�Ʊ�־λ������PCBδ���ߴ�����
u8 KEY_Buzzer_flag = 0;		// ����������KEY0��־λ
u8 KEY_Camera_flag = 0;		// ����ͷ����KEY2ʹ�ܱ�־λ
u8 KEY_Microphone_flag = 0; // ��˷簴��KEY2ʹ��λ
u8 KEY_Camera_BMP = 0;		// BMP��ʽ����ʹ�ܱ�־λ����LCDʵʱ��ʾ��ʾ����ͷKEY1��������
u8 Camera_flag = 0;			// LCD��Ļ�л�����ͷʵʱ���滹�ǲ˵������־λ������KEY2
u8 BMP_GSM = 0;				// �������պͷ����ű�־λ������KEY2
u8 Microphone_flag = 0;		// ��˷�����һֱˢ��
u8 Record_Star = 0;			// ��ʼ¼��
u8 Record_End = 0;			// ����¼��
u8 Record_Play = 0;			// ������һ��¼��
u8 voice_flag = 0;			// ����¼��ģ�鿪��״̬

u8 hsync_int = 0;	// ֡�жϱ�־
u8 ov2640_mode = 0; // ����ģʽ:0,RGB565ģʽ;1,JPEGģʽ

#define jpeg_dma_bufsize 5 * 1024 // ����JPEG DMA����ʱ���ݻ���jpeg_buf0/1�Ĵ�С(*4�ֽ�)
volatile u32 jpeg_data_len = 0;	  // buf�е�JPEG��Ч���ݳ���(*4�ֽ�)
volatile u8 jpeg_data_ok = 0;	  // JPEG���ݲɼ���ɱ�־
								  // 0,����û�вɼ���;
								  // 1,���ݲɼ�����,���ǻ�û����;
								  // 2,�����Ѿ����������,���Կ�ʼ��һ֡����

u32 *jpeg_buf0;		// JPEG���ݻ���buf,ͨ��malloc�����ڴ�
u32 *jpeg_buf1;		// JPEG���ݻ���buf,ͨ��malloc�����ڴ�
u32 *jpeg_data_buf; // JPEG���ݻ���buf,ͨ��malloc�����ڴ�

void jpeg_data_process(void);
void jpeg_dcmi_rx_callback(void);
void sw_ov2640_mode(void);
void sw_sdcard_mode(void);
void camera_new_pathname(u8 *pname, u8 mode);
u8 ov2640_jpg_photo(u8 *pname);

int main(void)
{
	u8 res;	   // �ļ����Ƿ񴴽��ɹ���־λ
	u8 *pname; // ��·�����ļ���
	// u8 i;
	u8 sd_ok = 1; // 0,sd��������;1,SD������.
	// u8 scale = 1;  // Ĭ����ȫ�ߴ�����
	// u8 msgbuf[15]; // ��Ϣ������

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ����ϵͳ�ж����ȼ�����2
	delay_init(168);
	uart_init(115200);					// usart1������LCD������ʾ
	TIM2_Init(1000, 8400);				// ���ڷ�����������
	TIM3_Int_Init(10000 - 1, 8400 - 1); // 10Khz����,1�����ж�һ��
	GSM_Init(9600);						// GSM ��usart3��������
	Buzzer_Init();						// ��Դ��������ʼ����4000Hz��
	KEY_EXIT_Init();					// ������ʼ���ⲿ�ж�
	LCD_Init();							// ��ʼ��LCD FSMC�ӿ�
	////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////

	FSMC_SRAM_Init();		 // ��ʼ���ⲿSRAM.
	W25QXX_Init();			 // ��ʼ��W25Q128
	my_mem_init(SRAMIN);	 // ��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);	 // ��ʼ���ڲ��ڴ��
	my_mem_init(SRAMCCM);	 // ��ʼ��CCM�ڴ��
	exfuns_init();			 // Ϊfatfs��ر��������ڴ�
	f_mount(fs[0], "0:", 1); // ����SD��
	LCD_Static_Display();	 // LCD�ľ�̬��ʾ
	LCD_Dynamic_Display();	 // LCD�Ķ�̬��ʾ
	////////////////////////////////////////////////////////////////
	res = f_mkdir("0:/Mas_Photo");		 // ����Mas_Photo�ļ���
	if (res != FR_EXIST && res != FR_OK) // �����˴���
	{
		POINT_COLOR = RED; // ������ɫ����ɫ
		Show_Str(30, 280, 240, 16, "SD_ERROR_1!", 16, 0);
		sd_ok = 0;
	}
	////////////////////////////////////////////////////////////////
	jpeg_buf0 = mymalloc(SRAMIN, jpeg_dma_bufsize * 4);					// Ϊjpeg dma���������ڴ�
	jpeg_buf1 = mymalloc(SRAMIN, jpeg_dma_bufsize * 4);					// Ϊjpeg dma���������ڴ�
	jpeg_data_buf = mymalloc(SRAMEX, 300 * 1024);						// Ϊjpeg�ļ������ڴ�(���300KB)
	pname = mymalloc(SRAMIN, 30);										// Ϊ��·�����ļ�������30���ֽڵ��ڴ�
	while (pname == NULL || !jpeg_buf0 || !jpeg_buf1 || !jpeg_data_buf) // �ڴ�������
	{
		Show_Str(30, 190, 240, 16, "Malloc_ERROR!", 16, 0);
		delay_ms(200);
		LCD_Fill(30, 190, 240, 146, WHITE); // �����ʾ
		delay_ms(200);
	}
	while (OV2640_Init()) // ��ʼ��OV2640
	{
		Show_Str(30, 190, 240, 16, "OV2640_ERROR!", 16, 0);
		delay_ms(200);
		LCD_Fill(30, 190, 239, 206, WHITE);
		delay_ms(200);
	}
	Show_Str(30, 190, 200, 16, "OV2640_RIGHT", 16, 0);

	OV2640_RGB565_Mode();																		 // JPEGģʽ
	My_DCMI_Init();																				 // DCMI����
	DCMI_DMA_Init((u32)&LCD->LCD_RAM, 0, 1, DMA_MemoryDataSize_HalfWord, DMA_MemoryInc_Disable); // DCMI DMA����
	OV2640_OutSize_Set(lcddev.width, lcddev.height);

	delay_ms(500); // ��ʼ������ӳ�0.5��

	////////////////////////////////////////////////////////////////
	while (1)
	{

		/**
		 * @Description: ����KEY0������Դ��������
		 * @return {*}
		 * @Author: ��ˬ
		 * @Date: 2024-01-26 00:20:27
		 */
		if (BMP_GSM == 0)
		{
			if (KEY_Buzzer_flag == 1)
			{
				LCD_Dynamic_Display();
				TIM2_Init(33, 2500);
				GPIO_SetBits(GPIOE, GPIO_Pin_2); // ����ʹ��
				delay_ms(300);
				GPIO_ResetBits(GPIOE, GPIO_Pin_2); // �ر�ʹ��
				delay_ms(100);

				TIM2_Init(47, 4500);
				GPIO_SetBits(GPIOE, GPIO_Pin_2); // ����ʹ��
				delay_ms(400);
				GPIO_ResetBits(GPIOE, GPIO_Pin_2); // �ر���ʹ��
				delay_ms(500);
				KEY_Buzzer_flag = 0;
				LCD_Dynamic_Display();
			}
		}

		/**
		 * @Description: ����KEY1ͨ������3�����ƶ��ŵ绰ģ��GSM���Ͷ���
		 * @return {*}
		 * @Author: ��ˬ
		 * @Date: 2024-01-26 00:21:05
		 */
		if (BMP_GSM == 0)
		{
			KEY_Camera_BMP = 0;
			if (KEY_GSM_flag == 1)
			{
				LCD_Dynamic_Display();
				delay_ms(200);
				USART3_Send_data("AT+CSCS=\"GSM\"\r\n"); // ���Ÿ�ʽ����Ϊ�ı�ģʽ
				delay_ms(2000);
				USART3_Send_data("AT+CMGF=1\r\n");				 // ���Ÿ�ʽ����Ϊ�ı�ģʽ
				delay_ms(2000);									 // �������ʱ���Ը��ĵ����ܵ���500MS���̫��ģ����ղ���
				USART3_Send_data("AT+CMGS=\"18029745289\"\r\n"); // ���Ž��ܺ���
				delay_ms(2000);
				USART3_Send_data("Someone is lingering at your doorstep.\r\n"); // ���Ͷ�������
				delay_ms(5000);
				USART3_Send_data_value(0x1a); // ���ͻس�������
				KEY_GSM_flag = 0;
				KEY_Buzzer_flag = 0; // ���Ͷ���ʱ�����岻����
				LCD_Dynamic_Display();
			}
		}

		/**
		 * @Description:����KEY1��������ͷ����BMP��ʽ
		 * @return {*}
		 * @Author: ��ˬ
		 * @Date: 2024-01-29 22:46:36
		 */
		if (BMP_GSM == 1)
		{
			KEY_GSM_flag = 0;
			if (KEY_Camera_BMP == 1)
			{
				if (sd_ok == 1) // SD��������������
				{
					sw_sdcard_mode();			   // �л�ΪSD��ģʽ
					camera_new_pathname(pname, 0); // �õ��ļ���
					res = bmp_encode(pname, 0, 0, lcddev.width, lcddev.height, 0);
					sw_ov2640_mode(); // �л�ΪOV2640ģʽ
					if (res)		  // ��������
					{
						Show_Str(30, 130, 240, 16, "WRITE_ERROR!", 16, 0);
					}
					else
					{
						//  ��������0.3���������ճɹ�
						TIM2_Init(33, 2500);
						GPIO_SetBits(GPIOE, GPIO_Pin_2); // ����ʹ��
						delay_ms(300);
						GPIO_ResetBits(GPIOE, GPIO_Pin_2); // �ر�ʹ��
						delay_ms(100);
					}
				}
				KEY_Camera_BMP = 0; // ����һ���գ��ر�ʹ��
			}
		}

		if (hsync_int) // �ող���֡�ж�,������ʱ
		{
			delay_ms(10);
			hsync_int = 0;
		}

		/**
		 * @Description: ����KEY2����LCD�ڲ˵����������ͷʵʱ������л�
		 * @return {*}
		 * @Author: ��ˬ
		 * @Date: 2024-01-29 22:27:50
		 */
		if (KEY_Camera_flag == 1)
		{
			KEY_Buzzer_flag = 0;
			if (Camera_flag == 1)
			{
				DCMI_Start();
				BMP_GSM = 1;		 // ����׼��BMP����
				Camera_flag = 0;	 // ���պͲ˵�����
				KEY_Camera_flag = 0; // ������0
			}
			else if (Camera_flag == 0)
			{
				DCMI_Stop();
				LCD_Static_Display();  // LCD�ľ�̬��ʾ
				LCD_Dynamic_Display(); // LCD�Ķ�̬��ʾ
				BMP_GSM = 0;		   // �ر�BMP����
				Camera_flag = 1;	   // ���պͲ˵�����
				KEY_Camera_flag = 0;   // ������0
			}
		}

		//		/**
		//		 * @Description: Do not edit
		//		 * @return {*}
		//		 * @Author: ��ˬ
		//		 * @Date: 2024-01-31 12:36:36
		//		 */
		//		if (KEY_Microphone_flag == 1)
		//		{
		//			wav_recorder();
		//		}
	}
}

// ����JPEG����
// ���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen; // ʣ�����ݳ���
	u32 *pbuf;
	if (ov2640_mode) // ֻ����JPEG��ʽ��,����Ҫ������.
	{
		if (jpeg_data_ok == 0) // jpeg���ݻ�δ�ɼ���?
		{
			DMA_Cmd(DMA2_Stream1, DISABLE); // ֹͣ��ǰ����
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE)
				;															// �ȴ�DMA2_Stream1������
			rlen = jpeg_dma_bufsize - DMA_GetCurrDataCounter(DMA2_Stream1); // �õ�ʣ�����ݳ���
			pbuf = jpeg_data_buf + jpeg_data_len;							// ƫ�Ƶ���Ч����ĩβ,�������
			if (DMA2_Stream1->CR & (1 << 19))
				for (i = 0; i < rlen; i++)
					pbuf[i] = jpeg_buf1[i]; // ��ȡbuf1�����ʣ������
			else
				for (i = 0; i < rlen; i++)
					pbuf[i] = jpeg_buf0[i]; // ��ȡbuf0�����ʣ������
			jpeg_data_len += rlen;			// ����ʣ�೤��
			jpeg_data_ok = 1;				// ���JPEG���ݲɼ��갴��,�ȴ�������������
		}
		if (jpeg_data_ok == 2) // ��һ�ε�jpeg�����Ѿ���������
		{
			DMA_SetCurrDataCounter(DMA2_Stream1, jpeg_dma_bufsize); // ���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA_Cmd(DMA2_Stream1, ENABLE);							// ���´���
			jpeg_data_ok = 0;										// �������δ�ɼ�
			jpeg_data_len = 0;										// �������¿�ʼ
		}
	}
	else
	{
		LCD_SetCursor(0, 0);
		LCD_WriteRAM_Prepare(); // ��ʼд��GRAM
		hsync_int = 1;
	}
}

// jpeg���ݽ��ջص�����
void jpeg_dcmi_rx_callback(void)
{
	u16 i;
	u32 *pbuf;
	pbuf = jpeg_data_buf + jpeg_data_len; // ƫ�Ƶ���Ч����ĩβ
	if (DMA2_Stream1->CR & (1 << 19))	  // buf0����,��������buf1
	{
		for (i = 0; i < jpeg_dma_bufsize; i++)
			pbuf[i] = jpeg_buf0[i];		   // ��ȡbuf0���������
		jpeg_data_len += jpeg_dma_bufsize; // ƫ��
	}
	else // buf1����,��������buf0
	{
		for (i = 0; i < jpeg_dma_bufsize; i++)
			pbuf[i] = jpeg_buf1[i];		   // ��ȡbuf1���������
		jpeg_data_len += jpeg_dma_bufsize; // ƫ��
	}
}

// �л�ΪOV2640ģʽ��GPIOC8/9/11�л�Ϊ DCMI�ӿڣ�
void sw_ov2640_mode(void)
{
	OV2640_PWDN = 0;										 // OV2640 Power Up
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);	 // PC8,AF13  DCMI_D2
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_DCMI);	 // PC9,AF13  DCMI_D3
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI); // PC11,AF13 DCMI_D4
}
// �л�ΪSD��ģʽ��GPIOC8/9/11�л�Ϊ SDIO�ӿڣ�
void sw_sdcard_mode(void)
{
	OV2640_PWDN = 1;										// OV2640 Power Down
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO); // PC8,AF12
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO); // PC9,AF12
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
}

// �ļ������������⸲�ǣ�
// mode:0,����.bmp�ļ�;1,����.jpg�ļ�.
// bmp��ϳ�:����"0:PHOTO/PIC13141.bmp"���ļ���
// jpg��ϳ�:����"0:PHOTO/PIC13141.jpg"���ļ���
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
		res = f_open(ftemp, (const TCHAR *)pname, FA_READ); // ���Դ�����ļ�
		if (res == FR_NO_FILE)
			break; // ���ļ���������=����������Ҫ��.
		index++;
	}
}
// OV2640����jpgͼƬ
// ����ֵ:0,�ɹ�
//     ����,�������
u8 ov2640_jpg_photo(u8 *pname)
{
	FIL *f_jpg;
	u8 res = 0;
	u32 bwr;
	u16 i;
	u8 *pbuf;
	f_jpg = (FIL *)mymalloc(SRAMIN, sizeof(FIL)); // ����FIL�ֽڵ��ڴ�����
	if (f_jpg == NULL)
		return 0XFF; // �ڴ�����ʧ��.
	ov2640_mode = 1;
	sw_ov2640_mode();																								// �л�ΪOV2640ģʽ
	dcmi_rx_callback = jpeg_dcmi_rx_callback;																		// �ص�����
	DCMI_DMA_Init((u32)jpeg_buf0, (u32)jpeg_buf1, jpeg_dma_bufsize, DMA_MemoryDataSize_Word, DMA_MemoryInc_Enable); // DCMI DMA����(˫����ģʽ)
	OV2640_JPEG_Mode();																								// �л�ΪJPEGģʽ
	OV2640_ImageWin_Set(0, 0, 1600, 1200);
	OV2640_OutSize_Set(1600, 1200); // ���ճߴ�Ϊ1600*1200
	DCMI_Start();					// ��������
	while (jpeg_data_ok != 1)
		;			  // �ȴ���һ֡ͼƬ�ɼ���
	jpeg_data_ok = 2; // ���Ա�֡ͼƬ,������һ֡�ɼ�
	while (jpeg_data_ok != 1)
		;			  // �ȴ��ڶ�֡ͼƬ�ɼ���
	jpeg_data_ok = 2; // ���Ա�֡ͼƬ,������һ֡�ɼ�
	while (jpeg_data_ok != 1)
		;		 // �ȴ�����֡ͼƬ�ɼ���,����֡,�ű��浽SD��ȥ.
	DCMI_Stop(); // ֹͣDMA����
	ov2640_mode = 0;
	sw_sdcard_mode();													 // �л�ΪSD��ģʽ
	res = f_open(f_jpg, (const TCHAR *)pname, FA_WRITE | FA_CREATE_NEW); // ģʽ0,���߳��Դ�ʧ��,�򴴽����ļ�
	if (res == 0)
	{
		printf("jpeg data size:%d\r\n", jpeg_data_len * 4); // ���ڴ�ӡJPEG�ļ���С
		pbuf = (u8 *)jpeg_data_buf;
		for (i = 0; i < jpeg_data_len * 4; i++) // ����0XFF,0XD8
		{
			if ((pbuf[i] == 0XFF) && (pbuf[i + 1] == 0XD8))
				break;
		}
		if (i == jpeg_data_len * 4)
			res = 0XFD; // û�ҵ�0XFF,0XD8
		else			// �ҵ���
		{
			pbuf += i; // ƫ�Ƶ�0XFF,0XD8��
			res = f_write(f_jpg, pbuf, jpeg_data_len * 4 - i, &bwr);
			if (bwr != (jpeg_data_len * 4 - i))
				res = 0XFE;
		}
	}
	jpeg_data_len = 0;
	f_close(f_jpg);
	sw_ov2640_mode();																			 // �л�ΪOV2640ģʽ
	OV2640_RGB565_Mode();																		 // RGB565ģʽ
	DCMI_DMA_Init((u32)&LCD->LCD_RAM, 0, 1, DMA_MemoryDataSize_HalfWord, DMA_MemoryInc_Disable); // DCMI DMA����
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
