#include <stdio.h>
#include <Windows.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

char buffer[131072]; // 接收缓冲区
int buffer_size = 0; // 缓冲区已用空间
HANDLE hPort; // 打开的串口

int convert_data(char *data, int len);
void save_file(const void *data, int len);

// 对数据进行CRC校验, len包括CRC校验码的长度
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
	
	while ((pend = (char *)memchr(pbuf, '\r', remaining_size)) != NULL) // 按换行符分割收到的数据
	{
		section_size = (int)(pend - pbuf); // 数据大小
		if (convert_data(pbuf, section_size)) // 如果数据内容为标准的十六进制字符串
		{
			if (check_data(pbuf, section_size / 2)) // 检验CRC
				save_file(pbuf, section_size / 2 - 4); // 保存文件
		}

		// 跳过\r\n, 继续往后搜索
		remaining_size -= section_size + 2;
		if (remaining_size <= 0) // 若\r\n后没有数据了, 或只有\r, 或没有\r\n, 则停止搜索
			break;
		pbuf = pend + 2;
	}

	if (remaining_size > 0)
	{
		// 未处理的数据前移, 为新收到的数据腾出空间
		buffer_size = remaining_size;
		memmove(buffer, pbuf, remaining_size);
	}
	else
		buffer_size = 0;
}

// 将十六进制字符串格式转换成二进制数据
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

// 根据port.txt写的串口号打开串口
int open_comm(void)
{
	char name[30];
	COMMTIMEOUTS timeouts = {0};
	DCB dcb = {0};
	FILE *fp;
	int port_id;

	// 获取串口号
	fopen_s(&fp, "port.txt", "r");
	if (fp == NULL)
	{
		printf("Cannot open port.txt!\n");
		return 0;
	}
	fscanf_s(fp, "COM%d", &port_id);
	sprintf_s(name, sizeof(name), "\\\\.\\COM%d", port_id);
	fclose(fp);

	// 打开串口
	hPort = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, (DWORD)NULL, NULL, OPEN_EXISTING, (DWORD)NULL, NULL);
	if (hPort == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open COM%d!\n", port_id);
		return 0;
	}

	// 设置波特率
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

	// 设置超时时间
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

	// 根据当前日期创建文件夹
	GetLocalTime(&systime);
	sprintf_s(name, sizeof(name), "images/%d-%02d-%02d", systime.wYear, systime.wMonth, systime.wDay);
	if (!PathFileExistsA("images"))
		CreateDirectoryA("images", NULL);
	if (!PathFileExistsA(name))
		CreateDirectoryA(name, NULL);

	// 根据当前时间生成文件名
	folder_len = strlen(name);
	sprintf_s(name + folder_len, sizeof(name) - folder_len, "/%d-%02d-%02d_%02d%02d%02d_%d.jpg", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
	folder_len++;

	// 保存图像数据
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
		WriteFile(hPort, "RECV", 4, &size, NULL); // 向串口发送数据
		ReadFile(hPort, buffer + buffer_size, sizeof(buffer) - buffer_size, &size, NULL); // 从串口接收数据
		if (size == 0)
			break;
		buffer_size += size;
		
		convert_all(); // 提取有效数据并保存
		if (buffer_size == sizeof(buffer))
		{
			printf("Buffer is full! Drop!\n");
			buffer_size = 0;
		}
	}

	CloseHandle(hPort);
	return 0;
}
