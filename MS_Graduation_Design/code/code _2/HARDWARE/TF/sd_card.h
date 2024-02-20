#ifndef _sd_card_H_
#define _sd_card_H_
#define uchar unsigned char
#define uint unsigned int
#define lint unsigned long int

//SD����������-----------------------------------------------------------------------------------------
#define SDKXZTML (sd_r7_xy[0]&0x01)//��⿨״̬;0��׼������1����״̬
#define SDCCFWML (sd_r7_xy[0]&0x02)//��������λ;0����:1:����
#define SDSFFFML (sd_r7_xy[0]&0x04)//��������Ƿ�Ƿ�;0:��������1:�Ƿ�����
#define SDCRCML (sd_r7_xy[0]&0x08)//���CRC�Ƿ����;0:����1:��ȷ
#define SDCCXLML (sd_r7_xy[0]&0x10)//���������������Ƿ����;0:����1:��ȷ
#define SDDZCWML (sd_r7_xy[0]&0x20)//����ַ�Ƿ����;0:��ַ����1:��ַ��ȷ
#define SDCSCWML (sd_r7_xy[0]&0x40)//�������Ƿ����;0:��������1:������ȷ
#define SDML58PD (sd_r7_xy[1]&0x80)==1//��CMD58����Ӧ��31λΪ1ʱ��ʾ�ϵ����0��ʾδ���
#define SDML58PD2 (sd_r7_xy[1]&0x40)==1//��CMD58����Ӧ��30λΪ1ʱ��ʾ������0��ʾ��׼����
#define ERROR 		unsigned char
#define ERROR_XY 	unsigned char
//��������
typedef struct{
	uchar  a;//'0'��ʼλ+����
	uchar  b[4];//�������
	uchar  c;//CRC+����λ'1'
	uchar  d;//��Ӧ����
}SDML;
//SD������
typedef struct{
	uchar a;//SD���汾
	uchar b;//SD������1�������� 0��׼������
	uchar c[16];//SD��CID��Ϣ
	uchar d[16];//SD��CSD��Ϣ
	uint  e;//SD���Ŀ鳤��
	uint  f;//SD���Ŀ�����
	uchar sd_card_zt;//SD��״̬
}SDCS;

//SD�� SPI
void SDXRML(SDML);//sd��д������s
ERROR_XY SDDQXY(uchar);//SD����ȡ��Ӧ����
ERROR SDDQSJ(uchar,uchar *,uint,uint);//��ȡ���ݺ���
ERROR SDDANDQSJ(lint a,uchar *b);//�����ȡ
ERROR SDDUODQSJ(lint a,uchar *b,uint d);//����ȡ
ERROR SDDANXRSJ(lint a,uchar *b);//����д��
ERROR SDDUOXRSJ(lint a,uchar *b,uint d);//���д��
ERROR SDCRADCSH(SDCS *);//SD��SPIģʽ��ʼ��
void END_SD(void);//SD������ͨѶ
void SDXXCL(SDCS *);//SD����Ϣ����������CSD CID��ȡ�����ݴ���

#endif

