#include "sd_card.h"
//0x55命令响应超时 0x44读数据超时
SDML CMD0={0x40,{0x00,0x00,0x00,0x00},0x95,0};//空闲模式
SDML CMD8={0x48,{0x00,0x00,0x01,0xaa},0x87,7};//读取电压
SDML CMD9={0x49,{0x00,0x00,0x00,0x00},0x01,0};//读取CSD
SDML CMD10={0x4a,{0x00,0x00,0x0,0x00},0x01,0};//读取CID
SDML CMD12={0x4c,{0x00,0x00,0x0,0x00},0x01,1};//结束多块读取命令
SDML CMD13={0x4d,{0x00,0x00,0x0,0x00},0x01,2};//读取状态
SDML CMD17={0x51,{0x00,0x00,0x0,0x00},0x01,0};//单块读取命令
SDML CMD18={0x52,{0x00,0x00,0x0,0x00},0x01,0};//多块读取命令
SDML CMD24={0x58,{0x00,0x00,0x0,0x00},0x01,0};//单块写入命令
SDML CMD25={0x59,{0x00,0x00,0x0,0x00},0x01,0};//多块写入命令
SDML CMD55={0x77,{0x00,0x00,0x00,0x00},0x01,0};//与ACMD命令组合使用
SDML CMD58={0x7a,{0x00,0x00,0x00,0x00},0x01,7};
SDML CMD59={0x7b,{0x00,0x00,0x00,0x00},0x01,0};//CRC打开/关闭
SDML ACMD41={0x69,{0x40,0x00,0x00,0x00},0x01,0};

uint SDKDX=0;//SD卡块大小
uchar SDCRC[2];//CRC数据
uchar r2_zhuang_tai=0;//R2响应状态字节存储
uchar sd_r1_xy=0;//r1响应存储
uchar sd_r7_xy[5];//r3/r7响应存储
//写入命令函数
void SDXRML(SDML a)
{
	SPI_XD_DATA(a.a);
	for(uint i=0;i<4;i++)
	{
		SPI_XD_DATA(a.b[i]);	
	}
	SPI_XD_DATA(a.c);
}
//读取响应函数
ERROR_XY SDDQXY(uchar a)
{
	int XY_BZW=0;
	uint i=0;
	switch(a)
	{
		case 0://R1响应
			while(XY_BZW<50)
			{
				XY_BZW++;
				sd_r1_xy=SPI_XD_DATA(0xff);
				if(sd_r1_xy==0x01||sd_r1_xy==0x00)
				{
					XY_BZW=50;
				}	
			}
			//当超时返回0x55
			if(XY_BZW==50)
				return 0x55;
			break;
		case 1://R1b响应
			sd_r1_xy=SPI_XD_DATA(0xff);
			while(XY_BZW<50)
			{
				XY_BZW++;
				if((sd_r1_xy&0x80)==0x00)
				{
					XY_BZW=50;
				}	
			}
			//当超时返回0x55
			if(XY_BZW!=50)
				return 0x55;
			break;
		case 2://R2响应
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
		case 3://R3响应
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
		case 4://R4响应
			break;
		case 5://R5响应
			break;
		case 7://R7响应
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
			//当超时返回0x55
			if(XY_BZW!=50)
				return 0x55;
			break;
	}
	return 0;
}
//a:读取类型(0:单块读取,1:多块读取)-b:读取的数据存放的地址-c读取的块大小-d读取的块数量
ERROR SDDQSJ(uchar a,uchar *b,uint c,uint d)
{
	int DU_QU_BZW=1;
	switch(a)
	{
		case 0://读取sd卡存储数据单块读取
			//读取开始字节
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
			//读取数据
			for(uint i=0;i<c;i++)
			{
				b[i]=SPI_XD_DATA(0xff);
			}
			SDCRC[0]=SPI_XD_DATA(0xff);
			SDCRC[1]=SPI_XD_DATA(0xff);
			while(SPI_XD_DATA(0xff)!=0xff);
			return 0;					
		case 1://读取sd卡存储数据多块读取
			for(uint i=0;i<d;i++)
			{
				DU_QU_BZW=1;
				//读取开始字节+超时检测
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
			SDXRML(CMD12);//写入CMD12命令停止读取数据
			SDDQXY(CMD12.d);
			return 0;
	}
	return 1;
}
//sd卡信息处理CID CSD寄存器
void SDXXCL(SDCS * a)
{
	//CSD信息处理
	//获取卡的块大小
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
	//获取卡的块数量(不包括保护区)用于计算卡容量等
	a->f=a->d[8]*256+a->d[9]+1;
	//设置块长度
	SDKDX=a->e;
	//sd卡通讯完成
	a->sd_card_zt=1;
}
//SD卡初始化函数
ERROR SDCRADCSH(SDCS * a)
{
	int ml_xun_huan_=1;
	//额外时钟
	for(uint i=0;i<14;i++)
	{
		SPI_XD_DATA(0xff);
	}
	//--------------------------------进入SPI模式（CMD0命令+片选低电平）
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);//将cs引脚拉低
	//CMD0指令
	while(ml_xun_huan_)
	{
		ml_xun_huan_++;//不能放在命令判断之后否则退不出循环
		SDXRML(CMD0);//写入CMD0命令
		SDDQXY(CMD0.d);
		//读取CMD0响应并判断
		if(sd_r1_xy==0x01)
		{
			//如果CMD0正常则
			ml_xun_huan_=0;
		}
		if(ml_xun_huan_>60)
		return 1;
	}
	ml_xun_huan_=1;
	//-------------------------------进行卡识别（SPI总线速度不能大于400hz）
	SDXRML(CMD8);//写入CMD8命令
	SDDQXY(CMD8.d);	//读取CMD8响应
	
	//-----检测CMD8是否为非法命令来确定卡版本:非法命令(1):1.0卡或其他卡;有效命令(0):2.0卡
	if(SDSFFFML==0x00)
	{
		a->a=0x02;
		//----------将目标卡设置为2.0版本
		ACMD41.b[0]=0xc0;
	}
	
	SDXRML(CMD58);		//写入CMD58命令
	SDDQXY(CMD58.d);	//读取CMD58响应
	while(sd_r1_xy!=0x00)
	{
		SDXRML(CMD55);		//写入CMD55命令
		SDDQXY(CMD55.d);	//读取CMD55响应
		
		SDXRML(ACMD41);		//写入ACMD41命令
		SDDQXY(ACMD41.d);	//读取CMD41响应
	}
	SDXRML(CMD58);		//写入CMD58命令
	SDDQXY(CMD58.d);	//读取CMD58响应

	

	if(SDML58PD)
	{
	//将卡设置的容量 1大容量 0标准容量
		if(SDML58PD2)
		a->b=1;
	}
	//读取卡CID
	SDXRML(CMD10);//写入CMD10命令
	SDDQXY(CMD10.d);
	SDDQSJ(0,a->c,16,1);

	//读CSD
	SDXRML(CMD9);//写入CMD9命令
	SDDQXY(CMD9.d);
	SDDQSJ(0,a->d,16,1);

	//处理信息
	SDXXCL(a);
	return 0;
}
//退出SD卡
//单块读取函数	a:读取的块地址-	b:读取的数据存放的地址-	c读取的块大小
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
//多块读取函数a:读取的块地址-b:读取的数据存放的地址-c读取的块大小-d读取的块数量
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
//单块写入函数a:写入的块地址-b:写入的数据-c:写入的块大小
ERROR SDDANXRSJ(lint a,uchar *b)
{
	uchar error_1=0;
	CMD24.b[0]=(a>>24)&0x000000ff;
	CMD24.b[1]=(a>>16)&0x000000ff;
	CMD24.b[2]=(a>>8)&0x000000ff;
	CMD24.b[3]=a&0x000000ff;
	SDXRML(CMD24);
	SDDQXY(CMD24.d);
	//当命令错误时返回错误
	if(sd_r1_xy==0)
	{
		SPI_XD_DATA(0xff);
		SPI_XD_DATA(0xfe);
		for(uint i=0;i<SDKDX;i++)
		{
			SPI_XD_DATA(b[i]);
		}
		//发送CRC
		SPI_XD_DATA(0xff);
		SPI_XD_DATA(0xff);
		//进入接收响应字节
		for (uint i = 0; i < 500; i++)
		{
			error_1=(SPI_XD_DATA(0xff)&0x1f);
			//判断数据响应令牌
			switch (error_1)
			{
			case 0x05 ://数据成功
					//退出判断
					i=500;
					break;
			case 0x0b ://CRC错误
				return error_1;
			case 0x0d ://写入错误
				return error_1;		
			default://非数据响应令牌
				break;
			}
			if(i==60)
			return 0x55;
		}
		//表示忙信号
		while(SPI_XD_DATA(0xff)!=0xff);
		return 0;
	}
	return sd_r1_xy;
}
//多块写入函数a:写入的块地址-b:写入的数据-c写入的块大小-d写入的块数量
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
				//判断数据响应令牌
				switch (error_1)
				{
					case 0x05 ://数据成功
							//退出判断
							i=500;
							break;
					case 0x0b ://CRC错误
						return error_1;
					case 0x0d ://写入错误
						return error_1;		
					default://非数据响应令牌
						break;
				}
				if(i==60)
				return 0x55;
			}
			//检测忙信号
			while((error_1=SPI_XD_DATA(0xff))!=0xff);	
		}
		//发送停止令牌
		SPI_XD_DATA(0xfd);
		//检测忙信号
		while((error_1=SPI_XD_DATA(0xff))!=0xff);
		return 0;
	}
	return sd_r1_xy;
}
//
