#include <stdio.h>
#include <Windows.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

char buffer[131072]; // ���ջ�����
int buffer_size = 0; // ���������ÿռ�
HANDLE hPort; // �򿪵Ĵ���

int convert_data(char *data, int len);
void save_file(const void *data, int len);

// �����ݽ���CRCУ��, len����CRCУ����ĳ���
int check_data(const void *data, int len)
{
	DWORD crc = 0xffffffff;
	DWORD temp;
	int i, j;
	
	for (i = 0; i < len; i += 4)
	{
		if (i <= len - 8 || i == len - 4)
			temp = *(LPDWORD)((LPBYTE)data + i);
		else
		{
			temp = 0;
			memcpy(&temp, (LPBYTE)data + i, len - i - 4);
			i = len - 8;
		}

		crc ^= temp;
		for (j = 0; j < 32; j++)
		{
			if (crc & 0x80000000)
				crc = (crc << 1) ^ 0x4c11db7;
			else
				crc <<= 1;
		}
	}

	return crc == 0;
}

void convert_all(void)
{
	char *pbuf = buffer;
	char *pend;
	int remaining_size = buffer_size;
	int section_size;
	
	while ((pend = (char *)memchr(pbuf, '\r', remaining_size)) != NULL) // �����з��ָ��յ�������
	{
		section_size = (int)(pend - pbuf); // ���ݴ�С
		if (convert_data(pbuf, section_size)) // �����������Ϊ��׼��ʮ�������ַ���
		{
			if (check_data(pbuf, section_size / 2)) // ����CRC
				save_file(pbuf, section_size / 2 - 4); // �����ļ�
		}

		// ����\r\n, ������������
		remaining_size -= section_size + 2;
		if (remaining_size <= 0) // ��\r\n��û��������, ��ֻ��\r, ��û��\r\n, ��ֹͣ����
			break;
		pbuf = pend + 2;
	}

	if (remaining_size > 0)
	{
		// δ���������ǰ��, Ϊ���յ��������ڳ��ռ�
		buffer_size = remaining_size;
		memmove(buffer, pbuf, remaining_size);
	}
	else
		buffer_size = 0;
}

// ��ʮ�������ַ�����ʽת���ɶ���������
int convert_data(char *data, int len)
{
	char ch;
	int i;
	for (i = 0; i < len; i++)
	{
		if (data[i] >= '0' && data[i] <= '9')
			ch = data[i] - '0';
		else if (data[i] >= 'A' && data[i] <= 'F')
			ch = data[i] - 'A' + 10;
		else
			return 0;

		if (i % 2 == 0)
			data[i / 2] = ch << 4;
		else
			data[i / 2] |= ch;
	}
	return 1;
}

// ����port.txtд�Ĵ��ںŴ򿪴���
int open_comm(void)
{
	char name[30];
	COMMTIMEOUTS timeouts = {0};
	DCB dcb = {0};
	FILE *fp;
	int port_id;

	// ��ȡ���ں�
	fopen_s(&fp, "port.txt", "r");
	if (fp == NULL)
	{
		printf("Cannot open port.txt!\n");
		return 0;
	}
	fscanf_s(fp, "COM%d", &port_id);
	sprintf_s(name, sizeof(name), "\\\\.\\COM%d", port_id);
	fclose(fp);

	// �򿪴���
	hPort = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, (DWORD)NULL, NULL, OPEN_EXISTING, (DWORD)NULL, NULL);
	if (hPort == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open COM%d!\n", port_id);
		return 0;
	}

	// ���ò�����
	dcb.BaudRate = CBR_115200;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.DCBlength = sizeof(DCB);
	if (!SetCommState(hPort, &dcb))
	{
		CloseHandle(hPort);
		printf("Cannot setup COM%d!\n", port_id);
		return 0;
	}

	// ���ó�ʱʱ��
	timeouts.ReadTotalTimeoutConstant = 20000;
	if (!SetCommTimeouts(hPort, &timeouts))
	{
		CloseHandle(hPort);
		printf("Cannot setup the timeouts of COM%d!\n", port_id);
		return 0;
	}
	return 1;
}

void save_file(const void *data, int len)
{
	char name[100];
	FILE *fp;
	size_t folder_len;
	SYSTEMTIME systime;

	// ���ݵ�ǰ���ڴ����ļ���
	GetLocalTime(&systime);
	sprintf_s(name, sizeof(name), "images/%d-%02d-%02d", systime.wYear, systime.wMonth, systime.wDay);
	if (!PathFileExistsA("images"))
		CreateDirectoryA("images", NULL);
	if (!PathFileExistsA(name))
		CreateDirectoryA(name, NULL);

	// ���ݵ�ǰʱ�������ļ���
	folder_len = strlen(name);
	sprintf_s(name + folder_len, sizeof(name) - folder_len, "/%d-%02d-%02d_%02d%02d%02d_%d.jpg", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
	folder_len++;

	// ����ͼ������
	fopen_s(&fp, name, "wb");
	if (fp == NULL)
	{
		printf("Cannot open file %s!\n", name + folder_len);
		return;
	}
	fwrite(data, 1, len, fp);
	fclose(fp);
	printf("%s (%d bytes)\n", name + folder_len, len);
}

int main(void)
{
	DWORD size;

	if (!open_comm())
		return 0;

	while (1)
	{
		WriteFile(hPort, "RECV", 4, &size, NULL); // �򴮿ڷ�������
		ReadFile(hPort, buffer + buffer_size, sizeof(buffer) - buffer_size, &size, NULL); // �Ӵ��ڽ�������
		if (size == 0)
			break;
		buffer_size += size;
		
		convert_all(); // ��ȡ��Ч���ݲ�����
		if (buffer_size == sizeof(buffer))
		{
			printf("Buffer is full! Drop!\n");
			buffer_size = 0;
		}
	}

	CloseHandle(hPort);
	return 0;
}
