#include "sd_card.h"
//0x55������Ӧ��ʱ 0x44�����ݳ�ʱ
SDML CMD0={0x40,{0x00,0x00,0x00,0x00},0x95,0};//����ģʽ
SDML CMD8={0x48,{0x00,0x00,0x01,0xaa},0x87,7};//��ȡ��ѹ
SDML CMD9={0x49,{0x00,0x00,0x00,0x00},0x01,0};//��ȡCSD
SDML CMD10={0x4a,{0x00,0x00,0x0,0x00},0x01,0};//��ȡCID
SDML CMD12={0x4c,{0x00,0x00,0x0,0x00},0x01,1};//��������ȡ����
SDML CMD13={0x4d,{0x00,0x00,0x0,0x00},0x01,2};//��ȡ״̬
SDML CMD17={0x51,{0x00,0x00,0x0,0x00},0x01,0};//�����ȡ����
SDML CMD18={0x52,{0x00,0x00,0x0,0x00},0x01,0};//����ȡ����
SDML CMD24={0x58,{0x00,0x00,0x0,0x00},0x01,0};//����д������
SDML CMD25={0x59,{0x00,0x00,0x0,0x00},0x01,0};//���д������
SDML CMD55={0x77,{0x00,0x00,0x00,0x00},0x01,0};//��ACMD�������ʹ��
SDML CMD58={0x7a,{0x00,0x00,0x00,0x00},0x01,7};
SDML CMD59={0x7b,{0x00,0x00,0x00,0x00},0x01,0};//CRC��/�ر�
SDML ACMD41={0x69,{0x40,0x00,0x00,0x00},0x01,0};

uint SDKDX=0;//SD�����С
uchar SDCRC[2];//CRC����
uchar r2_zhuang_tai=0;//R2��Ӧ״̬�ֽڴ洢
uchar sd_r1_xy=0;//r1��Ӧ�洢
uchar sd_r7_xy[5];//r3/r7��Ӧ�洢
//д�������
void SDXRML(SDML a)
{
	SPI_XD_DATA(a.a);
	for(uint i=0;i<4;i++)
	{
		SPI_XD_DATA(a.b[i]);	
	}
	SPI_XD_DATA(a.c);
}
//��ȡ��Ӧ����
ERROR_XY SDDQXY(uchar a)
{
	int XY_BZW=0;
	uint i=0;
	switch(a)
	{
		case 0://R1��Ӧ
			while(XY_BZW<50)
			{
				XY_BZW++;
				sd_r1_xy=SPI_XD_DATA(0xff);
				if(sd_r1_xy==0x01||sd_r1_xy==0x00)
				{
					XY_BZW=50;
				}	
			}
			//����ʱ����0x55
			if(XY_BZW==50)
				return 0x55;
			break;
		case 1://R1b��Ӧ
			sd_r1_xy=SPI_XD_DATA(0xff);
			while(XY_BZW<50)
			{
				XY_BZW++;
				if((sd_r1_xy&0x80)==0x00)
				{
					XY_BZW=50;
				}	
			}
			//����ʱ����0x55
			if(XY_BZW!=50)
				return 0x55;
			break;
		case 2://R2��Ӧ
			while(XY_BZW<50)
			{
				XY_BZW++;
				if(((sd_r1_xy=SPI_XD_DATA(0xff))&0x80)==0x00)
				{
					r2_zhuang_tai=SPI_XD_DATA(0xff);
					XY_BZW=50;
				}
				
			}
			if(XY_BZW!=50)
				return 0x55;
			break;
		case 3://R3��Ӧ
			while(i<50)
			{
				switch(sd_r7_xy[0]=SPI_XD_DATA(0xff))
				{
					case 0x00:
						for(uint x1=1;x1<5;x1++)
						{
							sd_r7_xy[x1]=SPI_XD_DATA(0xff);
						}
						i=50;
						break;
					case 0x01:
						for(uint x1=1;x1<5;x1++)
						{
							sd_r7_xy[x1]=SPI_XD_DATA(0xff);
						}
						i=50;
						break;
					default:
						i++;
						break;
				}	
			}
			break;
		case 4://R4��Ӧ
			break;
		case 5://R5��Ӧ
			break;
		case 7://R7��Ӧ
			while(XY_BZW<50)
			{
				XY_BZW++;
				sd_r7_xy[0]=SPI_XD_DATA(0xff);
				if((sd_r7_xy[0]&0x80)==0x00)
				{
					for(uint x1=1;x1<5;x1++)
					{
						sd_r7_xy[x1]=SPI_XD_DATA(0xff);
					}
					XY_BZW=50;
				}	
			}
			//����ʱ����0x55
			if(XY_BZW!=50)
				return 0x55;
			break;
	}
	return 0;
}
//a:��ȡ����(0:�����ȡ,1:����ȡ)-b:��ȡ�����ݴ�ŵĵ�ַ-c��ȡ�Ŀ��С-d��ȡ�Ŀ�����
ERROR SDDQSJ(uchar a,uchar *b,uint c,uint d)
{
	int DU_QU_BZW=1;
	switch(a)
	{
		case 0://��ȡsd���洢���ݵ����ȡ
			//��ȡ��ʼ�ֽ�
			while(DU_QU_BZW)
			{
				DU_QU_BZW++;
				if (SPI_XD_DATA(0xff)==0xfe)
				{
					DU_QU_BZW =0;
				}
				if(DU_QU_BZW>1000)
				return 0x55;
			}
			//��ȡ����
			for(uint i=0;i<c;i++)
			{
				b[i]=SPI_XD_DATA(0xff);
			}
			SDCRC[0]=SPI_XD_DATA(0xff);
			SDCRC[1]=SPI_XD_DATA(0xff);
			while(SPI_XD_DATA(0xff)!=0xff);
			return 0;					
		case 1://��ȡsd���洢���ݶ���ȡ
			for(uint i=0;i<d;i++)
			{
				DU_QU_BZW=1;
				//��ȡ��ʼ�ֽ�+��ʱ���
				while(DU_QU_BZW)
				{
					DU_QU_BZW++;
					if (SPI_XD_DATA(0xff)==0xfe)
					{
						DU_QU_BZW =0;
					}
					if(DU_QU_BZW>1000)
					return 0x55;
				}
				for(uint ii=0;ii<c;ii++)
				{
					b[ii+(512*i)]=SPI_XD_DATA(0xff);
				}
				SDCRC[0]=SPI_XD_DATA(0xff);
				SDCRC[1]=SPI_XD_DATA(0xff);
			}
			SDXRML(CMD12);//д��CMD12����ֹͣ��ȡ����
			SDDQXY(CMD12.d);
			return 0;
	}
	return 1;
}
//sd����Ϣ����CID CSD�Ĵ���
void SDXXCL(SDCS * a)
{
	//CSD��Ϣ����
	//��ȡ���Ŀ��С
	switch(a->d[5]&0x0f)
	{	
		case 9:
			a->e=512;
			break;
		case 10:
			a->e=1024;
			break;		
		case 11:
			a->e=2048;
			break;
	}
	//��ȡ���Ŀ�����(������������)���ڼ��㿨������
	a->f=a->d[8]*256+a->d[9]+1;
	//���ÿ鳤��
	SDKDX=a->e;
	//sd��ͨѶ���
	a->sd_card_zt=1;
}
//SD����ʼ������
ERROR SDCRADCSH(SDCS * a)
{
	int ml_xun_huan_=1;
	//����ʱ��
	for(uint i=0;i<14;i++)
	{
		SPI_XD_DATA(0xff);
	}
	//--------------------------------����SPIģʽ��CMD0����+Ƭѡ�͵�ƽ��
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);//��cs��������
	//CMD0ָ��
	while(ml_xun_huan_)
	{
		ml_xun_huan_++;//���ܷ��������ж�֮������˲���ѭ��
		SDXRML(CMD0);//д��CMD0����
		SDDQXY(CMD0.d);
		//��ȡCMD0��Ӧ���ж�
		if(sd_r1_xy==0x01)
		{
			//���CMD0������
			ml_xun_huan_=0;
		}
		if(ml_xun_huan_>60)
		return 1;
	}
	ml_xun_huan_=1;
	//-------------------------------���п�ʶ��SPI�����ٶȲ��ܴ���400hz��
	SDXRML(CMD8);//д��CMD8����
	SDDQXY(CMD8.d);	//��ȡCMD8��Ӧ
	
	//-----���CMD8�Ƿ�Ϊ�Ƿ�������ȷ�����汾:�Ƿ�����(1):1.0����������;��Ч����(0):2.0��
	if(SDSFFFML==0x00)
	{
		a->a=0x02;
		//----------��Ŀ�꿨����Ϊ2.0�汾
		ACMD41.b[0]=0xc0;
	}
	
	SDXRML(CMD58);		//д��CMD58����
	SDDQXY(CMD58.d);	//��ȡCMD58��Ӧ
	while(sd_r1_xy!=0x00)
	{
		SDXRML(CMD55);		//д��CMD55����
		SDDQXY(CMD55.d);	//��ȡCMD55��Ӧ
		
		SDXRML(ACMD41);		//д��ACMD41����
		SDDQXY(ACMD41.d);	//��ȡCMD41��Ӧ
	}
	SDXRML(CMD58);		//д��CMD58����
	SDDQXY(CMD58.d);	//��ȡCMD58��Ӧ

	

	if(SDML58PD)
	{
	//�������õ����� 1������ 0��׼����
		if(SDML58PD2)
		a->b=1;
	}
	//��ȡ��CID
	SDXRML(CMD10);//д��CMD10����
	SDDQXY(CMD10.d);
	SDDQSJ(0,a->c,16,1);

	//��CSD
	SDXRML(CMD9);//д��CMD9����
	SDDQXY(CMD9.d);
	SDDQSJ(0,a->d,16,1);

	//������Ϣ
	SDXXCL(a);
	return 0;
}
//�˳�SD��
//�����ȡ����	a:��ȡ�Ŀ��ַ-	b:��ȡ�����ݴ�ŵĵ�ַ-	c��ȡ�Ŀ��С
ERROR SDDANDQSJ(lint a,uchar *b)
{
	CMD17.b[0]=(a>>24)&0x000000ff;
	CMD17.b[1]=(a>>16)&0x000000ff;
	CMD17.b[2]=(a>>8)&0x000000ff;
	CMD17.b[3]=a&0x000000ff;
	SDXRML(CMD17);
	SDDQXY(CMD17.d);
	if(sd_r1_xy==0)
	{
		return SDDQSJ(0,b,SDKDX,1);
	}
	return sd_r1_xy;
}
//����ȡ����a:��ȡ�Ŀ��ַ-b:��ȡ�����ݴ�ŵĵ�ַ-c��ȡ�Ŀ��С-d��ȡ�Ŀ�����
ERROR SDDUODQSJ(lint a,uchar *b,uint d)
{
	CMD18.b[0]=(a>>24)&0x000000ff;
	CMD18.b[1]=(a>>16)&0x000000ff;
	CMD18.b[2]=(a>>8)&0x000000ff;
	CMD18.b[3]=a&0x000000ff;
	SDXRML(CMD18);
	SDDQXY(CMD18.d);
	if(sd_r1_xy==0)
	{
		return SDDQSJ(1,b,SDKDX,d);
	}
	return sd_r1_xy;
}
//����д�뺯��a:д��Ŀ��ַ-b:д�������-c:д��Ŀ��С
ERROR SDDANXRSJ(lint a,uchar *b)
{
	uchar error_1=0;
	CMD24.b[0]=(a>>24)&0x000000ff;
	CMD24.b[1]=(a>>16)&0x000000ff;
	CMD24.b[2]=(a>>8)&0x000000ff;
	CMD24.b[3]=a&0x000000ff;
	SDXRML(CMD24);
	SDDQXY(CMD24.d);
	//���������ʱ���ش���
	if(sd_r1_xy==0)
	{
		SPI_XD_DATA(0xff);
		SPI_XD_DATA(0xfe);
		for(uint i=0;i<SDKDX;i++)
		{
			SPI_XD_DATA(b[i]);
		}
		//����CRC
		SPI_XD_DATA(0xff);
		SPI_XD_DATA(0xff);
		//���������Ӧ�ֽ�
		for (uint i = 0; i < 500; i++)
		{
			error_1=(SPI_XD_DATA(0xff)&0x1f);
			//�ж�������Ӧ����
			switch (error_1)
			{
			case 0x05 ://���ݳɹ�
					//�˳��ж�
					i=500;
					break;
			case 0x0b ://CRC����
				return error_1;
			case 0x0d ://д�����
				return error_1;		
			default://��������Ӧ����
				break;
			}
			if(i==60)
			return 0x55;
		}
		//��ʾæ�ź�
		while(SPI_XD_DATA(0xff)!=0xff);
		return 0;
	}
	return sd_r1_xy;
}
//���д�뺯��a:д��Ŀ��ַ-b:д�������-cд��Ŀ��С-dд��Ŀ�����
ERROR SDDUOXRSJ(lint a,uchar *b,uint dd)
{
	uchar error_1=0;
	CMD25.b[0]=(a>>24)&0x000000ff;
	CMD25.b[1]=(a>>16)&0x000000ff;
	CMD25.b[2]=(a>>8)&0x000000ff;
	CMD25.b[3]=a&0x000000ff;
	SDXRML(CMD25);
	SDDQXY(CMD25.d);
	if(sd_r1_xy==0)
	{
		for(uint i=0;i<dd;i++)
		{
			SPI_XD_DATA(0xff);
			SPI_XD_DATA(0xfc);
			for(uint ii=0;ii<SDKDX;ii++)
			{
				SPI_XD_DATA(b[512*i+ii]);
			}
			SPI_XD_DATA(0xff);//CRC
			SPI_XD_DATA(0xff);//CRC
			for (uint i = 0; i < 500; i++)
			{
				error_1=(SPI_XD_DATA(0xff)&0x1f);
				//�ж�������Ӧ����
				switch (error_1)
				{
					case 0x05 ://���ݳɹ�
							//�˳��ж�
							i=500;
							break;
					case 0x0b ://CRC����
						return error_1;
					case 0x0d ://д�����
						return error_1;		
					default://��������Ӧ����
						break;
				}
				if(i==60)
				return 0x55;
			}
			//���æ�ź�
			while((error_1=SPI_XD_DATA(0xff))!=0xff);	
		}
		//����ֹͣ����
		SPI_XD_DATA(0xfd);
		//���æ�ź�
		while((error_1=SPI_XD_DATA(0xff))!=0xff);
		return 0;
	}
	return sd_r1_xy;
}
//
