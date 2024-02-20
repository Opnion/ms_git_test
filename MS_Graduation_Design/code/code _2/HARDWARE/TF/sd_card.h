#ifndef _sd_card_H_
#define _sd_card_H_
#define uchar unsigned char
#define uint unsigned int
#define lint unsigned long int

//SD卡参数定义-----------------------------------------------------------------------------------------
#define SDKXZTML (sd_r7_xy[0]&0x01)//检测卡状态;0卡准备就绪1空闲状态
#define SDCCFWML (sd_r7_xy[0]&0x02)//检测擦除复位;0清零:1:设置
#define SDSFFFML (sd_r7_xy[0]&0x04)//检测命令是否非法;0:正常命令1:非法命令
#define SDCRCML (sd_r7_xy[0]&0x08)//检测CRC是否错误;0:错误1:正确
#define SDCCXLML (sd_r7_xy[0]&0x10)//检测擦除命令序列是否错误;0:错误1:正确
#define SDDZCWML (sd_r7_xy[0]&0x20)//检测地址是否错误;0:地址错误1:地址正确
#define SDCSCWML (sd_r7_xy[0]&0x40)//检测参数是否错误;0:参数错误1:参数正确
#define SDML58PD (sd_r7_xy[1]&0x80)==1//当CMD58的响应第31位为1时表示上电完成0表示未完成
#define SDML58PD2 (sd_r7_xy[1]&0x40)==1//当CMD58的响应第30位为1时表示高容量0表示标准容量
#define ERROR 		unsigned char
#define ERROR_XY 	unsigned char
//命令类型
typedef struct{
	uchar  a;//'0'起始位+命令
	uchar  b[4];//命令参数
	uchar  c;//CRC+结束位'1'
	uchar  d;//响应类型
}SDML;
//SD卡类型
typedef struct{
	uchar a;//SD卡版本
	uchar b;//SD卡容量1大容量卡 0标准容量卡
	uchar c[16];//SD卡CID信息
	uchar d[16];//SD卡CSD信息
	uint  e;//SD卡的块长度
	uint  f;//SD卡的块数量
	uchar sd_card_zt;//SD卡状态
}SDCS;

//SD卡 SPI
void SDXRML(SDML);//sd卡写入命令s
ERROR_XY SDDQXY(uchar);//SD卡读取响应函数
ERROR SDDQSJ(uchar,uchar *,uint,uint);//读取数据函数
ERROR SDDANDQSJ(lint a,uchar *b);//单块读取
ERROR SDDUODQSJ(lint a,uchar *b,uint d);//多块读取
ERROR SDDANXRSJ(lint a,uchar *b);//单块写入
ERROR SDDUOXRSJ(lint a,uchar *b,uint d);//多块写入
ERROR SDCRADCSH(SDCS *);//SD卡SPI模式初始化
void END_SD(void);//SD卡结束通讯
void SDXXCL(SDCS *);//SD卡信息处理函数（对CSD CID读取的数据处理）

#endif

