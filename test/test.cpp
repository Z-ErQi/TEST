#include <windows.h>
#include <process.h>
#include<cstdlib>
#include<ctime>

#include <iostream>
#include <fstream>
#define random(a,b) (rand()%(b-a+1)+a)

CRITICAL_SECTION _critical;

typedef struct Buffer {
	char data[24];
	int head;
}Buffer;

static Buffer buffer;
static bool isExit = false;
/*
void writeBuffer(const char *str)
{
	unsigned int i = 0;
	for(;i < strlen(str) ; i ++)
	{
		if (buffer.head >= 24)
			buffer.head = 0;
		EnterCriticalSection(& _critical);
		buffer.data[buffer.head] = str[i];
		buffer.head ++;
		LeaveCriticalSection(& _critical);
	}
}

void readBuffer(char *str,int len)
{
	int temp = 0;
	for (int i = 1;i <= len;i++)
	{
		if (!buffer.data[buffer.head - i])
		{
			str[len - i] = ' ';
			temp++;
			continue;
		}
		str[len - i] = buffer.data[buffer.head - i];
	}
	for (int i = 0; i < temp; i++)
	{
		str[i] = str[len - temp + i];
		str[len - temp + i] = ' ';
	}		
}
*/
void writeBuffer(const char *str)
{
	char *temp = buffer.data;
	temp += buffer.head;
	unsigned int len = 24 - buffer.head;
	
	if (len >= strlen(str))
	{		
		memcpy(temp, str, strlen(str));
		buffer.head += strlen(str);
	}
	else
	{
		memcpy(temp, str, len);
		const char *temp1 = str + len;
		memcpy(buffer.data, temp1, strlen(str) - len);
		buffer.head = strlen(str) - len;
	}
}

void readBuffer(char *str,int len)
{	
	char *temp = buffer.data , *tempStr = str;
	if (len > buffer.head)
	{
		temp = temp + (24 - len + buffer.head);
		memcpy(tempStr, temp, len - buffer.head);
		len = buffer.head;
		tempStr += len - buffer.head;
	}
	temp = temp + buffer.head - len;
	memcpy(tempStr, temp, len);

}

void __cdecl writeProc(void*)
{
	while (!isExit)
	{
		int len = random(2, 4);
		int tempInt = len;
		char temp[5] = { 0 };
		while (len)
		{
			len--;
			temp[len] = random('a', 'z');
		}
		temp[tempInt] = '\0';
		writeBuffer(temp);

		Sleep(1000);
	}
	_endthread();
}


void __cdecl readProc1(void*)
{
	std::ofstream of("b.txt", std::ios::app);
	while (!isExit)
	{
		char str[3];
		readBuffer(str, 2);
		str[2] = '\0';

		of << str << std::endl;
		Sleep(1000);
	}
	of.close();

	_endthread();
}


void __cdecl readProc2(void*)
{
	std::ofstream of("c.txt", std::ios::app);
	while (!isExit)
	{
		char str[5];
		readBuffer(str,4);
		str[4] = '\0';	
		of << str << std::endl;
		Sleep(1000);
	}
	of.close();

	_endthread();
}

int main(int argv,char *argc[]) {
	InitializeCriticalSection(& _critical);
	ZeroMemory(buffer.data, 24);
	buffer.head = 0;

	_beginthread(writeProc, 0, NULL);
	_beginthread(readProc1, 0, NULL);
	_beginthread(readProc2, 0, NULL);

	std::cout << "ÊäÈëexitÍË³ö³ÌÐò" << std::endl;
	std::string exit;
	while (1)
	{
		std::cin >> const_cast<char *>(exit.c_str());
		if (!strncmp(exit.c_str(), "exit", 4))
		{
			DeleteCriticalSection(&_critical);
			isExit = true;
			Sleep(500);
			break;
		}
	}

	
	return 0;
}
