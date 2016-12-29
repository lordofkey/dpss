// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "socket-lib/Socket.hpp"
#include <direct.h>


typedef int ssize_t;


#define  PORT 8145
#define  IP_ADDRESS "172.1.10.134"

MyQueue que;
typedef void (*PF)(char* s,int len, void* param);

class ipadr
{
public:
	char ip[20];
	int port;
};


void process(MyImg* pimg, Socket::TCP server)
{
	vector<uchar> buff;//buffer for coding
	char token[] = "1234567890";
	int anthresult;
	try
	{
		server.send<char>(token, strlen(token));
		server.receive<int>(&anthresult, 1);
		if(anthresult == 0)
			return;
	}
	catch (Socket::SocketException &e)
	{
		cout << e << endl;
		return;
	}
//	cv::imencode(".tiff", pimg->img, buff);
	int len[3];
	len[0] = strlen(pimg->m_name);
//	len[1] = buff.size();
	len[1] = pimg->img.rows;
	len[2] = pimg->img.cols;

	try
	{
		server.send<int>(len, 3);
		server.send<char>(pimg->m_name, strlen(pimg->m_name));
		server.send_1dbuffer(pimg->img.data, len[1]*len[2]);
		server.receive<char>(pimg->result, 128);
	}
	catch (Socket::SocketException &e)
	{
		cout << e << endl;
	}
}




DWORD WINAPI SendFun(LPVOID pM)  
{
	char *pre;
	MyImg *pimg;
	Socket::TCP server;


	ipadr* curipadr = (ipadr*)pM;

	while (true)
	{
		pimg = (MyImg *)que.pop();
		if(pimg == NULL)
		{
			Sleep(1);
			continue;
		}
		if(pimg->func == NULL)
		{
			Sleep(1);
			continue;
		}
		PF proresul = (PF)(pimg->func);

		if (!pimg->img.data)                              // Check for invalid input
		{
			cout << "Could not open or find the image" << std::endl;
			proresul("invalid img\n", 12, pimg->param);
			continue;
		}
		//TODO 图片检查
		try
		{
			server.connect_to(Socket::Address(curipadr->ip, curipadr->port));
			printf("ip:%s,port%d", curipadr->ip, curipadr->port);
		}
		catch (Socket::SocketException &e)
		{
			proresul("no server\n", 10, pimg->param);
			cout << e << endl;
//TODO回掉处理函数
			server.close();
			continue;
		}
		process(pimg, server);
		pre = new char[128];
		strcpy_s(pre, 128, pimg->result);
		proresul(pre, 128, pimg->param);
		delete pimg;
		pimg = NULL;
		server.close();
	}

}  
DWORD WINAPI RevFun(LPVOID pM)  
{
	return 0;
} 


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	HANDLE sendthread;
	ipadr* curipadr;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Socket::TCP::initcri();
		curipadr = new ipadr();
		char szModuleFileName[100];
		_getcwd(szModuleFileName, 100);
		strcat_s(szModuleFileName, 100, "\\setting.ini");
		GetPrivateProfileString("dpss", "ip", "172.1.10.134", curipadr->ip, 20, szModuleFileName);
		curipadr->port = GetPrivateProfileInt("dpss", "port", 8145, szModuleFileName);
		for(int i = 0; i < 5; i++)
		{
			sendthread = CreateThread(NULL, 0, SendFun, curipadr, 0, NULL);
//			revthread = CreateThread(NULL, 0, RevFun, NULL, 0, NULL);
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}